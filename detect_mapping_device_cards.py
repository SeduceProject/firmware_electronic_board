from docopt import docopt
import os
import re
import sys
import serial
import yaml


def get_cards_description():
    result = {}
    with open("conf/sensors.yaml") as f:
        cards = yaml.load(f)
        result = cards
    return result


def get_serial_ports():
    return [f for f in os.listdir('/dev') if re.match(r'.*tty(\.|.*USB|.*usb).*', f)]


def detect_one_sensor_address_on_serial_port(serial_path):

    serial_device_path = serial_path
    baudrate = 9600

    result = None
    with serial.Serial(port=serial_device_path, baudrate=baudrate, timeout=1, writeTimeout=1) as serial_port:
        if serial_port.isOpen():
            while result is None:
                msg = serial_port.readline()
                if msg != "" and "28b8fb2909000003" in msg:
                    serial_port.close()
                    result = "28b8fb2909000003"
                elif msg != "" and " Sensor " in msg and "=" in msg:
                    sensor_label, sensor_address = [s.strip() for s in msg.strip().split("=")]
                    nothing, sensor_idx = [s.strip() for s in sensor_label.strip().split(" ")]
                    result = sensor_address
                    serial_port.close()
    return result


def scan_serial_port(complete_port_path):
    one_address = detect_one_sensor_address_on_serial_port(complete_port_path)

    for card_name, sensors in get_cards_description().iteritems():
        if one_address in sensors:
            print("  port %s is mapped with card '%s' (used sensor '%s')" % (complete_port_path, card_name, one_address))


def list_cards():
    for port in get_serial_ports():
        complete_port_path = "/dev/%s" % (port)
        scan_serial_port(complete_port_path)


if __name__ == "__main__":

    help = """Script that detects the mapping between electronic cards connected to the computer, and its associated linux device. 

    Usage:
      detect_mapping_device_cards.py [--list-cards]

    Options:
      -h --help          Show this message and exit.
      --list-cards       List cards.
    """
    arguments = docopt(help)

    if "--list-cards" in arguments:
        list_cards()
        sys.exit(0)
    else:
        print(help)
        sys.exit(0)
