# Arduinos firmware for the SeDuCe platform

## Objective

This repository contains a set of scripts that are used to generate firmwares of electronic sensors
featured by the SeDuCe platform.

## Installation

First, ensure that your development environment is configured with Python3 and its package manage pip.
If this is not the case, you may take a look at the following link [https://www.tecmint.com/install-pip-in-linux/](https://www.tecmint.com/install-pip-in-linux/).

Install the dependencies of the script with the following command:
```shell script
pip3 install -r requirements.txt
```

you can check that the installation did work by running the following command:

```shell script
python3 detect_mapping_device_cards.py --help
```

## Detect the mapping "usb device / electronic card"

```shell script
python3 detect_mapping_device_cards.py 
```

## List available firmwares

```shell script
python3 upload_firmware.py --list-firmwares
```

## List available serial ports

```shell script
python3 upload_firmware.py --list-serial
```

## Generate firmware and upload them on electronic boards

```shell script
python3 upload_firmware.py --firmware SensorTC --serial /dev/ttyUSB1 --variables "{'card_name': 'card1'}"
```

## Compute corrections for electronic cards after calibrations of thermocouples

First create a tunnel between your computer and seduce1

```shell script
ssh -N -L 8086:192.168.1.25:8086 seduce1
```

Then provide information on the calibration in `conf/calibration.yaml` and finally run the following script:

```shell script
python3 compute_corrections.py
```


## Contributing

Don't hesitate to fork this project and to create a pull request.