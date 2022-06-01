#include "Adafruit_FONA.h"
#include "LowPower.h"
#include <SoftwareSerial.h>
#define FONA_RX 11
#define FONA_TX 10
#define FONA_RST 4
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;
const int MOSFET = 13;
const int WakeUp = 2;
char replybuffer[255];
Adafruit_FONA_3G fona = Adafruit_FONA_3G(FONA_RST);
float latitude, longitude, speed_kph, heading, speed_mph, altitude;
unsigned long currentMillis;
unsigned long previousMillis = 0;
unsigned long timer;
unsigned long timer2 = 0;
boolean gps_success;


void setup() {
  Serial.begin(115200);
  pinMode(WakeUp, INPUT);
  pinMode(MOSFET, OUTPUT);
  attachInterrupt(0, TimerReset, RISING);
  digitalWrite(MOSFET, LOW);
  delay(10000);
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
}

void loop() {
  while ( timer < 4200000){
    if ( timer2 == 0 ){
      GetGPS();
      WriteSMS();
    }
    currentMillis = millis();
    timer = timer + (currentMillis - previousMillis);
    timer2 = timer2 + (currentMillis - previousMillis);
    previousMillis = currentMillis;
    if ( timer2 > 1800000){
      GetGPS();
      WriteSMS();
      timer2 = 1;
    }
  }
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
}

void WriteSMS () {
  fonaSS.print("AT+CMGF=1\r");
  delay(1000);
  fonaSS.print("AT+CMGS=\"+491521234567\"\r");
  delay(1000);
  fonaSS.println(latitude, 6);
  fonaSS.println(longitude, 6);
  fonaSS.print("\r");
  delay(1000);
  fonaSS.println((char)26);
  fonaSS.println();
  delay(10000);
  digitalWrite(MOSFET, LOW);
}

void GetGPS() {
  gps_success = 0;
  digitalWrite(MOSFET, HIGH);
  fonaSerial->begin(4800);
  if (! fona.begin(*fonaSerial)) {
    while (1);
  }
  fona.enableGPS(true);
  while(!gps_success){
  delay(2000);
  gps_success = fona.getGPS(&latitude, &longitude, &speed_kph, &heading, &altitude);
  if (gps_success) {
    Serial.print("GPS lat:");
    Serial.println(latitude, 6);
    Serial.print("GPS long:");
    Serial.println(longitude, 6);
  }}
}

void TimerReset() {
  timer = 0;
}

