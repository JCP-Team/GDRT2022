#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Seeed_HM330X.h>
#include "sensirion_common.h"
#include "SIM800.h"
#include <Wire.h>
#include "SCD30.h"
#include "Multichannel_Gas_GMXXX.h"

#define SIM800_TX_PIN 2  //SIM800 TX is connected to Arduino D2  
#define SIM800_RX_PIN 3 //SIM800 RX is connected to Arduino D3
//I2C A4 SDA -- A5 SCL
#define battPin A0
int PWX=5; //Power pin for SIM800
int BASE= 10; //Relay input pin 
 
// Communication ==========================================================================
SIM800* sim;
String URL = "http://ie-gtfs.up.ac.za/data/z-nano.php";
const char APN[9] = "afrihost";

int port = 80; // port 80 is the default for HTTP
String postData;
String DEVICE_ID ="ZLP001";
bool HTTPINIT=false;
#define WAIT  30000//360000   //Time in miliseconds sensors are off
#define WARMUP 30000//120000 //Time in miliseconds sensors are switched on for warm-up

// Sensors =======================================================
#define ON true
#define OFF false
  void external_state(bool state){  //Switches relay ON or OFF
    if(state) digitalWrite(BASE,HIGH);
    else digitalWrite(BASE,LOW);
  }

  GAS_GMXXX<TwoWire> multi_gas;
  HM330X sensor_HM330X;
  uint8_t buf[30];
  uint16_t HM330X_values[8];
  /*Using Code Sample from: https://wiki.seeedstudio.com/Grove-Laser_PM2.5_Sensor-HM3301/
    Parsing buffer with 29 uint8_t-data */
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

void reset_sim(){ //Triggers SIM800 module to reset
  digitalWrite(PWX, LOW);
  delay(3000);
  digitalWrite(PWX, HIGH);    
  delay(1000);
}

void indicate_state(int num){ //Debugging Function
  int i=num;
  while((i)>0){
    digitalWrite(LED_BUILTIN,HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN,LOW);
    delay(250);
    i--;
  }
}

 
void setup() {
  
  pinMode(LED_BUILTIN,OUTPUT); // Debugging purposes
  pinMode(PWX, OUTPUT);        //Setting PIN states
  pinMode(BASE,OUTPUT);
  sim = new SIM800(SIM800_TX_PIN,SIM800_RX_PIN);
  
  external_state(ON);         //Turns relay ON

  multi_gas.begin(Wire,0x08); //Initialises Sensor Connections
  sensor_HM330X.init();
  scd30.initialize();  
  reset_sim();
  sim->apn(APN);              // Sets SIM APN
}

 
void loop() {
indicate_state(3);

int attempts =0;
  while(!sim->init(URL)) {    // Attemps initialisation 
    //Serial.println("LOADING SIM");
    delay(3000);
    if( (attempts++) > 5) break; // Abandons connection attempt to rely on SIM800 internals
  } 
  attempts =0;
  indicate_state(2);

  while(attempts<5){ //Attempts HTTP connection, i.e recieve OK from SIM800
    HTTPINIT = sim->http_init();
    if(HTTPINIT) break;
    delay(5000);
    attempts++;
  }
  indicate_state(2);
  
  int val = 0;
  val = analogRead(battPin); // Battery voltage via analog pin
  float batt_m = 4850 * float(val)/1024; //manually measured reference voltage 4850 mV and compensation for voltage drop across protection resistor

  sensor_HM330X.read_sensor_value(buf, 29) ; // Populate buffer with sensor data
  parse_result(buf,HM330X_values); 
  float result[3] = {0}; //0: C02, 1: Temperature, 2: Humidity(%)
  scd30.getCarbonDioxideConcentration(result);

  uint32_t val_NO2 = multi_gas.measure_NO2();  //Multi-Gas Sensor
  uint32_t val_C2H50H = multi_gas.measure_C2H5OH();
  uint32_t val_VOC = multi_gas.measure_VOC();
  uint32_t val_CO = multi_gas.measure_CO();

  //postData variable used for HTTP Request with data stored in Request URL

  postData= "?";
  postData+= "ID=" +DEVICE_ID;
  postData+= "&1="+String(batt_m);
  postData+= "&2=" +String(HM330X_values[1]);
  postData+= "&3="+String(HM330X_values[2]);
  postData+= "&4="+String(val_VOC);
  postData+= "&5=" +String(val_CO);
  postData+= "&6=" +String(result[0]);
  postData+= "&7="+String(result[1]);
  
  attempts =0;
  sim->http_send(postData);
  delay(5000);
  external_state(OFF); //Turns sensors and SIM800 OFF
  delay(WAIT);         //Time Sensors are OFF
  external_state(ON);  //Turns sensors and SIM800 ON
  reset_sim();
  delay(WARMUP);       //Warm up period for sensors

}
 