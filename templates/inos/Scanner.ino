#include <OneWire.h>

///////////////////////////////////////////////////////
// Stuff related to ds18b20
///////////////////////////////////////////////////////

// The pin for the data wire of the onewire bus
#define ONEWIRE_PIN 5
// Enable debug messages
#define DEBUG
#define NB_SENSORS 9

/***
 * OneWire Configuration
 ***/

// OneWire instance
OneWire ds(ONEWIRE_PIN);

// Convert sensor addresses to string
String addr_str(byte addr[]) {
  String sensor_id = String(addr[0], HEX);
  for(int i = 1; i < 8; i++) {
    sensor_id += ":" + String(addr[i], HEX);
  }
  return sensor_id;
}

void setup() {
  // Open a serial port so we can send keystrokes to the module:
  Serial.begin(9600);
  Serial.println("I will start scanning the onwire bus (PIN "+String(ONEWIRE_PIN)+")");
}

void loop() {

  float temperature[NB_SENSORS];

  ds.reset_search();
  boolean continue_search = true;
  int idx = 0;
  while(continue_search) {
    byte data[9], addr[8];
    /* Recherche le prochain capteur 1-Wire disponible */
    if (!ds.search(addr)) {
      // Pas de capteur
      //continue_search = false;
      //return NO_SENSOR_FOUND;
      idx = 0;
      Serial.println(" ");
      return;
    }

    /* Reset le bus 1-Wire et selectionne le capteur */
    ds.reset();
    ds.select(addr);

    /* Lance une prise de mesure de temperature et attend la fin de la mesure */
    ds.write(0x44, 1);
    delay(800);

    /* Reset le bus 1-Wire, selectionne le capteur et envoie une demande de lecture du scratchpad */
    ds.reset();
    ds.select(addr);
    ds.write(0xBE);

    /* Lecture du scratchpad */
    for (byte i = 0; i < 9; i++) {
      data[i] = ds.read();
    }

    // Recuperation de la position du capteur sur le fil
    //int idx = getSensorIdx(addr);
    // Calcul de la temperature en degre Celsius
    if(idx == -1 || idx > NB_SENSORS - 1) {
    } else {
      temperature[idx] = ((data[1] << 8) | data[0]) * 0.0625;
    }
    char macStr[24];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
       addr[0], addr[1], addr[2], addr[3], addr[4], addr[5],addr[6], addr[7]);
    Serial.println("idx: "+String(idx)+" ---> "+macStr+" ("+String(temperature[idx])+")");
    idx++;
  }

  delay(1000);
}
