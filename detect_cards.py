from docopt import docopt
import os
import re
import sys
import moteino.moteino as moteino
import json
import serial
import threading
from Queue import Queue


sensor_cards = {
    "card1": [
        "3bac125909fc4c88",
        "3b67b95809fc0c65",
        "3bbefa5809fc2cb3",
        "3bd10a5909fc6c3b",
        "3bee125909fc4c93",
        "3b730c5909fc6c95",
        "3b65b75809fc0ca9",
        "3b5cf95809fc2cba",
        "3ba1125909fc4cc2",
        "3b1bb95809fc0c80",
        "3b41065909fc2ced",
        "3bb8f85809fc2c82",
        "3b630a5909fc2c14",
        "3bcef85809fc2ca8",
        "3bd8f85809fc2c41",
        "3b10f95809fc2cb2"
    ],
    "card2": [
        "3b3c0c5909fc6cc4",
        "3b46b95809fc0ce4",
        "3b46f95809fc2c2e",
        "3b10b95809fc0c78",
        "3b880c5909fc6c1f",
        "3bea055909fc2c07",
        "3bbf0c5909fc6c77",
        "3b98fb5809fc2c7a",
        "3bc70a5909fc6cd2",
        "3b360a5909fc2cd1",
        "3bb4b95809fc0cf8",
        "3be9095909fc2c7f",
        "3bcb0c5909fc6c33",
        "3b670c5909fc6c12",
        "3b71f95809fc2c46",
        "3be70a5909fc6c64"],
    "card3": [
        "3bbf055909fc2cc2",
        "3bf90c5909fc6cb0",
        "3b35fb5809fc2c6c",
        "3b1f065909fc2cd0",
        "3bd70c5909fc6c15",
        "3bdeb95809fc0cf4",
        "3baefb5809fc2c25",
        "3ba90c5909fc6c9e",
        "3b88055909fc2caa",
        "3b4bfb5809fc2ce7",
        "3b96125909fc4caa",
        "3bf20a5909fc6cd4",
        "3b93055909fc2c09",
        "3b7cf95809fc2c0c",
        "3b9e055909fc2c43",
        "3b77065909fc2cb2"],
    "card4": [
        "3baf0a5909fc2cf6",
        "3badf85809fc2c32",
        "3b4d0a5909fc2cb1",
        "3b1a0c5909fc6cc0",
        "3b40fb5809fc2c1f",
        "3bd3095909fc2c5d",
        "3bb9fb5809fc2cfb",
        "3ba9055909fc2c2b",
        "3b1bf95809fc2c4a",
        "3bb3fa5809fc2cf9",
        "3ba9b95809fc0ce9",
        "3bc8fa5809fc2c99",
        "3b510c5909fc6c4d",
        "3b82fb5809fc2cee",
        "3b040d5909fc6c45",
        "3bf2fa5809fc2cbb"],
    "card5": [
        "3bd1b75809fc0c72",
        "3bc6b75809fc0cac",
        "3ba6b75809fc0c6f",
        "3b96f85809fc2c27",
        "3bbbb75809fc0c7e",
        "3b8e0a5909fc2c77",
        "3b4fb75809fc0cd0",
        "3b9cb75809fc0c4d",
        "3b990a5909fc2ca9",
        "3b580a5909fc2c01",
        "3b210a5909fc2c0f",
        "3b14fb5809fc2ced",
        "3b010a5909fc2cb9",
        "3b05f95809fc2c02",
        "3b61065909fc2c5b",
        "3bb8125909fc4c0f"],
    "card6": [
        "3b3ab75809fc0ca3",
        "3b77fb5809fc2c77",
        "3b26f95809fc2ced",
        "3b0a065909fc2c60",
        "3b88b95809fc0c68",
        "3b55fb5809fc2caf",
        "3bc2125909fc4c58",
        "3b5cb95809fc0c70",
        "3b8dfb5809fc2cca",
        "3b36065909fc2cf0",
        "3bd4055909fc2cf9",
        "3bdf055909fc2c01",
        "3baf0a5909fc6cb0",
        "3b94b95809fc0c4e",
        "3b51b95809fc0c3a",
        "3b14065909fc2c28"],
    "card8": [
        "3bdf095909fc2c20", # CARTE 8 ?
        "3ba3fb5809fc2c6f",
        "3ba2f85809fc2c16",
        "3b7bb75809fc0ce1",
        "3bb1b75809fc0cb1",
        "3b2fb75809fc0c13",
        "3be4f85809fc2cd1",
        "3b5ab75809fc0c60",
        "3bfaf85809fc2c99",
        "3b10135909fc4c3f",
        "3b6c065909fc2c11",
        "3b56065909fc2c33",
        "3b190d5909fc6c54",
        "3b82065909fc2c2b", # PB
        "3b7e0c5909fc6cdf",
        "3b30b95809fc0cce", # PB
    ],
    "card9": [
        "3b6f0a5909fc2c69", # CARTE 9?
        "3b91b75809fc0c07",
        "3b0fb75809fc0ca5",
        "3b2c0a5909fc2c45",
        "3b1ab75809fc0c15",
        "3b840a5909fc2cb8",
        "3b00065909fc2caf",
        "3b29065909fc2c8f",
        "3b61fb5809fc2c9e",
        "3b25b95809fc0c7e",
        "3bc3f85809fc2ce2",
        "3b72b95809fc0cd5",
        "3bce125909fc4c25",
        "3bb40c5909fc6c8f",
        "3bf5055909fc2c78",
        "3be20c5909fc6c13",
    ],
    "card_tc_test": [
        "3b67f95809fc2caf",
        "3b420a5909fc2c95",
        "3b7a0a5909fc2cd9",
        "3be3125909fc4cd9",
        "3b05b75809fc0c6a",
        "3bf0b65809fc0c3e",
        "3b25b75809fc0cdc",
        "3b31f95809fc2c33",
        "3b0e0c5909fc6c47",
        "3b0b0a5909fc2c76",
        "3bd4b95809fc0c3b",
        "3b240c5909fc6c3e",
        "3b5c0c5909fc6c07",
        "3bbfb95809fc0c00",
        "3b9f0c5909fc6cc1",
        "3b51f95809fc2cf0",
    ],
    "ds18b20_exterior": [
        "28b8fb2909000003"
    ]
}


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

    #create a list of threads
    threads = []
        
    for card_name, sensors in sensor_cards.iteritems():
        if one_address in sensors:
            print("  port %s is mapped with card '%s' (used sensor '%s')" % (complete_port_path, card_name, one_address))

def list_cards():
    for port in get_serial_ports():
        complete_port_path = "/dev/%s" % (port)
        scan_serial_port(complete_port_path)


if __name__ == "__main__":

    help = """Card detector

    Usage:
      card_detector.py [--list-cards]

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
