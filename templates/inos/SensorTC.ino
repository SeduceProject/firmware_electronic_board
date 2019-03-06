#include <OneWire.h>
#include <DallasTemperature.h>
#include <math.h>


#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>


///////////////////////////////////////////////////////
// Stuff related to WIFI
///////////////////////////////////////////////////////

// variables related to WIFI
char ssid[] = "CloudNet";     //  your network SSID (name)
char password[] = "CloudNet";  // your network password
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

float calibrationMap[16][8] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
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

/* float getTempC_raw(DeviceAddress* devAddr) { */
/*   byte addr[8]; */
/*   byte i; */
/*   byte present = 0; */
/*   byte temptype; */
/*   byte data[12]; */
/*   float celsius, fahrenheit; */

/*   for (i=0; i<8; i++) { */
/*     addr[i] = *devAddr[i]; */
/*   } */

/*   if (OneWire::crc8(addr, 7) != addr[7]) { */
/*       return sqrt(-1); */
/*   }  */

/*   ds.reset(); */
/*   ds.select(addr); */
/*   ds.write(0x44, 1);        // start conversion, with parasite power on at the end */
  
/*   delay(750);     // maybe 750ms is enough, maybe not */
/*   // we might do a ds.depower() here, but the reset will take care of it. */
  
/*   present = ds.reset(); */
/*   ds.select(addr);     */
/*   ds.write(0xBE); */
  
/*   for ( i = 0; i < 9; i++) {           // we need 9 bytes */
/*     data[i] = ds.read(); */
/*   } */
  
/*   int16_t raw = (data[1] << 8) | data[0]; */
/*   if (raw & 0x01) { */
/*     return sqrt(-1); */
/*   } */
/*   celsius = (float)raw / 16.0; */
/*   fahrenheit = celsius * 1.8 + 32.0; */
/*   Serial.print("  *Temperature = "); */
/*   Serial.print(celsius); */
/*   Serial.print(" *Celsius, "); */
/*   Serial.print(fahrenheit); */
/*   Serial.println(" *Fahrenheit"); */
/* } */

/* float askTemperature(DeviceAddress devAddr) { */
/*   byte i; */
/*   byte present = 0; */
/*   byte temptype; */
/*   byte data[12]; */
/*   float celsius, fahrenheit; */
/*   byte addr[8]; */

/*   for(int i = 0; i < 8; i++) { */
/*     addr[i] = devAddr[i]; */
/*   } */

/*   if (OneWire::crc8(addr, 7) != addr[7]) { */
/*       return sqrt(-1); */
/*   } */

  
/*   ds.reset(); */
/*   ds.select(addr); */
/*   ds.write(0x44, 1);        // start conversion, with parasite power on at the end */

/*   return -1; */
}

/* float getTempC_raw(DeviceAddress devAddr) { */
/*   byte i; */
/*   byte present = 0; */
/*   byte temptype; */
/*   byte data[12]; */
/*   float celsius, fahrenheit; */
/*   byte addr[8]; */

/*   for(int i = 0; i < 8; i++) { */
/*     addr[i] = devAddr[i]; */
/*   } */

/*   if (OneWire::crc8(addr, 7) != addr[7]) { */
/*       return sqrt(-1); */
/*   }  */

/*   /\* */
/*   ds.reset(); */
/*   ds.select(addr); */
/*   ds.write(0x44, 1);        // start conversion, with parasite power on at the end */
  
/*   delay(100);     // maybe 750ms is enough, maybe not */
/*   // we might do a ds.depower() here, but the reset will take care of it. */
/*   *\/ */
  
/*   present = ds.reset(); */
/*   ds.select(addr);     */
/*   ds.write(0xBE); */
  
/*   for ( i = 0; i < 9; i++) {           // we need 9 bytes */
/*     data[i] = ds.read(); */
/*   } */
  
/*   int16_t raw = (data[1] << 8) | data[0]; */
/*   if (raw & 0x01) { */
/*     return sqrt(-1); */
/*   } */
/*   celsius = (float)raw / 16.0; */
/*   fahrenheit = celsius * 1.8 + 32.0; */

/*   /\* */
/*   Serial.print("  *Temperature = "); */
/*   Serial.print(celsius); */
/*   Serial.print(" *Celsius, "); */
/*   Serial.print(fahrenheit); */
/*   Serial.println(" *Fahrenheit"); */
/*   *\/ */

