from docopt import docopt
import os
import re
import sys
import moteino.moteino as moteino


def get_serial_ports():
    return [f for f in os.listdir('/dev') if re.match(r'.*tty(\.|.*USB|.*usb).*', f)]


def get_firmwares():
    return [x["name"] for x in moteino.get_ino_projects()]


if __name__ == "__main__":

    help = """Firmware uploader

    Usage:
      upload_firmware.py --firmware=<firmware> --serial=<serial>
      upload_firmware.py --list-firmwares
      upload_firmware.py --list-serial

    Options:
      -h --help          Show this message and exit.
      --list-firmwares   List firmwares that can be uploaded.
      --list-serial      List serial ports that can be crawled.
    """
    arguments = docopt(help)

    if arguments["--list-firmwares"]:
        print("Available firmwares:")
        for firmware in get_firmwares():
            print("  %s" % firmware)
        pass
        sys.exit(0)
    elif arguments["--list-serial"]:
        devices = moteino.get_devices()
        print("Available serial ports:")
        for device in devices:
            print("  %s" % (device["location"]))
        pass
        sys.exit(0)
    else:
        # Get parameters
        firmware = arguments["--firmware"]
        serial = arguments["--serial"]

        # Identify USB devices
        usb_devices = moteino.get_usb_devices()

        if len(usb_devices) < 1:
            print("ERROR: this script needs at least 1 device to illustrate ethernet and RF69")
            sys.exit(1)

        usb_serial_device_candidates = [x for x in usb_devices if x["location"] == serial]
        if len(usb_serial_device_candidates) == 0:
            print("Error: could not detect the following serial device '%s'" % (serial))
            sys.exit(1)
        usb_serial_device = usb_serial_device_candidates[0]

        print("Gateway usb device: %s" % (usb_serial_device))
        print(usb_devices)

        node_id = 1

        # Compile the "Programmer" ino file
        firmware_program = moteino.get_ino_project(firmware)
        firmware_compilation_result = moteino.compile_ino_project(
            firmware_program,
            variables={
                "node_id": node_id,
                "blink_period": 100
            }
        )

        # Upload "Gateway" on the given moteino
        moteino.upload_code(firmware_compilation_result, usb_serial_device)

        sys.exit(0)
