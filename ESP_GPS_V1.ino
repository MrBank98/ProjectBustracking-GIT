#include <TinyGPS++.h>                                  // Tiny GPS Plus Library
#include <SoftwareSerial.h>                             // Software Serial Library so we can use other Pins for communication with the GPS module
#include <FirebaseArduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#include <time.h>


#define FIREBASE_io "testarduino-01.firebaseio.com"                         // Select > Database 
#define FIREBASE_dataSecret "ycHsMBgsu5BttUExVQlqHXsIDBJ0IspeX04xzOrJ"      // Project Overview > Project  Settings > SERVICE ACCOUNTS > Database Secrets 


unsigned int localPort = 2390;
static const int RXPin = 5, TXPin = 4;              // Ublox 6m GPS module to pins 12 and 13
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;                                      // Create an Instance of the TinyGPS++ object called gps
SoftwareSerial ss(RXPin, TXPin);                      // The serial connection to the GPS device

boolean connectioWasAlive = true;

int Sat = 0;
int Minute = 0;
int Minuted = 0;
int Hour = 0;
int Day = 0;
int DayS = 0;
int A = 1;
int B = 0;
double LAT = 0;
double LNG = 0;

// Config time
int timezone = 7;       // Zone +7 for Thailand
char ntp_server1[20] = "time.navy.mi.th";
char ntp_server2[20] = "time.nist.gov";
char ntp_server3[20] = "time.uni.net.th";
int dst = 0;
String d, m, y, h, t;

unsigned long previousMillis = 0;
const long interval = 100;

void setup() {

//  Serial.end();
  Serial.begin(115200);
  delay(1);

  ss.begin(GPSBaud);                                    // Set Software Serial Comm Speed to 9600

  wifiMulti.addAP("Jirawat98", "0891411127");
  

  while (wifiMulti.run() != WL_CONNECTED) {
//    Serial.println("WiFi not connected!");
    delay(1000);
  }
//  Serial.println("WiFi connected!");
  delay(500);

  /*---- Config Firebase ----*/
//  Firebase.begin("example.firebaseio.com", "token_or_secret");
  Firebase.begin(FIREBASE_io, FIREBASE_dataSecret);

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
 
  Firebase.set("GPS", root);
  
  /*---- Config Time ----*/
  configTime(timezone * 3600, dst, ntp_server1, ntp_server2, ntp_server3);
//  Serial.println("Waiting for time");

}

void loop() {
    
    
  if (wifiMulti.run() != WL_CONNECTED) {
//    Serial.println("WiFi not connected!");
    delay(1000);
  } else {}

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) 
  {
    previousMillis = currentMillis;
    LAT = gps.location.lat();
    LNG = gps.location.lng();

    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["Latitude"] = LocLat();
    root["Longitude"] = LocLng();
    root["Address"] = Address();
     
//    A = (float)Firebase.get("Amount/A");
    /*if (A > 0) {
      A++;
      B = !B;

     StaticJsonBuffer<200> jsonBuffer;
      JsonObject& rootC = jsonBuffer.createObject();
      rootC["A"] = A;
      rootC["B"] = B;
      Firebase.set("Amount", rootC);
    } else {}*/
    delay(100);

    /*------------------- Local_Time.SET to Firebase -------------------*/
    NowString();

    if (1) {

      Firebase.set("GPSTIME/hour", Hour);
      Firebase.set("GPSTIME/minute", Minute);
      Firebase.set("GPSDAY/year", y.toInt());
      Firebase.set("GPSDAY/month", m.toInt());
      Firebase.set("GPSDAY/day", d.toInt());

      Firebase.set("GPS current location", root);

      
    } else {}
    yield();

 
    smartDelay(500);    // Run Procedure smartDelay

    Minuted = Minute;
    DayS = Day;
  }
}

String LocLat() {
  String LocLat = "";
  LocLat += String(LAT, 6);

  return LocLat;
}

String LocLng() {
  String LocLng = "";
  LocLng += String(LNG, 6);

  return LocLng;
}

String Address() {
  String Address = "BUSONE";
  
  return Address;
}

String NowString() {
  time_t now = time(nullptr);
  struct tm* newtime = localtime(&now);

  String tmpNow = "";
  tmpNow += String(newtime->tm_year + 1900);
  tmpNow += "-";
  tmpNow += String(newtime->tm_mon + 1);
  tmpNow += "-";
  tmpNow += String(newtime->tm_mday);
  tmpNow += " ";
  tmpNow += String(newtime->tm_hour);
  tmpNow += ":";
  tmpNow += String(newtime->tm_min);
  tmpNow += ":";
  tmpNow += String(newtime->tm_sec);

  y = String(newtime->tm_year + 1900);
  //  Serial.print(y);
  m = String(newtime->tm_mon + 1);
  //  Serial.print(m);
  d = String(newtime->tm_mday);
  //  Serial.print(d);
  Hour = String(newtime->tm_hour).toInt();
  //  Serial.print(Hour);
  Minute = String(newtime->tm_min).toInt();
  //  Serial.println(Minute);
//  Serial.println(tmpNow);
  return tmpNow;

}

static void smartDelay(unsigned long ms) {               // This custom version of delay() ensures that the gps object is being "fed".
  unsigned long start = millis();
  do {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}


  
