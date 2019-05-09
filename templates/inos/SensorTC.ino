
#include <OneWire.h>
#include <DallasTemperature.h>
#include <math.h>


#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>


///////////////////////////////////////////////////////
// Stuff related to WIFI
///////////////////////////////////////////////////////

// variables related to WIFI
char ssid[] = "b232_iot_network";     //  your network SSID (name)
char password[] = "iotnetwork";  // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

///////////////////////////////////////////////////////
// Stuff related to MAX31850K
///////////////////////////////////////////////////////

#define TEMPERATURE_PRECISION 11

#define TYPE_DS18S20 0
#define TYPE_DS18B20 1
#define TYPE_DS18S22 2
#define TYPE_MAX31850 3
OneWire  ds(D5);  // on pin 10 (a 4.7K pullup resistor is necessary)
DallasTemperature sensors(&ds);

{% if card_id == 1 %}

// Correction for card 'card1'
#define NB_SENSORS 16
#define NB_TEMPERATURE_ENTRIES 5
float calibrationMap[NB_SENSORS][NB_TEMPERATURE_ENTRIES] = {
  {  0.19,  0.32,  0.91,  0.74,  0.79},
  {  0.31,  0.47,  0.94,  0.79,  0.82},
  {  0.17,  0.39,  0.79,  0.81,  0.83},
  {  1.16,  1.17,  1.69,  1.66,  1.87},
  {  2.42,  2.56,  3.40,  3.12,  3.11},
  {  2.90,  3.27,  3.65,  3.81,  3.62},
  { -1.12, -1.05, -0.93, -1.01, -0.99},
  { -1.35, -1.28, -0.52, -1.16, -1.16},
  { -0.05, -0.10,  0.53,  0.42,  0.59},
  { -0.34,  0.14,  0.79,  0.91,  1.30},
  {  0.49,  0.79,  1.40,  1.86,  2.10},
  {  0.95,  1.14,  2.22,  1.91,  1.91},
  {  3.01,  2.72,  4.02,  3.68,  3.99},
  {  3.78,  3.94,  4.88,  4.86,  5.24},
  { -1.43, -1.07, -0.84, -0.42, -0.16},
  { -1.62, -1.35, -0.82, -0.85, -0.44},
};

{% elif card_id == 2 %}

// Correction for card 'card2'
#define NB_SENSORS 16
#define NB_TEMPERATURE_ENTRIES 7
float calibrationMap[NB_SENSORS][NB_TEMPERATURE_ENTRIES] = {
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
};

{% elif card_id == 3 %}

// Correction for card 'card3'
#define NB_SENSORS 16
#define NB_TEMPERATURE_ENTRIES 7
float calibrationMap[NB_SENSORS][NB_TEMPERATURE_ENTRIES] = {
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
};

{% elif card_id == 4 %}

// Correction for card 'card4'
#define NB_SENSORS 16
#define NB_TEMPERATURE_ENTRIES 7
float calibrationMap[NB_SENSORS][NB_TEMPERATURE_ENTRIES] = {
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
};

{% elif card_id == 5 %}

// Correction for card 'card5'
#define NB_SENSORS 16
#define NB_TEMPERATURE_ENTRIES 7
float calibrationMap[NB_SENSORS][NB_TEMPERATURE_ENTRIES] = {
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
};

{% elif card_id == 6 %}

// Correction for card 'card6'
#define NB_SENSORS 16
#define NB_TEMPERATURE_ENTRIES 7
float calibrationMap[NB_SENSORS][NB_TEMPERATURE_ENTRIES] = {
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
};

{% elif card_id == 7 %}

// Correction for card 'card7'
#define NB_SENSORS 16
#define NB_TEMPERATURE_ENTRIES 7
float calibrationMap[NB_SENSORS][NB_TEMPERATURE_ENTRIES] = {
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
};

{% elif card_id == 8 %}

// Correction for card 'card8'
#define NB_SENSORS 16
#define NB_TEMPERATURE_ENTRIES 7
float calibrationMap[NB_SENSORS][NB_TEMPERATURE_ENTRIES] = {
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
};

{% elif card_id == 9 %}

// Correction for card 'card9'
#define NB_SENSORS 16
#define NB_TEMPERATURE_ENTRIES 7
float calibrationMap[NB_SENSORS][NB_TEMPERATURE_ENTRIES] = {
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
};

{% elif card_id == 100 %}

// Correction for card 'card6'
#define NB_SENSORS 16
#define NB_TEMPERATURE_ENTRIES 7
float calibrationMap[NB_SENSORS][NB_TEMPERATURE_ENTRIES] = {
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
};

{% else %}

#define NB_SENSORS 16
#define NB_TEMPERATURE_ENTRIES 7
float calibrationMap[NB_SENSORS][NB_TEMPERATURE_ENTRIES] = {
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0},
};

