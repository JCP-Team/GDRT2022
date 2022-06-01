#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Seeed_HM330X.h>
#include "sensirion_common.h"
#include "SIM800C.h"
#include <Wire.h>
#include "SCD30.h"
#include "Multichannel_Gas_GMXXX.h"

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

SIM800C* sim;
 
void setup() {
  pinMode(PWX, OUTPUT);
  sim = new SIM800C(SIM800_TX_PIN,SIM800_RX_PIN);
  
  digitalWrite(PWX, HIGH);
  delay(3000);
  digitalWrite(PWX, LOW);    
  delay(1000);


  Serial.begin(9600);
  scd30.initialize();  
 
  while(!Serial);    
  sim->apn("afrihost");

  while(!sim->init()) {
    Serial.println("LOADING SIM");
    delay(1000);
  }
 
  Serial.println("SIM OK");
  Serial.println("Initialised HTTP with response: ");
  String response = (sim->http_init())? "OK INIT": "BAD INIT";
  Serial.println(response);
}
int i =0;
 
void loop() {

postData = "?FROMARDUINO=100";


Serial.println("Making HTTP Get Request with response:");
 String response = sim->http_send(postData);
//  sim->disable_error_msg(); // enable verbose error message in http_send, but should disable for other methods.
Serial.println(response);            
delay(60000);             
   
}
 