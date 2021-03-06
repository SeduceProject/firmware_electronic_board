// RFM69HCW Example Sketch
// Send serial input characters from one RFM69 node to another
// Based on RFM69 library sample code by Felix Rusu
// http://LowPowerLab.com/contact
// Modified for RFM69HCW by Mike Grusin, 4/16

// This sketch will show you the basics of using an
// RFM69HCW radio module. SparkFun's part numbers are:
// 915MHz: https://www.sparkfun.com/products/12775
// 434MHz: https://www.sparkfun.com/products/12823

// See the hook-up guide for wiring instructions:
// https://learn.sparkfun.com/tutorials/rfm69hcw-hookup-guide

// Uses the RFM69 library by Felix Rusu, LowPowerLab.com
// Original library: https://www.github.com/lowpowerlab/rfm69
// SparkFun repository: https://github.com/sparkfun/RFM69HCW_Breakout

// Include the RFM69 and SPI libraries:

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

///////////////////////////////////////////////////////
// Stuff related to WIFI
///////////////////////////////////////////////////////

// variables related to WIFI
char ssid[] = "b232_iot_network";     //  your network SSID (name)
char password[] = "iotnetwork";  // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status


///////////////////////////////////////////////////////
// Stuff related to ds18b20
///////////////////////////////////////////////////////
#define MINIMUM_PAUSE_BETWEEN_LOOP_CALLS 30000

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS D5
#define DEBUG

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
DeviceAddress* addresses[20];
int current_device_count = 0;

// function to convert a device address to a String
String deviceAddressToString(DeviceAddress deviceAddress)
{
  String result = "";
  for (uint8_t i = 0; i < 8; i++)
  {
    /* if (i > 0) { */
    /*   result += ":"; */
    /* } */
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) result += "0";
    result += String(deviceAddress[i], HEX);
  }
  return result;
}

void setup() {
  // Open a serial port so we can send keystrokes to the module:

  Serial.begin(9600);
  Serial.print("Node ");
  Serial.print(1234,DEC);
  Serial.println(" ready");

  // Initialize the WIFI:
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
	delay(500);
	Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Allocate 20 addresses
  for (int deviceIndex = 0; deviceIndex < 20; deviceIndex++) {
    addresses[deviceIndex] = (DeviceAddress*) malloc(sizeof(DeviceAddress));
  }
}

DeviceAddress tempDeviceAddress;
void rescan_devices() {
  /*  for (int deviceIndex = 0; deviceIndex < current_device_count; deviceIndex++) { */
  /*     DeviceAddress *address = addresses[deviceIndex]; */
  /*     free(address); */
  /* } */
  current_device_count = sensors.getDeviceCount();
  for (int deviceIndex = 0; deviceIndex < current_device_count; deviceIndex++) {
      sensors.getAddress(tempDeviceAddress, deviceIndex);

      //addresses[deviceIndex] = (DeviceAddress*) malloc(sizeof(DeviceAddress));
      sensors.setResolution(tempDeviceAddress, 11);
      memcpy(addresses[deviceIndex], &tempDeviceAddress, sizeof(DeviceAddress));
  }
}

int cpt = 0;

void loop() {

  Serial.println("Waking up");
  
  if (cpt == 0 || current_device_count == 0) {
    Serial.println("Sensors.begin");
    // Start up the library
    sensors.begin(); // IC Default 9 bit. If you have troubles consider upping it 12. Ups the delay giving the IC more time to process the temperature measurement
    Serial.println("  - Sensors.begin: done");
  }

  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  
  Serial.println("Requesting temperatures");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println(" - done");
  
  Serial.println("Current_device_count: "+String(current_device_count));

  if (sensors.getDeviceCount() != current_device_count) {
    Serial.println("scanning devices");
    rescan_devices();
    Serial.println("  - scanning devices: done");
  }

  Serial.println("building message from temperatures");
  String msg = "[";
  for (int deviceIndex = 0; deviceIndex< current_device_count; deviceIndex++) {
      DeviceAddress *deviceAddress = addresses[deviceIndex];
      float temperature = sensors.getTempC(*deviceAddress);
      String macAddressAsString = deviceAddressToString(*deviceAddress);

      Serial.println("["+String(deviceIndex)+"] "+macAddressAsString+" --> "+String(temperature));
      /* Serial.print(sensors.getResolution(*deviceAddress), DEC); */

      // Perform an HTTP get request
      if (deviceIndex > 0) {
        msg += ",";
      }
      msg += "{\"sensor\":\""+macAddressAsString+"\",\"t\":\"T\",\"v\":"+String(temperature)+"}";
  }
  msg += "]";
  Serial.println("  - building message from temperatures: done");

  Serial.println("Doing an HTTP request to the webservice");
  HTTPClient http;
  Serial.println("   - http.begin");
  http.begin("http://{{project.variables.webservice_host}}:{{project.variables.webservice_port}}/temperature/list");
  Serial.println("   - http.addHeader");
  http.addHeader("Content-Type", "application/json");
  Serial.println("   - http.Post");
  int httpCode = http.POST(msg);
  /* Serial.println("   - http.getString"); */
  /* String payload = http.getString(); */
  /* Serial.println("     -> response: '"+payload+"'"); */
  Serial.println("   - http.end");
  http.end();

  Serial.println("end of loop");

  cpt = (cpt + 1) % (3 * current_device_count + 1);

  /* Serial.println("Sleeping 5 seconds"); */
  /* delay(5000); */
}

void Blink(byte PIN, int DELAY_MS)
// Blink an LED for a given number of ms
{
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}