{% endif %}

// function to convert a device address to a String
String deviceAddressToString(DeviceAddress deviceAddress)
{
  String result = "";
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) result += "0";
    result += String(deviceAddress[i], HEX);
  }
  return result;
}

DeviceAddress* addresses[20];

void scan_address_indexes() {
  byte i;
  byte present = 0;
  byte temptype;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;

  bool do_scan = true;

  Serial.println(F("******************************************"));
  Serial.println(F("SCANNING"));
  while(do_scan) {
      
    if ( !ds.search(addr)) {
      ds.reset_search();
      delay(250);
      do_scan = false;
      
    } else {
      Serial.print("ROM =");
    
      if (OneWire::crc8(addr, 7) != addr[7]) {
          return;
      }
         
      ds.reset();
      ds.select(addr);
      ds.write(0x44, 1);        // start conversion, with parasite power on at the end
      
      delay(100);     // maybe 750ms is enough, maybe not
      // we might do a ds.depower() here, but the reset will take care of it.
      
      present = ds.reset();
      ds.select(addr);    
      ds.write(0xBE);         // Read Scratchpad
    
      for ( i = 0; i < 9; i++) {           // we need 9 bytes
        data[i] = ds.read();
      }

      int index = data[4] & 0xF;
      
      Serial.print("  Address = ");
      Serial.println(index);

      
      DeviceAddress devAddr;
      for( i = 0; i < 8; i++) {
        devAddr[i] = addr[i];
      }
      
      addresses[index] = (DeviceAddress*) malloc(sizeof(DeviceAddress));
      sensors.setResolution(devAddr, TEMPERATURE_PRECISION);
      memcpy(addresses[index], &devAddr, sizeof(DeviceAddress));
    }  
  }
  Serial.println(F("ENDOF SCANNING"));
}

float getCalibratedTemperature(int deviceIndex, float uncalibratedTemperature) {
  int calib_t_min = 10;
  int calib_t_max = 50;

  if (uncalibratedTemperature <= calib_t_min) {
    return uncalibratedTemperature + calibrationMap[deviceIndex][0];
  }

  if (uncalibratedTemperature >= calib_t_max) {
    return uncalibratedTemperature + calibrationMap[deviceIndex][NB_TEMPERATURE_ENTRIES - 1];
  }

  int calib_index_inferior_temp = (int) (uncalibratedTemperature - calib_t_min) / 10.0;
  int calib_index_superior_temp = calib_index_inferior_temp + 1;

  float calib_inferior_temp = (calib_index_inferior_temp - 1) * 10.0;
  float calib_superior_temp = (calib_index_superior_temp - 1) * 10.0;

  float calib_inferior_correction = calibrationMap[deviceIndex][calib_index_inferior_temp];
  float calib_superior_correction = calibrationMap[deviceIndex][calib_index_superior_temp];

  float correction = calib_inferior_correction + ((calib_superior_correction - calib_inferior_correction) / (calib_superior_temp - calib_inferior_temp)) * (uncalibratedTemperature - calib_inferior_temp);
  
  return uncalibratedTemperature + correction;
}


float distance(float a, float b) {
  float difference = a-b;
  return sqrt(difference * difference);
}

void setup(void) {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  
  scan_address_indexes();
  sensors.begin();
  
  // report parasite power requirements
  Serial.print("Parasite power is: "); 
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");
  
  // Initialize the WIFI:
  WiFi.begin(ssid, password);
}

int cpt = 0;
float temperatures_array[16][4];
int temperatures_array_initialization_step = 0;