/*   return celsius; */
/* } */

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
  /* WiFi.setOutputPower(0); */
  /* WiFi.forceSleepBegin(); */
}

int cpt = 0;
float temperatures_array[16][3];
int temperatures_array_initialization_step = 0;

void loop(void) {
  Serial.println("Start of cycle");
  if (cpt == 0) {
    for (int i =0; i< 16; i++) {
        DeviceAddress* devAddrPtr = addresses[i];
        String macAddressAsString = deviceAddressToString(*devAddrPtr);
        Serial.println(" Sensor "+String(i)+" = "+macAddressAsString);
    }
  }
       
  sensors.begin();
  sensors.requestTemperatures();

  /* // Prevent noise on the onewire bus */
  /* delay(850); */
  
  /* Serial.println("Iterating over sensors"); */

  String msg = "[";
  int count = 0;
  int indexes[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  for (int i=0; i < 16; i++) {
     int n = random(0, 16);  // Integer from 0 to questionCount-1
     int temp = indexes[n];
     indexes[n] =  indexes[i];
     indexes[i] = temp;
  }
  
  for (int i = 0; i < 16; i++) {
      int deviceIndex = indexes[i];
      DeviceAddress* devAddrPtr = addresses[deviceIndex];
      float temperature = sensors.getTempC(*devAddrPtr);    
      //float temperature = getTempC_raw(*devAddrPtr);
    
      float avg_temperature = temperature;
      
      float calibratedTemperature = getCalibratedTemperature(deviceIndex, temperature);
      String macAddressAsString = deviceAddressToString(*devAddrPtr);

      if (!isnan(temperature) && temperature > -10) {
        /* Serial.println("["+String(deviceIndex)+"] --> "+ String(temperature)+" ("+String(avg_temperature)+")"+" ("+String(calibratedTemperature)+")"); */
        
        if (count > 0) {
          msg += ",";
        }
        

        if (temperatures_array_initialization_step > 2) {
          temperatures_array[deviceIndex][0] = temperatures_array[deviceIndex][1];
        }
        if (temperatures_array_initialization_step > 1) {
          temperatures_array[deviceIndex][1] = temperatures_array[deviceIndex][2];
        }
        temperatures_array[deviceIndex][2] = calibratedTemperature;

        float a = temperatures_array[deviceIndex][0];
        float b = temperatures_array[deviceIndex][1];
        float c = temperatures_array[deviceIndex][2];

        int skip_sending = 0;

        if (distance(a,b) > 1.0 && distance(a,c) < 1.0) {
          skip_sending = 1;
        }

        if (!skip_sending) {
          String current_value = "{\"sensor\":\""+String(macAddressAsString)+"\",\"t\":\"T\",\"v\":"+String(b)+"}";
          msg += current_value;
          count += 1;
        }


        /* // Prevent noise on the OneWire bus */
        /* if (deviceIndex > 10) { */
        /*   delay(100); */
        /* } else { */
        /*   delay(50); */
        /* } */
        /* delay(50); */
      }
  }
  msg += "]";

  cpt = (cpt + 1) % 30;

  /* Serial.println("Turning on wifi"); */
  /* WiFi.setOutputPower(5); */
  /* delay(1); */
  /* // Bring up the WiFi connection */
  /* WiFi.mode( WIFI_STA ); */
  /* /\* WiFi.begin( WLAN_SSID, WLAN_PASSWD ); *\/ */
  /* WiFi.begin(ssid, password); */

  /* Serial.println("I will send data to the temperature service"); */
  HTTPClient http;
  //http.begin("http://192.168.1.50:8080/temperature/list");
  http.begin("http://10.1.0.4:8080/temperature/list");
  http.addHeader("Content-Type", "application/json");
  // Perform an HTTP POST request
  int httpCode = http.POST(msg);
  String payload = http.getString();
  http.end();
  /* Serial.println("    - httpCode: "+String(httpCode)); */
  /* Serial.println("    - payload: "+String(payload)); */
  /* Serial.println("  -> Done sending data!"); */
  
  /* Serial.println("Turning off wifi"); */
  /* WiFi.setOutputPower(0); */
  /* WiFi.forceSleepBegin(); */
  
  if (temperatures_array_initialization_step < 3) {
    temperatures_array_initialization_step += 1;
  }

  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);
  /* Serial.println("End of cycle"); */
}
