import platformio
import sys
import os
import shutil
import jinja2
import moteino.config as moteino_config
import subprocess
from moteino.capture.serial_capture import SerialCaptureThread

PLATFORM = "atmelavr"
BOARD = "moteino"

capture_threads = []


def get_ino_projects():
    return moteino_config.config["ino_projects"]


def get_ino_project(program_name):
    candidates = filter(lambda x: x["name"] == program_name, moteino_config.config["ino_projects"])
    if len(candidates) > 0:
        return candidates[0]
    else:
        return None


def _render_jinja_template(template_path, output_path, variables):
    path, filename = os.path.split(template_path)
    result = jinja2.Environment(
        loader=jinja2.FileSystemLoader(path or './')
    ).get_template(filename).render(variables)

    # Write to the output file
    text_file = open(output_path, "w")
    text_file.write(result)
    text_file.close()

    return True


def compile_ino_project(project, variables=None):

    if variables is None:
        variables = {}

    variables["project"] = project

    # Create temp folder for this project
    temp_project_path = "tmp/%s" % (project["name"])
    if os.path.exists(temp_project_path):
        shutil.rmtree(temp_project_path, ignore_errors=True)
    os.makedirs(temp_project_path)
    os.makedirs("%s/src" % (temp_project_path))

    # Generate the config of the project
    _render_jinja_template("templates/conf/platformio.ini",
                           "%s/platformio.ini" % (temp_project_path),
                           variables)

    # Generate the source code of the project
    _render_jinja_template(project["file"],
                           "%s/src/main.ino" % (temp_project_path),
                           variables)

    # Compile the project
    FNULL = open(os.devnull, 'w')
    ret_code = subprocess.call("cd %s; pio run" % (temp_project_path), shell=True, stdout=FNULL, close_fds=True)

    firmware_hex_path = "%s/.pioenvs/uno/firmware.hex" % (temp_project_path)
    if ret_code == 0 and os.path.exists(firmware_hex_path):
        firmware_hex_absolute_path = os.path.abspath(firmware_hex_path)
        return {
            "project": project,
            "variables": variables,
            "name": project["name"],
            "temp_project_path": temp_project_path,
            "firmware_hex_path": firmware_hex_path,
            "firmware_hex_absolute_path": firmware_hex_absolute_path
        }
    else:
        return None


def generate_project(name):
    return True


def get_devices():
    devices = []
    proc = subprocess.Popen(["pio device list"], stdout=subprocess.PIPE, shell=True)
    (out, err) = proc.communicate()

    for line in out.split("\n"):
        if line.startswith("/dev/"):
            name = line.split("/")[-1]
            device = {
                "name": name,
                "location": line,
                "type": "usb" if "usb" in name else "unknown"
            }
            devices += [device]
        elif line.startswith("Hardware ID:"):
            hardware_id = line.split("Hardware ID:")[-1]
            devices[-1]["hardware_id"] = hardware_id
        elif line.startswith("Description:"):
            description = line.split("Description:")[-1]
            devices[-1]["description"] = description
        else:
            pass

    return devices


def get_usb_devices():
    return filter(lambda x: x["type"] == "usb", get_devices())


def manage_dependencies():
    return True


def upload_code(program, device):
    FNULL = open(os.devnull, 'w')
    cmd = "cd %s; pio run --target upload --upload-port '%s'" % (program["temp_project_path"], device["location"])
    return_code = subprocess.call([cmd], shell=True, stdout=FNULL, stderr=FNULL, close_fds=True)
    return return_code == 0


def wireless_upload_code(program, target_usb_device=None, programmer_usb_device=None, programmer_baudrate=9600):

    if target_usb_device is None or programmer_usb_device is None:
        return False

    FNULL = open(os.devnull, 'w')
    # cmd = "cd %s; pio run --target upload --upload-port '%s'" % (program["temp_project_path"], device["location"])
    hex_file__absolute_path = program["firmware_hex_absolute_path"]
    target_network_id = 2
    programmer_usb_device_path = programmer_usb_device["location"]
    programmer_baudrate = programmer_baudrate
    cmd = "python moteino/wireless/OTA.py -f %s -t %s -s %s -b %s" % (hex_file__absolute_path, target_network_id, programmer_usb_device_path, programmer_baudrate)
    return_code = subprocess.call([cmd], shell=True, stdout=FNULL, stderr=FNULL, close_fds=True)
    return return_code == 0


if __name__ == "__main__":
    print "Running server"

    # Clean temp folder
    if os.path.exists("tmp/"):
        shutil.rmtree("tmp/", ignore_errors=True)
    os.makedirs("tmp/")

    # Compile the "Programmer" ino file
    programmer_program = get_ino_project("Programmer")
    programmer_compilation_result = compile_ino_project(programmer_program,
                                                        variables={
                                                            "node_id": 1,
                                                            "blink_period": 250
                                                        })

    # Compile the "Target" ino file
    target_program = get_ino_project("Target")
    target_compilation_result = compile_ino_project(target_program,
                                                    variables={
                                                        "node_id": 2,
                                                        "blink_period": 250
                                                    })

    # Identify USB devices
    usb_devices = get_usb_devices()

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
    upload_code(programmer_compilation_result, programmer_usb_device)
    upload_code(target_compilation_result, target_usb_device)

    # Capture serial port of "Programmer" and "Target"
    # capture_thread_programmer = SerialCaptureThread("programmer", programmer_usb_device["location"], msg_handler, baudrate=115200)
    # capture_thread_programmer.start()
    #
    # capture_thread_target = SerialCaptureThread("target", target_usb_device["location"], msg_handler, baudrate=115200)
    # capture_thread_target.start()

    import time
    time.sleep(10)

    print("Will now try to wireless update the \"Target\" arduino (1)")


    #############################################################
    # Try to make an OTA update of the "Target" arduino
    #############################################################

    # Recompile the "Target" ino file
    target_recompilation_result = compile_ino_project(target_program,
                                                    variables={
                                                        "node_id": 2,
                                                        "blink_period": 2500
                                                    })
    wireless_upload_code(target_compilation_result,
                         target_usb_device=target_usb_device,
                         programmer_usb_device=programmer_usb_device,
                         programmer_baudrate=115200)
    time.sleep(10)

    print("Will now try to wireless update the \"Target\" arduino (2)")


    #############################################################
    # Try to make an OTA update of the "Target" arduino
    #############################################################

    # Recompile the "Target" ino file
    target_recompilation_result = compile_ino_project(target_program,
                                                    variables={
                                                        "node_id": 2,
                                                        "blink_period": 250
                                                    })
    wireless_upload_code(target_compilation_result,
                         target_usb_device=target_usb_device,
                         programmer_usb_device=programmer_usb_device,
                         programmer_baudrate=115200)
    # # Wait until something happens to the "Programmer" arduino
    # capture_thread_target.join(timeout=60.0)

    # # Compile files located in the inos folder
    # inos = get_ino_files()
    # for ino in inos:
    #     compile_inos(ino)

    sys.exit(0)
