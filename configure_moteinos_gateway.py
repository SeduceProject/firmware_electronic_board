import moteino.moteino as moteino
import sys
from moteino.capture.serial_capture import SerialCaptureThread

if __name__ == "__main__":
    print "Running server"

    node_id = 1

    # Compile the "Programmer" ino file
    gateway_program = moteino.get_ino_project("Gateway")
    gateway_compilation_result = moteino.compile_ino_project(gateway_program,
                                                             variables={
                                                                    "node_id": node_id,
                                                                    "blink_period": 100
                                                                })

    # Identify USB devices
    usb_devices = moteino.get_usb_devices()

    if len(usb_devices) < 1:
        print("ERROR: this script needs at least 1 device to illustrate ethernet and RF69")
        sys.exit(1)

    gateway_usb_device = usb_devices[0]
    # target_usb_device = usb_devices[1]

    print("Gateway usb device: %s" % (gateway_usb_device))
    # print("Target usb device: %s" % (target_usb_device))
    print(usb_devices)

    # capture first USB device
    def msg_handler(instance, msg):
        print("%s: > %s" % (instance.label, msg))

    # Upload "Gateway" on the given moteino
    moteino.upload_code(gateway_compilation_result, gateway_usb_device)

    # # Capture serial port of "Gateway"
    # capture_thread_programmer = SerialCaptureThread("gateway",
    #                                                 gateway_usb_device["location"],
    #                                                 msg_handler,
    #                                                 baudrate=9600)
    # capture_thread_programmer.start()
    # # capture_thread_programmer.join()

    # while True:
    #     x = raw_input("Press a key when you have connected a new sensor on an USB port")
    #
    #     node_id += 1
    #     print("I will configure a new sensor (node_id=%i)" % (node_id))
    #
    #     # Compile the "Programmer" ino file
    #     sensor_program = moteino.get_ino_project("Sensor")
    #     sensor_compilation_result = moteino.compile_ino_project(sensor_program,
    #                                                             variables={
    #                                                                 "node_id": node_id,
    #                                                                 "blink_period": 100
    #                                                             })
    #
    #     # Identify USB devices
    #     usb_devices = moteino.get_usb_devices()
    #
    #     if len(usb_devices) < 1:
    #         print("ERROR: this script needs at least 1 device to illustrate ethernet and RF69")
    #         sys.exit(1)
    #
    #     sensor_usb_device = usb_devices[-1]
    #     # target_usb_device = usb_devices[1]
    #
    #     print("Sensor usb device: %s" % (sensor_usb_device))
    #     # print("Target usb device: %s" % (target_usb_device))
    #     print(usb_devices)
    #
    #     # Upload "Programmer" and "Target" programs on two moteino
    #     moteino.upload_code(sensor_compilation_result, sensor_usb_device)

    sys.exit(0)
