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
#define NB_TEMPERATURE_ENTRIES 7
float calibrationMap[NB_SENSORS][NB_TEMPERATURE_ENTRIES] = {
  { -0.95, -0.73, -0.35, -0.15, -0.14, -0.28,  0.32},
  { -0.67, -0.31,  0.03,  0.21,  0.24,  0.30,  0.51},
  { -0.52, -0.19,  0.12,  0.31,  0.38,  0.42,  0.66},
  { -0.36, -0.02,  0.39,  0.59,  0.62,  0.66,  0.92},
  { -0.07,  0.22,  0.62,  0.92,  1.00,  1.01,  1.31},
  { -0.63, -0.20,  0.17,  0.51,  0.54,  0.63,  0.92},
  { -1.62, -1.29, -1.04, -0.85, -0.85, -1.06, -0.84},
  { -1.13, -0.90, -0.44, -0.38, -0.30, -0.59,  0.06},
  { -1.36, -1.03, -0.71, -0.42, -0.27, -0.49, -0.15},
  { -1.65, -1.13, -0.82, -0.52, -0.49, -0.68, -0.09},
  { -0.88, -0.39, -0.04,  0.18,  0.25,  0.20,  0.48},
  { -1.45, -1.01, -0.57, -0.30, -0.28, -0.36,  0.04},
  { -1.02, -0.42, -0.02,  0.32,  0.37,  0.47,  0.74},
  {  0.04,  0.34,  0.74,  1.10,  1.19,  1.23,  1.51},
  { -2.11, -1.70, -1.39, -1.13, -1.15, -1.34, -1.51},
  { -1.63, -1.32, -0.88, -0.66, -0.73, -0.91, -0.51},
};

{% elif card_id == 2 %}

// Correction for card 'card2'
#define NB_SENSORS 16
#define NB_TEMPERATURE_ENTRIES 7
float calibrationMap[NB_SENSORS][NB_TEMPERATURE_ENTRIES] = {
  { -1.22, -0.85, -0.56, -0.43, -0.32, -0.30,  0.14},
  {  0.01,  0.48,  0.80,  0.89,  1.01,  1.08,  1.36},
  {  0.07,  0.59,  0.86,  1.02,  1.07,  1.26,  1.57},
  { -0.30,  0.37,  0.67,  0.86,  0.88,  1.06,  1.34},
  {  0.66,  1.22,  1.50,  1.66,  1.83,  2.02,  2.35},
  { -3.58, -3.26, -2.88, -2.88, -2.71, -2.74, -2.21},
  { -3.38, -2.95, -2.64, -2.66, -2.55, -2.67, -2.07},
  { -0.92, -0.47, -0.12,  0.01,  0.04,  0.02,  0.50},
  { -1.29, -0.87, -0.54, -0.39, -0.22, -0.32,  0.12},
  { -0.87, -0.33, -0.09,  0.09,  0.16,  0.17,  0.66},
  {  0.05,  0.63,  0.86,  1.11,  1.16,  1.36,  1.50},
  {  1.05,  1.67,  1.89,  2.06,  2.06,  2.38,  2.69},
  {  1.23,  1.81,  2.04,  2.27,  2.30,  2.59,  2.79},
  { -2.46, -2.11, -1.70, -1.74, -1.59, -1.57, -0.94},
  { -1.21, -0.84, -0.49, -0.36, -0.25, -0.30,  0.30},
  { -0.60, -0.24,  0.12,  0.16,  0.18,  0.24,  0.62},
};

{% elif card_id == 3 %}

// Correction for card 'card3'
#define NB_SENSORS 16
#define NB_TEMPERATURE_ENTRIES 7
float calibrationMap[NB_SENSORS][NB_TEMPERATURE_ENTRIES] = {
  { -1.12, -0.77, -0.66, -0.47, -0.33, -0.25,  0.26},
  {  0.08,  0.36,  0.49,  0.59,  0.74,  0.82,  1.34},
  { -0.73, -0.47, -0.23, -0.05,  0.08,  0.13,  0.72},
  { -0.80, -0.39, -0.23, -0.07,  0.03,  0.16,  0.60},
  { -0.79, -0.26,  0.01,  0.16,  0.16,  0.37,  0.81},
  { -0.63, -0.11,  0.20,  0.31,  0.48,  0.67,  1.13},
  { -2.23, -1.74, -1.81, -1.54, -1.37, -1.47, -0.90},
  { -1.90, -1.60, -1.63, -1.31, -1.14, -1.14, -0.63},
  { -2.09, -1.62, -1.36, -1.23, -1.09, -1.09, -0.61},
  { -1.33, -0.94, -0.64, -0.52, -0.42, -0.40,  0.08},
  { -0.74, -0.30, -0.01,  0.21,  0.21,  0.38,  0.81},
  { -1.77, -1.30, -1.06, -0.93, -0.75, -0.73, -0.36},
  {  0.40,  0.82,  1.12,  1.19,  1.33,  1.39,  1.88},
  { -0.58, -0.10,  0.32,  0.34,  0.46,  0.62,  1.00},
  { -0.87, -0.37, -0.30, -0.19, -0.05, -0.09,  0.36},
  { -1.53, -0.98, -0.69, -0.55, -0.51, -0.44, -0.14},
};

{% elif card_id == 4 %}

