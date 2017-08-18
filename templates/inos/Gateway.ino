/*
 * UIPEthernet TcpClient example.
 *
 * UIPEthernet is a TCP/IP stack that can be used with a enc28j60 based
 * Ethernet-shield.
 *
 * UIPEthernet uses the fine uIP stack by Adam Dunkels <adam@sics.se>
 *
 *      -----------------
 *
 * This TcpClient example gets its local ip-address via dhcp and sets
 * up a tcp socket-connection to 192.168.0.1 port 5000 every 5 Seconds.
 * After sending a message it waits for a response. After receiving the
 * response the client disconnects and tries to reconnect after 5 seconds.
 *
 * Copyright (C) 2013 by Norbert Truchsess <norbert.truchsess@t-online.de>
 */
#include <Arduino.h>

#include <RFM69.h>
#include <SPI.h>

#include <Ethernet.h>
#include <utility/w5100.h>

EthernetClient client;
unsigned long send_delay=1000;
unsigned long next = 0 ;
size_t buff_size=500;

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


  Serial.println("Now configuring the ethernet conenction");

  Ethernet.select(4);
  byte mac[6] = { 0x00, 0x50, 0x56, 0x13, 0x37, 0x20 };
  Ethernet.begin(mac);
  Serial.print(F("My IP address: "));
  for (size_t idx = 0; idx < 4; idx++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[idx], DEC);
    if (idx != 3) Serial.print(F("."));
  }
  Serial.println();
  W5100.setRetransmissionTime(2000); //where each unit is 100us, so 0x07D0 (decimal 2000) means 200ms.
  W5100.setRetransmissionCount(3); //That gives me a 3 second timeout on a bad server connection.
  next = millis() + send_delay;
}


void loop()
{
  //Serial.println("LOOPING");
  // Set up a "buffer" for characters that we'll send:

  static char sendbuffer[1000];
  static int sendlength = 0;

  /*
  // SENDING

  // In this section, we'll gather serial characters and
  // send them to the other node if we (1) get a carriage return,
  // or (2) the buffer is full (61 characters).

  // If there is any serial input, add it to the buffer:

  if (Serial.available() > 0)
  {
    char input = Serial.read();

    if (input != '\r') // not a carriage return
    {
      sendbuffer[sendlength] = input;
      sendlength++;
    }

    // If the input is a carriage return, or the buffer is full:

    if ((input == '\r') || (sendlength == 999)) // CR or buffer full
    {
      // Send the packet!


      Serial.print("sending to node ");
      Serial.print(TONODEID, DEC);
      Serial.print(", message [");
      for (byte i = 0; i < sendlength; i++)
        Serial.print(sendbuffer[i]);
      Serial.println("]");

      // There are two ways to send packets. If you want
      // acknowledgements, use sendWithRetry():

      if (USEACK)
      {
        if (radio.sendWithRetry(TONODEID, sendbuffer, sendlength))
          Serial.println("ACK received!");
        else
          Serial.println("no ACK received");
      }

      // If you don't need acknowledgements, just use send():

      else // don't use ACK
      {
        radio.send(TONODEID, sendbuffer, sendlength);
      }

      sendlength = 0; // reset the packet
      Blink(LED,10);
    }
  }
  */

  // RECEIVING

  // In this section, we'll check with the RFM69HCW to see
  // if it has received any packets:

  if (radio.receiveDone()) // Got one!
  {
    // Print out the information:

    Serial.print("received from node ");
    Serial.print(radio.SENDERID, DEC);
    Serial.print(", message [");

    // The actual message is contained in the DATA array,
    // and is DATALEN bytes in size:

    for (byte i = 0; i < radio.DATALEN; i++)
      Serial.print((char)radio.DATA[i]);

    // RSSI is the "Receive Signal Strength Indicator",
    // smaller numbers mean higher power.

    Serial.print("], RSSI ");
    Serial.println(radio.RSSI);

    // Send an ACK if requested.
    // (You don't need this code if you're not using ACKs.)

    if (radio.ACKRequested())
    {
      radio.sendACK();
      Serial.println("ACK sent");
    }
    Blink(LED,10);
  }
}

void Blink(byte PIN, int DELAY_MS)
// Blink an LED for a given number of ms
{
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}

/*
void loop() {
  unsigned long time = millis();
  if (next<time) {
    Ethernet.maintain();
    next += send_delay;
    if(next <= time) next = time+1;
    if (client.connected()){
      Serial.println("already connected");
    } else {
      Serial.print("connecting results : ");
      Serial.println(client.connect("tarzan.info.emn.fr",80));
    }
    int sent = client.println("GET /ping.php HTTP/1.1");
    sent = client.println(F("Host: tarzan.info.emn.fr"));
    client.println();
  }
  if(client.connected() && client.available()>0){
    Serial.println("client received data  : ");
    unsigned char buffer[buff_size+1];
    while(client.available()>0){
      int  read = client.read(buffer, buff_size);
      buffer[read]='\0';
      Serial.println(read);
    }
  }
}

*/
