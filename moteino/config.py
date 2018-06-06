config = {
    "ino_projects": [
        {
            "name": "Programmer",
            "file": "templates/inos/Programmer.ino"
        },
        {
            "name": "Target",
            "file": "templates/inos/Target.ino"
        },
        {
            "name": "Gateway",
            "file": "templates/inos/Gateway.ino",
            "variables": {
                "webservice_host": "192.168.1.4",
                "webservice_port": 8080
            }
        },
        {
            "name": "SerialGateway",
            "file": "templates/inos/SerialGateway.ino",
            "variables": {
                "webservice_host": "192.168.1.4",
                "webservice_port": 8080
            }
        },
        {
            "name": "Sensor",
            "file": "templates/inos/Sensor.ino"
        },
        {
            "name": "SensorESP12E",
            "file": "templates/inos/SensorESP12E.ino",
            "variables": {
                "webservice_host": "192.168.1.30",
                "webservice_port": 8080
            }
        }
    ]
}
