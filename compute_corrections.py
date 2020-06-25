import sys
from influxdb import InfluxDBClient
import pytz
import datetime as DT
import numpy
import math
import yaml


DB_HOST = "127.0.0.1"
DB_PORT = 8086
DB_USER = "root"
DB_PASSWORD = "root"
DB_NAME = "pidiou"


def get_calibration_description():
    result = {}
    with open("conf/calibration.yaml") as f:
        cards = yaml.load(f)
        result = cards
    return result


def get_cards_description():
    result = {}
    with open("conf/sensors.yaml") as f:
        cards = yaml.load(f)
        result = cards
    return result


def get_values(sensor_name, start_epoch, end_epoch):
    db_client = InfluxDBClient(DB_HOST, DB_PORT, DB_USER, DB_PASSWORD, DB_NAME)

    result = []

    query = "SELECT * " \
            "FROM sensors " \
            "WHERE sensor = '%s' and time >= %ss and time <= %ss" % (sensor_name, start_epoch, end_epoch)

    points = db_client.query(query).get_points()

    for point in points:
        result += [{
            "x": DT.datetime.strptime(point.get("time"), '%Y-%m-%dT%H:%M:%SZ'),
            "y": point.get("value")
        }]

    return result


def datetime_to_epoch(datime_str, convert_local_time=True):
    text = datime_str
    tz = pytz.timezone('Europe/Paris')
    date = DT.datetime.strptime(text, '%Y-%m-%d %H:%M:%S')

    if convert_local_time:
        date = tz.localize(date)

    if convert_local_time:
        epoch = DT.datetime(1970, 1, 1, tzinfo=pytz.utc)
    else:
        epoch = DT.datetime(1970, 1, 1)
    timestamp = (date - epoch).total_seconds()
    return timestamp


def datetime_exp_computer_to_epoch(datime_str, convert_local_time=True):
    text = datime_str
    tz = pytz.timezone('Europe/Paris')
    date = DT.datetime.strptime(text, '%d-%m-%y_%H:%M:%S,%f')

    if convert_local_time:
        date = tz.localize(date)

    if convert_local_time:
        epoch = DT.datetime(1970, 1, 1, tzinfo=pytz.utc)
    else:
        epoch = DT.datetime(1970, 1, 1)
    timestamp = (date - epoch).total_seconds()
    return timestamp


if __name__ == "__main__":

    cards_corrections = {

    }

    # The following variable represents the difference of time between
    # the remote experimental computer (which is not using network time)
    # and the NTP time.
    #
    # A positive value means that the remote computer's time is in the past compared
    # with the NTP time. A negative time, means that the experimental computer's
    # time is in advance (in the future).
    # time_delay_of_ds2e_computer_s = -23
    time_delay_of_ds2e_computer_s = 0

    series = []

    sensor_cards = get_cards_description()
    experiments = get_calibration_description()

    for card in sensor_cards:

        cards_corrections[card] = {}
        corrections = cards_corrections[card]

        SENSORS = sensor_cards[card]

        for (exp_k, exp_v) in [(k,v) for (k,v) in experiments.items() if card in v["cards"]]:
            corrections[exp_k] = {
                "corrections": {}
            }
            exp_corrections = corrections[exp_k]["corrections"]
            stabilization_duration_s = exp_v["stabilization_duration_s"]

            current_targets = exp_v["targets"]

            for sensor in SENSORS:
                exp_corrections[sensor] = []

            for (temperature_target, datetime_str) in current_targets.items():
                end_stabilization_datetime = datetime_exp_computer_to_epoch(datetime_str)
                end_stabilization_datetime_with_offset = int(end_stabilization_datetime + time_delay_of_ds2e_computer_s)
                start_stabilization_datetime_with_offset = int(
                    end_stabilization_datetime_with_offset - stabilization_duration_s)
                cpt = 1
                for sensor in SENSORS:
                    points = get_values(sensor,
                                        start_stabilization_datetime_with_offset,
                                        end_stabilization_datetime_with_offset)

                    x = [p.get("x") for p in points]
                    y = [p.get("y") for p in points]

                    temperature_mean = numpy.mean(y)
                    temperature_std = numpy.std(y)

                    y_sorted = sorted(y, key=lambda y: (y - temperature_mean) * (y - temperature_mean))
                    errors_sorted = [math.sqrt((y - temperature_mean) * (y - temperature_mean)) for y in y_sorted]

                    # We filter the 15% of worst errors
                    filter_proportion = 0.15
                    filtered_value_count = int((1.0 - filter_proportion) * len(y_sorted))
                    filtered_y = y_sorted[0:filtered_value_count]

                    filtered_temperature_mean = numpy.mean(filtered_y)
                    filtered_temperature_std = numpy.std(filtered_y)

                    correction = temperature_target - filtered_temperature_mean

                    exp_corrections[sensor] += [correction]

                    cpt += 1

            if "corrected_with" in exp_v:
                exp_used_for_correction_id = exp_v["corrected_with"]
                exp_used_for_correction = corrections[exp_used_for_correction_id]
                for sensor in SENSORS:
                    cpt = 0
                    for (temperature_target, datetime_str) in current_targets.items():
                        temperature_correction = exp_used_for_correction["corrections"][sensor][cpt]
                        temperature_correction = round(temperature_correction, 2)
                        exp_corrections[sensor][cpt]

                        # Do the correction
                        exp_corrections[sensor][cpt] = exp_corrections[sensor][cpt] + temperature_correction
                        cpt += 1

    for card in sensor_cards:

        corrections = cards_corrections[card]
        SENSORS = sensor_cards[card]

        # Compute standard deviation of corrections
        corrections_mean = {}
        for sensor in SENSORS:
            corrections_mean[sensor] = []
        for sensor in SENSORS:
            cpt = 0
            for (temperature_target, datetime_str) in current_targets.items():
                sensor_temperature_target_corrections = [v["corrections"][sensor][cpt] for (c, v) in corrections.items()]
                sensor_temperature_target_corrections_std = numpy.mean(sensor_temperature_target_corrections)
                corrections_mean[sensor] += [sensor_temperature_target_corrections_std]
                cpt += 1

        output = "// Correction for card '%s'\n" % (card)
        output += "#define NB_SENSORS %d\n" % (len(SENSORS))
        output += "#define NB_TEMPERATURE_ENTRIES %d\n" % (len(current_targets))
        output += "float calibrationMap[NB_SENSORS][NB_TEMPERATURE_ENTRIES] = {\n"
        for (sensor_id, sensor_corrections_std) in corrections_mean.items():
            newline = "  {%s},\n" % (",".join(["%6.2f" % y for y in sensor_corrections_std]))
            newline = newline.replace("nan", "0 /* warning: there was an issue with this value... */")
            output += newline
        output += "};\n"

        print(output)

    sys.exit(0)