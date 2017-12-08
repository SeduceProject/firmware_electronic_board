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

#include <RFM69.h>
#include <SPI.h>
#include <OneWire.h>
#include <LowPower.h>
#include <DallasTemperature.h>

///////////////////////////////////////////////////////
// Stuff related to Radio communication
///////////////////////////////////////////////////////

// Addresses for this node. CHANGE THESE FOR EACH NODE!
#define NETWORKID     0   // Must be the same for all nodes
#define MYNODEID      {{ node_id }}   // My node ID
#define TONODEID      1   // Destination node ID

// RFM69 frequency, uncomment the frequency of your module:
#define FREQUENCY   RF69_433MHZ
//#define FREQUENCY     RF69_915MHZ

// AES encryption (or not):

#define ENCRYPT       true // Set to "true" to use encryption
#define ENCRYPTKEY    "TOPSECRETPASSWRD" // Use the same 16-byte key on all nodes

// Use ACKnowledge when sending messages (or not):

#define USEACK        true // Request ACKs or not

// Packet sent/received indicator LED (optional):

#define LED           9 // LED positive pin
#define GND           8 // LED ground pin

#define MAX_SEND     250 // maximum packet size

#define USE_EXPONENTIAL_BACKOFF true
#define BURST true

// Create a library object for our RFM69HCW module:
RFM69 radio;

///////////////////////////////////////////////////////
// Stuff related to ds18b20
///////////////////////////////////////////////////////
#define MINIMUM_PAUSE_BETWEEN_LOOP_CALLS 30000

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 5
#define DEBUG

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// function to convert a device address to a String
String deviceAddressToString(DeviceAddress deviceAddress)
{
  String result = "";
  for (uint8_t i = 0; i < 8; i++)
  {
    if (i > 0) {
      result += ":";
    }
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
  Serial.print(MYNODEID,DEC);
  Serial.println(" ready");

  // Set up the indicator LED (optional):
  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);
  pinMode(GND,OUTPUT);
  digitalWrite(GND,LOW);

  // Initialize the RFM69HCW:

  radio.initialize(FREQUENCY, MYNODEID, NETWORKID);
  //radio.setHighPower(); // Always use this for RFM69HCW

  // Turn on encryption if desired:
  if (ENCRYPT)
    radio.encrypt(ENCRYPTKEY);
}

boolean deep_sleep(int n_msecs) {
  if (n_msecs > 8000) {
    // deep sleep SLEEP_8S
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    return deep_sleep(n_msecs - 8000);
  } else if (n_msecs > 4000) {
    // deep sleep SLEEP_4S
    LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
    return deep_sleep(n_msecs - 4000);
  } else if (n_msecs > 2000) {
    // deep sleep SLEEP_2S
    LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
    return deep_sleep(n_msecs - 2000);
  } else if (n_msecs > 1000) {
    // deep sleep SLEEP_1S
    LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
    return deep_sleep(n_msecs - 1000);
  } else if (n_msecs > 500) {
    // deep sleep SLEEP_500MS
    LowPower.powerDown(SLEEP_500MS, ADC_OFF, BOD_OFF);
    return deep_sleep(n_msecs - 500);
  }  else if (n_msecs > 250) {
    // deep sleep SLEEP_250MS
    LowPower.powerDown(SLEEP_250MS, ADC_OFF, BOD_OFF);
    return deep_sleep(n_msecs - 250);
  } else if (n_msecs > 120) {
    // deep sleep SLEEP_120MS
    LowPower.powerDown(SLEEP_120MS, ADC_OFF, BOD_OFF);
    return deep_sleep(n_msecs - 120);
  } else if (n_msecs > 60) {
    // deep sleep SLEEP_60MS
    LowPower.powerDown(SLEEP_60MS, ADC_OFF, BOD_OFF);
    return deep_sleep(n_msecs - 60);
  } else if (n_msecs > 30) {
    // deep sleep SLEEP_30MS
    LowPower.powerDown(SLEEP_30MS, ADC_OFF, BOD_OFF);
    return deep_sleep(n_msecs - 30);
  } else if (n_msecs > 15) {
    // deep sleep SLEEP_15MS
    LowPower.powerDown(SLEEP_15MS, ADC_OFF, BOD_OFF);
    return deep_sleep(n_msecs - 15);
  }

  delay(n_msecs);
  return true;
}


//long int TIME_BETWEEN_TEMP_READING_MS = 30 * 1000;

void loop() {
   // Start up the library
  sensors.begin(); // IC Default 9 bit. If you have troubles consider upping it 12. Ups the delay giving the IC more time to process the temperature measurement

  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  sensors.requestTemperatures(); // Send the command to get temperatures


  for (int deviceIndex = 0; deviceIndex< sensors.getDeviceCount(); deviceIndex++) {
      DeviceAddress deviceAddress;
      sensors.getAddress(deviceAddress, deviceIndex);

      float temperature = sensors.getTempCByIndex(deviceIndex);
      String macAddressAsString = deviceAddressToString(deviceAddress);


      if (temperature != -127.00) {
        static char sendbuffer[250];
        static int sendlength = 0;

        Serial.println(" ");
        Serial.println("+----------------------------------");
        Serial.println("| Read temperature sensor");
        Serial.println("+----------------------------------");

        delay(50); // This pause prevent the Arduino to crashes when using a very long cable ?!?

        Serial.println("| temperature_to_send: "+String(temperature));
        Serial.println("| sensor: "+String(deviceIndex));
        Serial.println("| mac_address: "+macAddressAsString);
        Serial.println(" ");

        String msg = "{\"sensor\":\""+macAddressAsString+"\",\"t\":\"T\",\"v\":"+String(temperature)+"}";

        sprintf(sendbuffer,"%s", msg.c_str());
        sendlength = msg.length();

        if (USEACK) {
          boolean ack_received = false;
          boolean requestACK = true;

          #if USE_EXPONENTIAL_BACKOFF

          Serial.println("+----------------------------------");
          Serial.println("| Sending temperature");
          Serial.println("+----------------------------------");

          unsigned long slot_time = 20 + MYNODEID * 5; // default: 50ms
          unsigned long slot_numbers = 2;
          int retry_max = 10;
          int retry_count = 0;
          while (retry_count < retry_max && ! (ack_received)) {

            Serial.println("| Try n="+String(retry_count));

            unsigned long time_to_wait = slot_time * (1 + random(slot_numbers - 1));

            if (radio.sendWithRetry(TONODEID, sendbuffer, sendlength, 2, time_to_wait)) {
              ack_received = true;
            }

            if(!ack_received) {
              Serial.println("| Still no ACK received, I will wait: "+String(time_to_wait));
              delay(time_to_wait);
            }

            retry_count += 1;
            //slot_numbers = slot_numbers * 2;
            slot_numbers = slot_numbers + 1;
          }
          #else
          if (radio.sendWithRetry(TONODEID, sendbuffer, sendlength)) {
              Serial.println("| Success: ACK received!");
              ack_received = true;
          } else {
              Serial.println("| Failure: no ACK received");
              ack_received = false;
          }
          Serial.println(" ");
          #endif

          if (ack_received) {
            Serial.println("| ACK received!");
          }
        } else {
          radio.send(TONODEID, sendbuffer, sendlength);
        }

        Blink(LED,10);
      }
  }

  delay(100);
  deep_sleep(MINIMUM_PAUSE_BETWEEN_LOOP_CALLS);
  //delay(MINIMUM_PAUSE_BETWEEN_LOOP_CALLS);
}

void Blink(byte PIN, int DELAY_MS)
// Blink an LED for a given number of ms
{
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}