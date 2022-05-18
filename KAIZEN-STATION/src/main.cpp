#include <Arduino.h>
#include <MKRGSM.h>
#include <Seeed_HM330X.h>
#include <TCA9548A.h>
#include "sensirion_common.h"
#include "sgp30.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "ThingSpeak.h"
#include "DHT.h"
#include "Multichannel_Gas_GMXXX.h"


#define PINNUMBER     "0000"
#define GPRS_APN      "afrihost" 
#define GPRS_LOGIN    ""    
#define GPRS_PASSWORD "" 

#define BAUD_RATE 115200 

// Communication ===============================================
GSMClient client;
GPRS gprs;
GSM gsmAccess;
GSMScanner scanNet;

char server[] = "ie-gtfs.up.ac.za";
char path[] = "/data/post.php";
int port = 80; // port 80 is the default for HTTP
unsigned long myChannelNumber = 1698096;
const char * myWriteAPIKey = "4A9Q3S8W03GXJ51S";

String postData;

//I2C ===========================================================
  
  #include <Wire.h>
  TCA9548A<TwoWire> TCA;
  #define WIRE Wire
  LiquidCrystal_I2C lcd(0x27,16,4);

// SEEED_HM330X ============================================================
  #ifdef  ARDUINO_SAMD_VARIANT_COMPLIANCE
    #define SERIAL_OUTPUT SerialUSB
#else
    #define SERIAL_OUTPUT Serial
#endif

HM330X sensor_HM330X;
uint8_t buf[30];
uint16_t HM330X_values[8];

/*ERROR HANDLING CODE: 
  - Can remove later if serial output is going to be removed for space:
*/
  const char* str[] = {"sensor_HM330X num: ", "PM1.0 concentration(CF=1,Standard particulate matter,unit:ug/m3): ",
                     "PM2.5 concentration(CF=1,Standard particulate matter,unit:ug/m3): ",
                     "PM10 concentration(CF=1,Standard particulate matter,unit:ug/m3): ",
                     "PM1.0 concentration(Atmospheric environment,unit:ug/m3): ",
                     "PM2.5 concentration(Atmospheric environment,unit:ug/m3): ",
                     "PM10 concentration(Atmospheric environment,unit:ug/m3): ",
                    };

HM330XErrorCode print_result(const char* str, uint16_t value) {
    if (NULL == str) {
        return ERROR_PARAM;
    }
    SERIAL_OUTPUT.print(str);
    SERIAL_OUTPUT.println(value);
    return NO_ERROR;
}