void loop(void) {
  delay(200);
  Serial.println("Start of cycle");
  if (cpt == 0) {
    for (int i=0; i< 16; i++) {
        DeviceAddress* devAddrPtr = addresses[i];
        String macAddressAsString = deviceAddressToString(*devAddrPtr);
        Serial.println(" Sensor "+String(i)+" = "+macAddressAsString);
    }
  }

  Serial.println("Sensors: asking temperartures");
       
  sensors.begin();
  sensors.requestTemperatures();

  /* // Prevent noise on the onewire bus */
  /* delay(850); */
  
  Serial.println("Iterating over sensors");

  String msg = "[";
  int count = 0;
  int indexes[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  /* for (int i=0; i < 16; i++) { */
  /*    int n = random(0, 16);  // Integer from 0 to questionCount-1 */
  /*    int temp = indexes[n]; */
  /*    indexes[n] =  indexes[i]; */
  /*    indexes[i] = temp; */
  /* } */

  /* Serial.println("a"); */
  for (int i = 0; i < 16; i++) {
      delay(50);
      /* Serial.println("getting temperatures"); */
      /* Serial.println("a.1"); */
      int deviceIndex = indexes[i];
      DeviceAddress* devAddrPtr = addresses[deviceIndex];
      /* Serial.println("a.2"); */
      float temperature = sensors.getTempC(*devAddrPtr);  
      /* Serial.println("a.3");   */
      //float temperature = getTempC_raw(*devAddrPtr);
    
      float avg_temperature = temperature;
      
      /* Serial.println("a.4"); */
      float calibratedTemperature = getCalibratedTemperature(deviceIndex, temperature);
      String macAddressAsString = deviceAddressToString(*devAddrPtr);
      /* Serial.println("a.5"); */

      if (!isnan(temperature) && temperature > 0) {
          /* Serial.println("a.5.a1"); */
          Serial.println("["+String(deviceIndex)+"] --> "+ String(temperature)+" ("+String(avg_temperature)+")"+" ("+String(calibratedTemperature)+")");
          /* Serial.println("a.5.a2"); */


          if (temperatures_array_initialization_step > 3) {
              temperatures_array[deviceIndex][0] = temperatures_array[deviceIndex][1];
          }
          if (temperatures_array_initialization_step > 2) {
              temperatures_array[deviceIndex][1] = temperatures_array[deviceIndex][2];
          }
          if (temperatures_array_initialization_step > 1) {
              temperatures_array[deviceIndex][2] = temperatures_array[deviceIndex][3];
          }
          
          /* Serial.println("a.5.a3"); */
          temperatures_array[deviceIndex][2] = calibratedTemperature;

          float a = temperatures_array[deviceIndex][0];
          float b = temperatures_array[deviceIndex][1];
          float c = temperatures_array[deviceIndex][2];
          float d = temperatures_array[deviceIndex][3];
          /* Serial.println("a.5.a4"); */

          int skip_sending = 0;

          if (temperatures_array_initialization_step > 3) {
              // First case: \/ or /\ 
              if ((distance(a,b) >= 1.0 || distance(b,c) >= 1.0) && distance(a,c) <= 1.0) {
                  skip_sending = 1;
                  temperatures_array[deviceIndex][1] = a;
                  Serial.println("a:"+String(a)+" b:"+String(b)+" c:"+String(c));
              }
              //                      _
              // Second case: \_/ or / \ 
              else if(distance(a,b) >= 1.0 && distance(c,d) >= 1.0 && distance(a,d) <= 2.0) {
                  skip_sending = 2;
                  temperatures_array[deviceIndex][1] = a;
                  temperatures_array[deviceIndex][2] = a;
                  Serial.println("a:"+String(a)+" b:"+String(b)+" c:"+String(c)+" d:"+String(d));
              }
          }
          /* Serial.println("a.5.a5"); */

          if (!skip_sending) {
              String current_value = "{\"sensor\":\""+String(macAddressAsString)+"\",\"t\":\"T\",\"v\":"+String(b)+"}";
              if (count > 0) {
                  msg += ",";
              }
              msg += current_value;
              count += 1;
          } else {
              Serial.println("Skip Sending...." + String(skip_sending));
          }
          
          /* Serial.println("a.5.a6"); */
      } else {
          /* Serial.println("a.5.b1"); */
          // Print sensors that may have a problem
          Serial.println("["+String(deviceIndex)+"] (KO) --> "+ String(temperature)+" ("+String(avg_temperature)+")"+" ("+String(calibratedTemperature)+")");
          /* Serial.println("a.5.b2"); */
      }
      /* Serial.println("a.6"); */
  }
  
  /* Serial.println("b"); */
  msg += "]";

  cpt = (cpt + 1) % 30;  
  
  if (temperatures_array_initialization_step < 4) {  
    /* Serial.println("b.1a"); */
    temperatures_array_initialization_step += 1;
    /* Serial.println("b.1b"); */
  } else {
    /* Serial.println("b.2a"); */
    Serial.println("I will send data to the temperature service");
    HTTPClient http;
    /* Serial.println("b.2b"); */
    //http.begin("http://192.168.1.50:8080/temperature/list");
    http.begin("http://11.9.1.55:8080/temperature/list");
    http.addHeader("Content-Type", "application/json");
    /* Serial.println("b.2c"); */
    // Perform an HTTP POST request
    int httpCode = http.POST(msg);
    /* Serial.println("b.2d"); */
    String payload = http.getString();
    /* Serial.println("b.2e"); */
    http.end();
    /* Serial.println("b.2f"); */
    Serial.println("    - httpCode: "+String(httpCode));
    Serial.println("    - payload: "+String(payload));
    Serial.println("  -> Done sending data!");
    /* Serial.println("b.2g"); */
  }
  
  /* Serial.println("c"); */

  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);
  /* Serial.println("End of cycle"); */
}
