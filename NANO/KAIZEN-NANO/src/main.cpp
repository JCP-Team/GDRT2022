#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Seeed_HM330X.h>
#include "sensirion_common.h"
#include "SIM800C.h"
#include <Wire.h>
#include "SCD30.h"
#include "Multichannel_Gas_GMXXX.h"
#define ON true
#define OFF false
#define SIM800_TX_PIN 2  //SIM800 TX is connected to Arduino D2  
#define SIM800_RX_PIN 3 //SIM800 RX is connected to Arduino D3
//I2C A4 SDA -- A5 SCL
#define battPin A0
int PWX=4; //pull up pin for SIM800
int BASE= 5; //Base of transistor.
 
// Communication ==========================================================================
SIM800C* sim;
char server[] = "http://ie-gtfs.up.ac.za";
char path[] = "/data/z-nano.php";
int port = 80; // port 80 is the default for HTTP
String postData;
String DEVICE_ID ="C01";
bool HTTPINIT=false;
#define WAIT 10000
#define WARMUP 20000

// Sensors =======================================================
  void sensor_state(bool state){ //1 is on, 0 is off
    if(state) digitalWrite(BASE,LOW);
    else digitalWrite(BASE,HIGH);
  }

  GAS_GMXXX<TwoWire> multi_gas;
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

 
void setup() {
  pinMode(PWX, OUTPUT);
  pinMode(BASE,OUTPUT);
  sim = new SIM800C(SIM800_TX_PIN,SIM800_RX_PIN);
  
  // digitalWrite(PWX, HIGH);
  // delay(3000);
  // digitalWrite(PWX, LOW);    
  // delay(1000);
  sensor_state(ON);
  digitalWrite(PWX, LOW);
  delay(3000);
  digitalWrite(PWX, HIGH);    
  delay(1000);


   multi_gas.begin(Wire,0x08);
   sensor_HM330X.init();

  Serial.begin(9600);
  scd30.initialize();  
 
 // while(!Serial);    
  sim->apn("afrihost");

  while(!sim->init()) {
    Serial.println("LOADING SIM");
    delay(1000);
  }
 
 // Serial.println("SIM OK");
  HTTPINIT=sim->http_init();
 Serial.println("Initialised HTTP with response: ");
 (HTTPINIT)? Serial.println("OK INIT"): Serial.println("BAD INIT"); 
}
int i =0;
 
void loop() {
int attempts =0;
while(!HTTPINIT){
  HTTPINIT=sim->http_init(); 
  if(attempts++ == 5){
    digitalWrite(PWX, HIGH);
  delay(3000);
  digitalWrite(PWX, LOW);    
  delay(1000);
  }
  delay(500);
}
HTTPINIT=false;

int val = 0;
val = analogRead(battPin); 
float batt_m = 4850 * float(val)/1024; //manually measured reference voltage 4850 mV

sensor_HM330X.read_sensor_value(buf, 29) ;
 parse_result(buf,HM330X_values);
float result[3] = {0}; //0: co2, 1: temp, 2: hum
scd30.getCarbonDioxideConcentration(result);

uint32_t val_NO2 = multi_gas.measure_NO2();
uint32_t val_C2H50H = multi_gas.measure_C2H5OH();
uint32_t val_VOC = multi_gas.measure_VOC();
uint32_t val_CO = multi_gas.measure_CO();

// Serial.println("N02: " +String(val_NO2) + " C2H50H: " +String(val_C2H50H) + " VOC: " +String(val_VOC )+ " CO: " +String(val_CO));
// for(int i=1; i<3;i++)
//  Serial.println(" value" +String(i)+'='+String(HM330X_values[i]));

//Serial.println("CO2: " + String(result[0]) +" " + result[1] + " " + result[2]);

postData= "?";
postData+= "ID=" +DEVICE_ID;
postData+= "&BATT="+String(batt_m);
postData+= "&PM2=" +String(HM330X_values[1]);
postData+= "&PM10="+String(HM330X_values[2]);
postData+= "&VOC="+String(val_VOC);
postData+= "&C0=" +String(val_CO);
postData+= "&C02=" +String(result[0]);
postData+= "&Temperature="+String(result[1]);

//Serial.println(postData);
Serial.println("Making HTTP Get Request with response:");
String response = sim->http_send(postData);
//  sim->disable_error_msg(); // enable verbose error message in http_send, but should disable for other methods.
Serial.println(response);            

// sensor_state(OFF);
// sim->sleep();
// delay(WAIT); 
// sensor_state(ON);
// delay(1000);
// sim->wake();
//   delay(100);
//  delay(WARMUP);          
}
 