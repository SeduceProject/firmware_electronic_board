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

// Addresses for this node. CHANGE THESE FOR EACH NODE!

#define NETWORKID     0   // Must be the same for all nodes
#define MYNODEID      {{node_id}}   // My node ID
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

int cpt = 0;

long int TIME_BETWEEN_TEMP_READING_MS = 50;
unsigned long last_temp_reading = -1;

void loop() {

  bool send_temp_reading = false;
  unsigned long time_since_last_reading = millis() - last_temp_reading;
  if ((last_temp_reading == -1) || (time_since_last_reading > TIME_BETWEEN_TEMP_READING_MS)) {
    last_temp_reading = millis();
    send_temp_reading = true;
  } else {
    long time_to_sleep = TIME_BETWEEN_TEMP_READING_MS - time_since_last_reading;
    //Serial.println("I will sleep:");
    //Serial.println(time_to_sleep);
    delay(time_to_sleep);
  }

  if (send_temp_reading)
  {
    static char sendbuffer[250];
    static int sendlength = 0;

    Serial.println(" ");
    Serial.println("+----------------------------------");
    Serial.println("| Read temperature sensor");
    Serial.println("+----------------------------------");

    float temperature_to_send = -1.0;

    #if BURST
    temperature_to_send = 25.0 + random(8.0);
    Serial.println("| mode: BURST");
    Serial.println("| temperature_to_send: "+String(temperature_to_send));
    Serial.println(" ");
    #else
    float temperature = 0.0;
    int reading_count = 0;

    int min = 9999;
    int max = -9999;
    for (int i=0; i<10; i++) {
        uint8_t temperature_reading = radio.readTemperature();
        if (temperature_reading < min) {
            min = temperature_reading;
        }
        if (temperature_reading > max) {
            max = temperature_reading;
        }
        temperature += temperature_reading;
        reading_count += 1;
        delay(50); // Ensure we don't get 10 times the same temperature
    }

    // The following block enables to filter aberrant temperature readings
    if ((max - min) > 2) {
      int time_until_next_try = 100 + random(1600);
      Serial.println("| Failure: inaccurate temperature detected, I will sleep and try again in "+String(time_until_next_try));
      delay(time_until_next_try);
      return;
    }

    temperature_to_send = temperature / reading_count;

    Serial.println("| temperature_to_send: "+String(temperature_to_send));
    Serial.println("| temperature: "+String(temperature));
    Serial.println("| reading_count: "+String(reading_count));
    Serial.println("| min: "+String(min));
    Serial.println("| max: "+String(max));
    Serial.println(" ");
    #endif

    String msg = "{\"sensor\": {{ node_id }}, \"key\": \"temp\", \"value\": "+String(temperature_to_send)+"}";

    sprintf(sendbuffer,"%s", msg.c_str());
    sendlength = msg.length();

    if (USEACK) {
      boolean ack_received = false;
      boolean requestACK = true;

      #if USE_EXPONENTIAL_BACKOFF

      Serial.println("+----------------------------------");
      Serial.println("| Sending temperature");
      Serial.println("+----------------------------------");

      unsigned long slot_time = 50 + {{node_id}} * 10; // default: 50ms
      unsigned long slot_numbers = 2;
      int retry_max = 10;
      int retry_count = 0;
      while (retry_count < retry_max && ! (ack_received)) {
        Serial.println("| Try n="+String(retry_count));

        unsigned long time_to_wait = slot_time * (1 + random(slot_numbers - 1));

        if (radio.sendWithRetry(TONODEID, sendbuffer, sendlength, 2, time_to_wait))
        {
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

  //delay(200);
}

void Blink(byte PIN, int DELAY_MS)
// Blink an LED for a given number of ms
{
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}