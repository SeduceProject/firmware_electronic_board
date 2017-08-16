import threading
from serial import Serial


class SerialCaptureThread(threading.Thread):

    def __init__(self, label, serial_device_path, handle_message_func):
        threading.Thread.__init__(self)
        self.label = label
        self.serial_device_path = serial_device_path
        self.handle_message_func = handle_message_func
        self.serial_port = None

    def run(self):
        with Serial(port=self.serial_device_path, baudrate=9600, timeout=1, writeTimeout=1) as serial_port:
            self.serial_port = serial_port
            if serial_port.isOpen():
                while True:
                    msg = serial_port.readline()
                    if msg != "":
                        self.handle_message_func(self, msg.strip())
