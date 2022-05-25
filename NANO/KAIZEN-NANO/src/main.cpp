#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Seeed_HM330X.h>
#include "sensirion_common.h"
#include <Wire.h>
//#include "DHT.h"
#include "SCD30.h"
#include "Multichannel_Gas_GMXXX.h"
#include "BareBoneSim800.h"
#define SIM800_TX_PIN 2  //SIM800 TX is connected to Arduino D2  
#define SIM800_RX_PIN 3 //SIM800 RX is connected to Arduino D3
//I2C A4 SDA -- A5 SCL
#define battPin A0
int PWX=4; //pull up pin for SIM800


  #ifdef  ARDUINO_SAMD_VARIANT_COMPLIANCE
    #define SERIAL_OUTPUT SerialUSB
#else
    #define SERIAL_OUTPUT Serial
#endif
 
// Communication ==========================================================================
//SoftwareSerial serialSIM800(SIM800_TX_PIN,SIM800_RX_PIN);
// BareBoneSim800 serialSIM800("afrihost"); 
char server[] = "http://ie-gtfs.up.ac.za";
char path[] = "/data/z-nano.php";
int port = 80; // port 80 is the default for HTTP
String postData;

// Sensors =======================================================
  HM330X sensor_HM330X;
  uint8_t buf[30];
  uint16_t HM330X_values[8];
  /*parse buf with 29 uint8_t-data*/
  HM330XErrorCode parse_result(uint8_t* data, uint16_t* out)
   {
    uint16_t value = 0;
    if (NULL == data) {
        return ERROR_PARAM;
    }
    for (int i = 1; i < 8; i++) {
        value = (uint16_t) data[i * 2] << 8 | data[i * 2 + 1];
      out[i-1] = value;

    }
    return NO_ERROR;
}

//DHT20 ===========================================================
// DHT dht(DHT20); 

//===========MULTI GAS
 
    GAS_GMXXX<TwoWire> multi_gas;
//

 
void setup() {
  scd30.initialize();  
  pinMode(PWX, OUTPUT);
 
  Serial.begin(9600);   
  while(!Serial);    
  //serialSIM800.begin(9600);
  serialSIM800.begin();
   delay(8000);
  
 serialSIM800.print("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
`checkResponse()
serialSIM800.print("AT+SAPBR=3,1,\"APN\",\"afrihost\"");
delay(2000);
serialSIM800.print("AT+SAPBR=1,1");
delay(2000);
serialSIM800.print("AT+SAPBR=2,1");
delay(2000);

  
  /* SENSOR INITIATION:

  */

 //dht.begin();
 multi_gas.begin(Wire,0x08);
 sensor_HM330X.init();

}
 
void loop() {
  // //Read SIM800 output (if available) and print it in Arduino IDE Serial Monitor
  // if(serialSIM800.available()){
  //   Serial.write(serialSIM800.read());
  // }
  // //Read Arduino IDE Serial Monitor inputs (if available) and send them to SIM800
  // if(Serial.available()){    
  //   serialSIM800.write(Serial.read());
  // }
int val = 0;
val = analogRead(battPin);  // read the input pin
float batt_m = 4850 * float(val)/1024;

sensor_HM330X.read_sensor_value(buf, 29) ;
 parse_result(buf,HM330X_values);

// float SENO_conc=0;
// float voltage =  analogRead(SEN019)*(5000/1024.0);
// Serial.println("Volts" + String(voltage));
//  if(voltage == 0)
//  {
//  Serial.println("Fault");
//  }
//  else if(voltage < 400)
//  { //pre-heating
//  } else SENO_conc=(voltage-400)*50.0/16.0; 
 
//  float SENO_concentration =0;
//   if(flag == 4){
//     flag = 1;
//     float pwm_high_val_ms = (pwm_high_val * 1000) / (pwm_low_val + pwm_high_val);

//     if (pwm_high_val_ms < 0.01){
//       Serial.println("Fault");
//     }
//     else if (pwm_high_val_ms < 80.00){
//       Serial.println("preheat");
//     }
//     else if (pwm_high_val_ms < 998.00){
//        SENO_concentration = (pwm_high_val_ms - 2) * 5;
//       // Print pwm_high_val_ms
//       Serial.print("pwm_high_val_ms:");
//       Serial.print(pwm_high_val_ms);
//       Serial.println("ms");
//       //Print CO2 concentration
//       Serial.print(SENO_concentration);
//       Serial.println("ppm");
//     }
//     else{
//       Serial.println("Beyond the maximum range ：398~4980ppm");
//     }
//     Serial.println();
//   }


float result[3] = {0}; //0: co2, 1: temp, 2: hum
scd30.getCarbonDioxideConcentration(result);

uint32_t val_NO2 = multi_gas.measure_NO2();
uint32_t val_C2H50H = multi_gas.measure_C2H5OH();
uint32_t val_VOC = multi_gas.measure_VOC();
uint32_t val_CO = multi_gas.measure_CO();
Serial.println("N02: " +String(val_NO2) + " C2H50H: " +String(val_C2H50H) + " VOC: " +String(val_VOC )+ " CO: " +String(val_CO));
for(int i=1; i<3;i++)
 Serial.println(" value" +String(i)+'='+String(HM330X_values[i]));

Serial.println("CO2: " + String(result[0]) +" " + result[1] + " " + result[2]);
// Serial.println("SENO: " + String(SENO_conc));
delay(1000);

postData= "?";
postData+= "Sensor_ID=SEN_0" ;
postData+= "&PM2=" +String(HM330X_values[1]);
postData+= "&PM10="+String(HM330X_values[2]);
postData+= "&VOC="+String(val_VOC);
postData+= "&C0=" +String(val_CO);
postData+= "&C02=" +String(result[0]);
postData+= "&Temperature="+String(result[1]);
// postData+= "&str=" + 2;
// postData+= ":" +String(HM330X_values[1]);
// postData+= ":"+String(HM330X_values[2]);
// postData+= ":"+String(val_VOC);
// postData+= ":" +String(val_CO);
// postData+= ":" +String(result[0]);
// postData+= ":"+String(result[1]);
//postData = "?FROMARDUINO=100\"";

// serialSIM800.print("AT+HTTPINIT");
// serialSIM800.print( "AT+HTTPPARA=\"CID\",1");

//String url = "AT+HTTPPARA=\"URL\",\""+String(server)+path + String(postData) +"\"";
String url = String(server)+path + String(postData);
 Serial.println("Making HTTP Get Request");
  String response = serialSIM800.sendHTTPData(url.c_str());
    Serial.println("Received Info: ");
    Serial.println(response);
// serialSIM800.print(url); //url.c_str()
// serialSIM800.print("AT+HTTPPARA=\"PROPORT\",\"80\"");
// serialSIM800.print("AT+HTTPACTION=0");

// _readSerial();

                
}
 