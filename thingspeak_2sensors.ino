
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>
#include <ThingSpeak.h>
#include <WiFiUdp.h>
#include <NTPClient.h>               
#include <TimeLib.h>  

#define DHTTYPE DHT11

DHT HT1(D4, DHTTYPE);
DHT HT2(D2, DHTTYPE);

HTTPClient http;
WiFiUDP ntpUDP;

NTPClient timeClient(ntpUDP, "europe.pool.ntp.org",0, 60000);

char Time[ ] = "TIME:00:00:00";
char Date[ ] = "DATE:00/00/2000";
byte last_second, second_, minute_, hour_, day_, month_;
int year_;


float temp1;
float temp2;

WiFiClient client;

#ifndef STASSID
#define STASSID "" //network name
#define STAPSK  "" //network password
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

int status = WL_IDLE_STATUS;

unsigned long myChannelNumber = 1104627;
const char * myWriteAPIKey = "P5VRTKJTQTPV81KN";

void setup() {
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Waiting to connect...");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  timeClient.begin();
  ThingSpeak.begin(client);
  HT1.begin();
  HT2.begin();
  delay(1000);
}

void loop() {
  
  temp1 = HT1.readTemperature();
  temp2 = HT2.readTemperature();
  Serial.println("measure");

  ThingSpeak.setField(1,temp1);
  ThingSpeak.setField(2,temp2);
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey); 
  Serial.println("sent");

   timeClient.update();
    unsigned long unix_epoch = timeClient.getEpochTime();    // Get Unix epoch time from the NTP server

    second_ = second(unix_epoch);
    if (last_second != second_) {
      minute_ = minute(unix_epoch);
      hour_   = hour(unix_epoch);
      day_    = day(unix_epoch);
      month_  = month(unix_epoch);
      year_   = year(unix_epoch);
  
   
  
      Time[12] = second_ % 10 + 48;
      Time[11] = second_ / 10 + 48;
      Time[9]  = minute_ % 10 + 48;
      Time[8]  = minute_ / 10 + 48;
      Time[6]  = hour_   % 10 + 48;
      Time[5]  = hour_   / 10 + 48;
  
   
  
      Date[5]  = day_   / 10 + 48;
      Date[6]  = day_   % 10 + 48;
      Date[8]  = month_  / 10 + 48;
      Date[9]  = month_  % 10 + 48;
      Date[13] = (year_   / 10) % 10 + 48;
      Date[14] = year_   % 10 % 10 + 48;
  
      Serial.println(Time);
      Serial.println(Date);
  
      last_second = second_;
    }
  
  Serial.print("Temp1: ");
  Serial.println(temp1);
  Serial.print("Temp2: ");
  Serial.println(temp2);
  
  // Wait a few seconds between measurements.
  delay(60000);
 
}