// Correction for card 'card4'
#define NB_SENSORS 16
#define NB_TEMPERATURE_ENTRIES 7
float calibrationMap[NB_SENSORS][NB_TEMPERATURE_ENTRIES] = {
  { -0.60, -0.24,  0.12,  0.16,  0.18,  0.24,  0.62},
  {  0.15,  0.31,  0.74,  0.76,  0.84,  0.81,  1.22},
  {  0.58,  0.96,  1.28,  1.38,  1.43,  1.44,  1.76},
  {  0.13,  0.56,  0.80,  1.01,  1.01,  1.11,  1.39},
  {  1.51,  1.86,  2.15,  2.38,  2.49,  2.55,  2.87},
  {  0.66,  1.24,  1.51,  1.75,  1.85,  1.97,  2.17},
  { -3.19, -2.99, -2.59, -2.49, -2.49, -2.43, -1.97},
  { -2.68, -2.30, -1.96, -1.94, -1.87, -1.81, -1.43},
  { -1.54, -1.15, -0.85, -0.68, -0.59, -0.56, -0.30},
  { -1.28, -0.85, -0.50, -0.37, -0.20, -0.20,  0.22},
  { -0.69, -0.15,  0.13,  0.24,  0.31,  0.43,  0.67},
  {  0.00,  0.54,  0.78,  0.98,  1.04,  1.23,  1.44},
  {  0.80,  1.24,  1.56,  1.76,  1.88,  2.09,  2.31},
  {  0.81,  1.47,  1.78,  1.95,  2.15,  2.23,  2.47},
  { -3.54, -3.18, -2.87, -2.81, -2.67, -2.63, -2.20},
  { -1.11, -0.70, -0.45, -0.40, -0.25, -0.15,  0.28},
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
  int calib_t_min = 0;
  int calib_t_max = 60;

  if (uncalibratedTemperature <= calib_t_min) {
    return uncalibratedTemperature - calibrationMap[deviceIndex][0];
  }

  if (uncalibratedTemperature >= calib_t_max) {
    return uncalibratedTemperature - calibrationMap[deviceIndex][6];
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
  Serial.println("Hello!");

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
  
  for (int i = 0; i < 16; i++) {
      Serial.println("getting temperatures");
      int deviceIndex = indexes[i];
      DeviceAddress* devAddrPtr = addresses[deviceIndex];
      float temperature = sensors.getTempC(*devAddrPtr);    
      //float temperature = getTempC_raw(*devAddrPtr);
    
      float avg_temperature = temperature;
      
      float calibratedTemperature = getCalibratedTemperature(deviceIndex, temperature);
      String macAddressAsString = deviceAddressToString(*devAddrPtr);

      if (!isnan(temperature) && temperature > 0) {
        Serial.println("["+String(deviceIndex)+"] --> "+ String(temperature)+" ("+String(avg_temperature)+")"+" ("+String(calibratedTemperature)+")");

        if (temperatures_array_initialization_step > 3) {
          temperatures_array[deviceIndex][0] = temperatures_array[deviceIndex][1];
        }
        if (temperatures_array_initialization_step > 2) {
          temperatures_array[deviceIndex][1] = temperatures_array[deviceIndex][2];
        }
        if (temperatures_array_initialization_step > 1) {
          temperatures_array[deviceIndex][2] = temperatures_array[deviceIndex][3];
        }
        temperatures_array[deviceIndex][2] = calibratedTemperature;

        float a = temperatures_array[deviceIndex][0];
        float b = temperatures_array[deviceIndex][1];
        float c = temperatures_array[deviceIndex][2];
        float d = temperatures_array[deviceIndex][3];

        int skip_sending = 0;

        if (temperatures_array_initialization_step > 3) {
          // First case: \/
          if ((distance(a,b) >= 1.0 || distance(b,c) >= 1.0 ) && distance(a,c) <= 1.0) {
            skip_sending = 1;
            temperatures_array[deviceIndex][1] = a;
          }
          // Second case: \_/
          else if(distance(a,b) >= 1.0 && distance(c,d) >= 1.0 && distance(a,d) <= 1.0) {
            skip_sending = 1;
            temperatures_array[deviceIndex][1] = a;
            temperatures_array[deviceIndex][2] = a;
          }
        }

        if (!skip_sending) {
          String current_value = "{\"sensor\":\""+String(macAddressAsString)+"\",\"t\":\"T\",\"v\":"+String(b)+"}";
          if (count > 0) {
            msg += ",";
          }
          msg += current_value;
          count += 1;
        }
      } else {
        // Print sensors that may have a problem
        Serial.println("["+String(deviceIndex)+"] (KO) --> "+ String(temperature)+" ("+String(avg_temperature)+")"+" ("+String(calibratedTemperature)+")");
      }
  }
  msg += "]";

  cpt = (cpt + 1) % 30;


  
  
  if (temperatures_array_initialization_step < 4) {
    temperatures_array_initialization_step += 1;
  } else {
    Serial.println("I will send data to the temperature service");
    HTTPClient http;
    //http.begin("http://192.168.1.50:8080/temperature/list");
    http.begin("http://11.9.1.55:8080/temperature/list");
    http.addHeader("Content-Type", "application/json");
    // Perform an HTTP POST request
    int httpCode = http.POST(msg);
    String payload = http.getString();
    http.end();
    Serial.println("    - httpCode: "+String(httpCode));
    Serial.println("    - payload: "+String(payload));
    Serial.println("  -> Done sending data!");
  }

  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);
  /* Serial.println("End of cycle"); */
}
