import moteino.moteino as moteino
import sys


if __name__ == "__main__":
    print "Running server"

    # Compile the "Programmer" ino file
    programmer_program = moteino.get_ino_project("Programmer")
    programmer_compilation_result = moteino.compile_ino_project(programmer_program,
                                                                variables={
                                                                    "node_id": 1,
                                                                    "blink_period": 100
                                                                })

    # Compile the "Target" ino file
    target_program = moteino.get_ino_project("Target")
    target_compilation_result = moteino.compile_ino_project(target_program,
                                                            variables={
                                                                "node_id": 2,
                                                                "blink_period": 100
                                                            })

    # Identify USB devices
    usb_devices = moteino.get_usb_devices()

    if len(usb_devices) < 2:
        print("ERROR: this script needs at least 2 devices to illustrate wireless programming")
        sys.exit(1)

    programmer_usb_device = usb_devices[0]
    target_usb_device = usb_devices[1]

    print("Programmer usb device: %s" % (programmer_usb_device))
    print("Target usb device: %s" % (target_usb_device))
    print(usb_devices)

    # capture first USB device
    def msg_handler(instance, msg):
        print("%s: > %s" % (instance.label, msg))

    # Upload "Programmer" and "Target" programs on two moteino
    moteino.upload_code(programmer_compilation_result, programmer_usb_device)
    moteino.upload_code(target_compilation_result, target_usb_device)

    print("Waiting 20 seconds")

    import time
    time.sleep(20)

    print("Will now try to wireless update the \"Target\" arduino (1)")

    #############################################################
    # Try to make an OTA update of the "Target" arduino
    #############################################################

    # Recompile the "Target" ino file
    target_recompilation_result = moteino.compile_ino_project(target_program,
                                                              variables={
                                                                  "node_id": 2,
                                                                  "blink_period": 2500
                                                              })
    moteino.wireless_upload_code(target_compilation_result,
                                 target_usb_device=target_usb_device,
                                 programmer_usb_device=programmer_usb_device,
                                 programmer_baudrate=115200)
    time.sleep(20)

    print("Will now try to wireless update the \"Target\" arduino (2)")

    #############################################################
    # Try to make an OTA update of the "Target" arduino
    #############################################################

    # Recompile the "Target" ino file
    target_recompilation_result = moteino.compile_ino_project(target_program,
                                                              variables={
                                                                  "node_id": 2,
                                                                  "blink_period": 100
                                                              })
    moteino.wireless_upload_code(target_compilation_result,
                                 target_usb_device=target_usb_device,
                                 programmer_usb_device=programmer_usb_device,
                                 programmer_baudrate=115200)

    # Capture serial port of "Programmer" and "Target"
    # capture_thread_programmer = SerialCaptureThread("programmer", programmer_usb_device["location"], msg_handler, baudrate=115200)
    # capture_thread_programmer.start()
    #
    # capture_thread_target = SerialCaptureThread("target", target_usb_device["location"], msg_handler, baudrate=115200)
    # capture_thread_target.start()

    sys.exit(0)
