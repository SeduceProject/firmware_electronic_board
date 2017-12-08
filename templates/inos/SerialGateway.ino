#include <Arduino.h>

#include <RFM69.h>
#include <SPI.h>

#include <utility/w5100.h>
#include <Bridge.h>

#include <MemoryFree.h>
#define DEBUG_MEM false

//unsigned long send_delay=1000;
//unsigned long next = 0 ;
//size_t buff_size=500;

// Addresses for this node. CHANGE THESE FOR EACH NODE!

#define NETWORKID     0   // Must be the same for all nodes
#define MYNODEID      {{node_id}}   // My node ID
#define TONODEID      2   // Destination node ID

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

// Create a library object for our RFM69HCW module:

RFM69 radio;

void setup() {

  Serial.begin(9600);
  Serial.println("Serial initialized");

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


  Serial.println();
  //W5100.setRetransmissionTime(2000); //where each unit is 100us, so 0x07D0 (decimal 2000) means 200ms.
  //W5100.setRetransmissionCount(3); //That gives me a 3 second timeout on a bad server connection.
  //next = millis() + send_delay;
}

void loop()
{
  Serial.println("<begin loop>");
  // Set up a "buffer" for characters that we'll send:

  static char sendbuffer[500];
  static int sendlength = 0;

  // RECEIVING

  // In this section, we'll check with the RFM69HCW to see
  // if it has received any packets:

  if (radio.receiveDone()) // Got one!
  {

    // Print out the information:
    Serial.print("received from node ");
    Serial.print(radio.SENDERID, DEC);
    Serial.print(", message ");

    char* msg_buffer = (char*) malloc(radio.DATALEN + 1);
    for (int i =0; i < radio.DATALEN; i++) {
      msg_buffer[i] = (char) radio.DATA[i];
    }
    msg_buffer[radio.DATALEN] = 0;
    Serial.println(msg_buffer);

    String msg_string = String(msg_buffer);
    Serial.println(msg_string);

    // Send an ACK if requested.
    if (radio.ACKRequested())
    {
      radio.sendACK();
      Serial.println("ACK sent");
    }

    Serial.println("<before freeing>");
    free(msg_buffer);
    Serial.println("<after freeing>");

    Blink(LED,10);
    //delay(50);
  }
  Serial.println("<end loop>");
}

void Blink(byte PIN, int DELAY_MS)
// Blink an LED for a given number of ms
{
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}
