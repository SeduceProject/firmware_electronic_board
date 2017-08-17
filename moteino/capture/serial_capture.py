import threading
from serial import Serial
import time


class SerialCaptureThread(threading.Thread):

    def __init__(self, label, serial_device_path, handle_message_func, baudrate=None):
        threading.Thread.__init__(self)
        self.label = label
        self.serial_device_path = serial_device_path
        self.handle_message_func = handle_message_func
        self.serial_port = None
        self.baudrate = baudrate if baudrate is not None else 9600

    def run(self):
        with Serial(port=self.serial_device_path, baudrate=self.baudrate, timeout=1, writeTimeout=1) as serial_port:
            self.serial_port = serial_port
            if serial_port.isOpen():
                while True:
                    msg = serial_port.readline()
                    if msg != "":
                        self.handle_message_func(self, msg.strip())

    def write(self, msg):
        while self.serial_port is None:
            time.sleep(50)
        self.serial_port.write(msg)
