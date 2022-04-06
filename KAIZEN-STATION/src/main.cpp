#include <Arduino.h>
#include <MKRGSM.h>
#include <Seeed_HM330X.h>
#include <TCA9548A.h>
#include "sensirion_common.h"
#include "sgp30.h"
#include <Wire.h>


#define PINNUMBER     "0000"
#define GPRS_APN      "afrihost" 
#define GPRS_LOGIN    ""    
#define GPRS_PASSWORD "" 

#define BAUD_RATE 115200 

// Communication ===============================================
GSMClient client;
GPRS gprs;
GSM gsmAccess;

char server[] = "ie-gtfs.up.ac.za";
char path[] = "/data/post.php";
int port = 80; // port 80 is the default for HTTP
String apiKeyValue = "000ZZ4ND";

String postData;
String postVariable = "test=";

//I2C ===========================================================
  
  #include <Wire.h>
  TCA9548A<TwoWire> TCA;
  #define WIRE Wire

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
      //  print_result(str[i - 1], value);
      out[i-1] = value;

    }

    return NO_ERROR;
}

HM330XErrorCode parse_result_value(uint8_t* data) {
    if (NULL == data) {
        return ERROR_PARAM;
    }
    // for (int i = 0; i < 28; i++) {
    //     SERIAL_OUTPUT.print(data[i], HEX);
    //     SERIAL_OUTPUT.print("  ");
    //     if ((0 == (i) % 5) || (0 == i)) {
    //         SERIAL_OUTPUT.println("");
    //     }
    // }
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

//==========

//

void setup() {

  SERIAL_OUTPUT .begin(BAUD_RATE);
  while (!SERIAL_OUTPUT ) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  TCA.begin(WIRE);
  TCA.openChannel(TCA_CHANNEL_0);   
  TCA.openChannel(TCA_CHANNEL_1);
  /* SENSOR INITIATION:

  */
 SERIAL_OUTPUT.println("STARTING");

  if (sensor_HM330X.init()) {
        SERIAL_OUTPUT.println("HM330X init failed!!");  
  }
  SERIAL_OUTPUT.println("HM330X init ok");

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
    } else {
      SERIAL_OUTPUT .println("Not connected");
      delay(1000);
    }
  }

  SERIAL_OUTPUT .println("connecting...");
}

void loop() {
  

  /*
    Sensor Activation:
  */

  if (sensor_HM330X.read_sensor_value(buf, 29)) {
      SERIAL_OUTPUT.println("HM330X read result failed!!");
  }
  parse_result_value(buf);
  parse_result(buf,HM330X_values);
  
  delay(5000);


  s16 err = 0;
  u16 tvoc_ppb, co2_eq_ppm;
    err = sgp_measure_iaq_blocking_read(&tvoc_ppb, &co2_eq_ppm);
    if (err == STATUS_OK) {
        // Serial.print("tVOC  Concentration:");
        // Serial.print(tvoc_ppb);
        // Serial.println("ppb");

        // Serial.print("CO2eq Concentration:");
        // Serial.print(co2_eq_ppm);
        // Serial.println("ppm");
    } else {
        Serial.println("error reading IAQ values\n");
    }
  /*
    Data formating:
  */
  // String httpDATA ="api_key=" + apiKeyValue;
  // for(int i=0; i<8;i++)
  //     httpDATA += "&value" +String(i)+'='+String(HM330X_values[i]);
  // httpDATA += "&tvoc_ppb=" +String(tvoc_ppb) +"&co2_eq_ppm=" + String(co2_eq_ppm)+"";
  String httpDATA ="&str=";
  String sign = " : ";
  for(int i=0; i<8;i++){
      httpDATA.concat(i);
      httpDATA.concat(sign);
      httpDATA+=HM330X_values[i];
      httpDATA+=" ";
  }
    httpDATA += "tvoc_ppb-->" +tvoc_ppb +String("co2_eq_ppm-->") + co2_eq_ppm+"";
  /*
    HTTP Activation:
  */
    int res_connect = client.connect(server, 80);
    if (res_connect) {
      Serial.println("SERVER: OK");

      client.print("POST ");
      client.print(path);
      client.println(" HTTP/1.1");
      client.println("Host: ie-gtfs.up.ac.za");
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.print("Content-Length: ");
      client.println(httpDATA.length());
      client.println();
      client.print(httpDATA);
      Serial.println(httpDATA);
      //if (client.connected()) client.stop();
      String response = "";
        long time = millis();
        while(millis() - time < 10000){
          while (client.available() > 0) {
            char he = client.read();
            response += he; 
          } 
        }
        Serial.println("Printing response: ");
        Serial.println(response);
      }else {
      // if you didn't get a connection to the server
      Serial.println("SERVER: ERROR");
    }
  delay(10000);

}