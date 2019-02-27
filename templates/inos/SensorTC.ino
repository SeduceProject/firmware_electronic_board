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
OneWire  ds(D3);  // on pin 10 (a 4.7K pullup resistor is necessary)
DallasTemperature sensors(&ds);

float calibrationMap[16][8] = {
  { -0.22, -0.07, 0.05, 0.23, 0.27, 0.41, 0.27, 0.28 },
  { -1.33, -0.93, -0.30, -0.11, -0.02, 0.07, 0.02, 0.31 },
  { -0.86, -0.64, -0.12, -0.10, 0.21, 0.10, 0.30, 0.24 },
  { -1.39, -1.32, -0.90, -0.77, -0.33, -0.49, -0.51, -0.48 },
  { -1.57, -1.16, -0.94, -0.66, -0.58, -0.47, -0.48, -0.52 },
  { -1.19, -0.68, -0.45, -0.09, -0.12, 0.03, 0.08, 0.02 },
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  { -1.19, -0.94, -0.62, -0.47, -0.35, -0.33, -0.25, -0.25 },
  { -0.88, -0.29, -0.07, 0.30, 0.48, 0.51, 0.57, 0.65 },
  { -1.13, -0.47, -0.31, -0.04, 0.11, 0.18, 0.23, 0.33 },
  { -1.36, -0.68, -0.24, 0.06, 0.28, 0.47, 0.39, 0.24 },
  { -1.14, -0.50, -0.24, -0.02, 0.21, 0.43, 0.49, 0.47 },
  { -1.51, -1.07, -0.69, -0.42, -0.23, 0.02, -0.16, -0.08 }
};

// function to convert a device address to a String
String deviceAddressToString(DeviceAddress deviceAddress)
{
  String result = "";
  for (uint8_t i = 0; i < 8; i++)
  {
    //if (i > 0) {
    //  result += ":";
    //}
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
  int calib_t_min = -10;
  int calib_t_max = 60;

  if (uncalibratedTemperature <= calib_t_min) {
    return uncalibratedTemperature - calibrationMap[deviceIndex][0];
  }

  if (uncalibratedTemperature >= calib_t_max) {
    return uncalibratedTemperature - calibrationMap[deviceIndex][7];
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

float getTempC_raw(DeviceAddress* devAddr) {
  byte addr[8];
  byte i;
  byte present = 0;
  byte temptype;
  byte data[12];
  float celsius, fahrenheit;

  for (i=0; i<8; i++) {
    addr[i] = *devAddr[i];
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      return sqrt(-1);
  } 

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(750);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);
  
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }
  
  int16_t raw = (data[1] << 8) | data[0];
  if (raw & 0x01) {
    return sqrt(-1);
  }
  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
  Serial.print("  *Temperature = ");
  Serial.print(celsius);
  Serial.print(" *Celsius, ");
  Serial.print(fahrenheit);
  Serial.println(" *Fahrenheit");
}

float askTemperature(DeviceAddress devAddr) {
  byte i;
  byte present = 0;
  byte temptype;
  byte data[12];
  float celsius, fahrenheit;
  byte addr[8];

  for(int i = 0; i < 8; i++) {
    addr[i] = devAddr[i];
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      return sqrt(-1);
  } 

  
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end

  return -1;
}

float getTempC_raw(DeviceAddress devAddr) {
  byte i;
  byte present = 0;
  byte temptype;
  byte data[12];
  float celsius, fahrenheit;
  byte addr[8];

  for(int i = 0; i < 8; i++) {
    addr[i] = devAddr[i];
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      return sqrt(-1);
  } 

  /*
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(100);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  */
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);
  
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }
  
  int16_t raw = (data[1] << 8) | data[0];
  if (raw & 0x01) {
    return sqrt(-1);
  }
  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;

  /*
  Serial.print("  *Temperature = ");
  Serial.print(celsius);
  Serial.print(" *Celsius, ");
  Serial.print(fahrenheit);
  Serial.println(" *Fahrenheit");
  */

  return celsius;
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
void loop(void) {
  Serial.println("Start of cycle");
  if (cpt == 0) {    
    for (int i =0; i< 16; i++) {
        DeviceAddress* devAddrPtr = addresses[i];
        String macAddressAsString = deviceAddressToString(*devAddrPtr);
        Serial.println(" Sensor "+String(i)+" = "+macAddressAsString);
    }
  }
  
  Serial.println("Asking temperatures");
  
  for (int deviceIndex = 0; deviceIndex < 16; deviceIndex++) {
    DeviceAddress* devAddrPtr = addresses[deviceIndex];
    askTemperature(*devAddrPtr);
  }
  Serial.println(" -> Done asking temperatures");
  delay(850);
  //sensors.begin();
  //sensors.requestTemperatures();
  Serial.println("Iterating over sensors");

  String msg = "[";
  int count = 0;
  for (int deviceIndex = 0; deviceIndex < 16; deviceIndex++) {
      DeviceAddress* devAddrPtr = addresses[deviceIndex];
      float temperature = sensors.getTempC(*devAddrPtr);    
      //float temperature = getTempC_raw(*devAddrPtr);
    
      float avg_temperature = temperature;
      
      float calibratedTemperature = getCalibratedTemperature(deviceIndex, temperature);
      String macAddressAsString = deviceAddressToString(*devAddrPtr);

      if (!isnan(temperature) && temperature > 0) {        
        Serial.println("["+String(deviceIndex)+"] --> "+ String(temperature)+" ("+String(avg_temperature)+")"+" ("+String(calibratedTemperature)+")");
        
        // Perform an HTTP get request
        if (count > 0) {
          msg += ",";
        }
        msg += "{\"sensor\":\""+String(macAddressAsString)+"\",\"t\":\"T\",\"v\":"+String(calibratedTemperature)+"}";
        count += 1;
      }
    
  }
  msg += "]";

  cpt = (cpt + 1) % 30;

  Serial.println("I will send data to the temperature service");
  HTTPClient http;
  //http.begin("http://192.168.1.50:8080/temperature/list");
  http.begin("http://192.168.1.30:8080/temperature/list");
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(msg);
  String payload = http.getString();
  http.end();
  Serial.println("    - httpCode: "+String(httpCode));
  Serial.println("    - payload: "+String(payload));
  Serial.println("  -> Done sending data!");

  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("End of cycle");
}
