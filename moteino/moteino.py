import os
import shutil
import jinja2
import config
import subprocess

PLATFORM = "atmelavr"
BOARD = "moteino"

capture_threads = []


def clean_tmp_folder():
    # Clean temp folder
    if os.path.exists("tmp/"):
        shutil.rmtree("tmp/", ignore_errors=True)
    os.makedirs("tmp/")


def ensure_tmp_folder_exists():
    # Clean temp folder
    if not os.path.exists("tmp/"):
        os.makedirs("tmp/")


def get_ino_projects():
    return config.config["ino_projects"]


def get_ino_project(program_name):
    candidates = filter(lambda x: x["name"] == program_name, config.config["ino_projects"])
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
    ensure_tmp_folder_exists()
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
    firmware_bin_path = "%s/.pioenvs/esp12e/firmware.bin" % (temp_project_path)
    if ret_code == 0 and os.path.exists(firmware_hex_path):
        firmware_hex_absolute_path = os.path.abspath(firmware_hex_path)
        return {
            "project": project,
            "variables": variables,
            "name": project["name"],
            "temp_project_path": temp_project_path,
            "firmware_path": firmware_hex_path,
            "firmware_absolute_path": firmware_hex_absolute_path
        }
    elif ret_code == 0 and os.path.exists(firmware_bin_path):
        firmware_bin_absolute_path = os.path.abspath(firmware_bin_path)
        return {
            "project": project,
            "variables": variables,
            "name": project["name"],
            "temp_project_path": temp_project_path,
            "firmware_path": firmware_bin_path,
            "firmware_absolute_path": firmware_bin_absolute_path
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
                "type": "usb" if "usb" in name.lower() else "unknown"
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
