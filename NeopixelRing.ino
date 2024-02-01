#include <Adafruit_NeoPixel.h>
#include <time.h>
#include <WiFi.h>

//einstellungen anfang

#define Ringdrehung 30 //Anzahl der LEDs um die der Ring gedreht ist
#define LedHelligkeit 50 //Helligkeit der bunten LEDs
#define LedWeissHell 10 //Helligkeit der viertelstunden LEDS
#define LewWeissDunkel 1 //Helligkeit der 5min LEDs
#define NeopixelPinNummer 23 //Pin an dem der Ring angeschlossen ist
#define SSID "" //WLAN Name
#define Password "" //WLAN Passwort
#define ZeitzoneInSekunden 3600 //Unterschied der aktuellen Zeitzone im Vergleich zu UTC in Sekunden
#define SommerzeitInSekunden 3600 //Unterschied zwischen Sommerzeit und Winterzeit in Sekunden
#define Zeitserver "pool.ntp.org" //NTP Server von dem die Zeit geholt wird

//einstellungen ende

struct tm timeinfo; //zeitinfo von ntp server

Adafruit_NeoPixel ring(60, NeopixelPinNummer, NEO_GRBW + NEO_KHZ800); //steuerdaten für neopixel library

int rotate(int value){
  value = value + Ringdrehung;
  while (value > 59){value = value -60;}
  return value;
}

void getntptime(){
  WiFi.begin(SSID, Password); //start wifi communication
  while (WiFi.status() != WL_CONNECTED) {error();}
  configTime(ZeitzoneInSekunden, SommerzeitInSekunden, Zeitserver); //ntp zeit abrufen
  getLocalTime(&timeinfo);
  WiFi.disconnect(true);
}

void error(){
  for (int i = 0; i < 60; i++){
    ring.setPixelColor(i, LedHelligkeit, 0, 0, 0);
  }
  ring.show();
  delay(50);
  ring.clear();
  ring.show();
  delay(50);
}

void ringclock(){
  int led [60] [4] = {0};
  led [rotate(timeinfo.tm_sec)] [0] = LedHelligkeit; //sekundenzeiger
  led [rotate(timeinfo.tm_min)] [1] = LedHelligkeit; //minutenzeiger
  led [rotate((timeinfo.tm_hour*5) + (timeinfo.tm_min/12))] [2] = LedHelligkeit; //stundenzeiger.
  for (int i = 0; i < 60; i = i +5){led [rotate(i)] [3] = LewWeissDunkel;}
  for (int i = 0; i < 60; i = i +15){led [rotate(i)] [3] = LedWeissHell;}
  
  for (int i = 0; i < 60; i++){
    ring.setPixelColor(i, led [i] [0], led [i] [1], led [i] [2], led [i] [3]);
  }
  ring.show();
}

void setup() {
  ring.begin(); //start neopixel protocol
  ring.clear();
  ring.show();
  Serial.begin(115200); //start serial console
  getntptime();
}

void loop(){
  getLocalTime(&timeinfo);
  if (timeinfo.tm_year < 100 || (timeinfo.tm_wday == 0 && timeinfo.tm_hour == 0 && timeinfo.tm_min == 0 && timeinfo.tm_sec == 0)){getntptime();} //jeden sonntag um mitternacht die zeit neu laden
  ringclock();
}