/*parse buf with 29 uint8_t-data*/
HM330XErrorCode parse_result(uint8_t* data, uint16_t* out) {
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

HM330XErrorCode parse_result_value(uint8_t* data) {
    if (NULL == data) {
        return ERROR_PARAM;
    }
  
    uint8_t sum = 0;
    for (int i = 0; i < 28; i++) {
        sum += data[i];
    }
    if (sum != data[28]) {
        SERIAL_OUTPUT.println("wrong checkSum!!");
    }
   // SERIAL_OUTPUT.println("");
    return NO_ERROR;
}

//DHT20 ===========================================================
 DHT dht(DHT20); 

// SEN0219 =====================
  #define  sensorIn  0   // Sensor PWM interface
#define  interruptNumber  0   // interrupt number

unsigned long pwm_high_start_ticks=0, pwm_high_end_ticks=0;
float pwm_high_val=0, pwm_low_val=0;
volatile uint8_t flag=0;

void interrupt_falling();
void interrupt_rising()
{
  pwm_high_start_ticks = micros();    // store the current micros() value
  if(2 == flag){
    flag = 4;
    pwm_low_val = pwm_high_start_ticks - pwm_high_end_ticks;
  }else{
    flag = 1;
  }

  attachInterrupt(interruptNumber, interrupt_falling, FALLING);
}

void interrupt_falling()
{
  pwm_high_end_ticks = micros();    // store the current micros() value
  if(1 == flag){
    flag = 2;
    pwm_high_val = pwm_high_end_ticks - pwm_high_start_ticks;
  }

  attachInterrupt(interruptNumber, interrupt_rising, RISING);
}

//===========MULTI GAS
 
    GAS_GMXXX<TwoWire> multi_gas;
//

void setup() {
 
  SERIAL_OUTPUT .begin(BAUD_RATE);
  while (!SERIAL_OUTPUT ) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  TCA.begin(WIRE);
  TCA.openChannel(TCA_CHANNEL_0);   
  TCA.openChannel(TCA_CHANNEL_1);
  TCA.openChannel(TCA_CHANNEL_2);
  TCA.openChannel(TCA_CHANNEL_3);
  /* SENSOR INITIATION:

  */
 multi_gas.begin(Wire,0x08);
 dht.begin();
 SERIAL_OUTPUT.println("STARTING");
  if (sensor_HM330X.init()) {
        SERIAL_OUTPUT.println("HM330X init failed!!");  
  }
  SERIAL_OUTPUT.println("HM330X init ok");

pinMode(sensorIn, INPUT);
attachInterrupt(interruptNumber, interrupt_rising, RISING);

s16 err;
u16 scaled_ethanol_signal, scaled_h2_signal;

  while (sgp_probe() != STATUS_OK) {
        Serial.println("SGP failed");
    }
  Serial.println("SGP ok");
  /*Read H2 and Ethanol signal in the way of blocking*/
    err = sgp_measure_signals_blocking_read(&scaled_ethanol_signal,
                                            &scaled_h2_signal);
    if (err == STATUS_OK) {
        Serial.println("get ram signal!");
    } else {
        Serial.println("error reading signals");
    }
    err = sgp_iaq_init();



  /* GSM INITIATION:

  */

  SERIAL_OUTPUT .println("Starting Arduino web client.");
  bool connected = false; //CONNECTION STATE

  // After starting the modem with GSM.begin()
  // attach the shield to the GPRS network with the APN, login and password
  while (!connected) {
    if ((gsmAccess.begin(PINNUMBER) == GSM_READY) &&
        (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY)) {
      connected = true;
       Serial.println("Signal strength: " + scanNet.getSignalStrength());
       ThingSpeak.begin(client);

    } else {
      SERIAL_OUTPUT .println("Not connected");
      delay(1000);
    }
  }
  
  SERIAL_OUTPUT .println("connecting...");


}

void loop() {
  Serial.println("Signal strength: " + scanNet.getSignalStrength());
   // signal strength in 0-31 scale. 31 means power > 51dBm
  // BER is the Bit Error Rate. 0-7 scale. 99=not detectable
 
  /*
    Sensor Activation:
  */

  if (sensor_HM330X.read_sensor_value(buf, 29)) {
      SERIAL_OUTPUT.println("HM330X read result failed!!");
  }
  parse_result_value(buf);
  parse_result(buf,HM330X_values);
   float SENO_concentration =0;
  if(flag == 4){
    flag = 1;
    float pwm_high_val_ms = (pwm_high_val * 1000) / (pwm_low_val + pwm_high_val);

    if (pwm_high_val_ms < 0.01){
      Serial.println("Fault");
    }
    else if (pwm_high_val_ms < 80.00){
      Serial.println("preheat");
    }
    else if (pwm_high_val_ms < 998.00){
       SENO_concentration = (pwm_high_val_ms - 2) * 5;
      // Print pwm_high_val_ms
      Serial.print("pwm_high_val_ms:");
      Serial.print(pwm_high_val_ms);
      Serial.println("ms");
      //Print CO2 concentration
      Serial.print(SENO_concentration);
      Serial.println("ppm");
    }
    else{
      Serial.println("Beyond the maximum range ：398~4980ppm");
    }
    Serial.println();
  }


  s16 err = 0;
  u16 tvoc_ppb, co2_eq_ppm;
    err = sgp_measure_iaq_blocking_read(&tvoc_ppb, &co2_eq_ppm);
    if (err != STATUS_OK) {
        Serial.println("error reading IAQ values\n");
    }
   float temp_hum_val[2] = {0};
   if (!dht.readTempAndHumidity(temp_hum_val)) {
   }else {Serial.println("DHT20 FAIL");}

uint32_t val_NO2 = multi_gas.measure_NO2();
uint32_t val_C2H50H = multi_gas.measure_C2H5OH();
uint32_t val_VOC = multi_gas.measure_VOC();
uint32_t val_CO = multi_gas.measure_CO();
Serial.println("N02: " +String(val_NO2) + " C2H50H: " +String(val_C2H50H) + " VOC: " +String(val_VOC )+ " CO: " +String(val_CO));
  /*
    Data formating:
  */

  // String httpDATA ="&api_key=" + apiKeyValue;
  // for(int i=0; i<8;i++)
  //     httpDATA += "&value" +String(i)+'='+String(HM330X_values[i]);
  // httpDATA += "&tvoc_ppb=" +String(tvoc_ppb) +"&co2_eq_ppm=" + String(co2_eq_ppm)+"";
  String httpDATA ="&str=";
  String sign = "  ";
  for(int i=1; i<4;i++){
      ThingSpeak.setField(i, HM330X_values[i]);
  }
    ThingSpeak.setField(4,String(val_VOC)); //VOC
    ThingSpeak.setField(5,co2_eq_ppm);
  if(temp_hum_val[0]!=0) ThingSpeak.setField(6, temp_hum_val[0]);
   if(temp_hum_val[1]!=0)ThingSpeak.setField(7, temp_hum_val[1]);
  if(SENO_concentration!=0) ThingSpeak.setField(8,SENO_concentration);

  /*
    HTTP Activation:
  */
   
    int res_connect = client.connect(server, 80);
    if (res_connect) {
      Serial.println("SERVER: OK");
      int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
      if(x == 200){
        Serial.println("Update successful.");
      }
      else{
        Serial.println(" HTTP error: " + String(x));
      }
      
      // client.print("POST ");
      // client.print(path);
      // client.println(" HTTP/1.1");
      // client.println("Host: ie-gtfs.up.ac.za");
      // client.println("Connection: close");
      // client.println("Content-Type: application/x-www-form-urlencoded");
      // client.print("Content-Length: ");
      // client.println(httpDATA.length());
      // client.println();
      // client.print(httpDATA);
     
      //Serial.println(httpDATA);
      
      //if (client.connected()) client.stop();
      //  String response = "";
      //   long time = millis();
      //   while(millis() - time < 10000){
      //     while (client.available() > 0) {
      //       char he = client.read();
      //       response += he; 
      //     } 
      //   }
      //   Serial.println("Printing response: ");
      //   Serial.println(response);
      }else {
      // if you didn't get a connection to the server
      Serial.println("SERVER: ERROR");
    }
  delay(10000);

}