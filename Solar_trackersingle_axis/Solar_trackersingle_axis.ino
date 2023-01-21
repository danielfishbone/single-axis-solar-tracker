#define BLYNK_TEMPLATE_ID "TMPLdbVY0tMW"
#define BLYNK_DEVICE_NAME "Solar Tracker"
#define BLYNK_AUTH_TOKEN "esV3kznUNZh-tHtFDlVo23yff6hpVhFg"
#define BLYNK_PRINT Serial
#define ESP8266_BAUD 9600 // Your ESP8266 baud rate:
#define DHTPIN 2
#define DHTTYPE    DHT11     // DHT 11

#include<Servo.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <SoftwareSerial.h>
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>


// Your credentials. Replace with yours 
char ssid[] = "solartracker";  // wifi name 
char pass[] = "solartracker"; //  Set password to "" for open networks.
char auth[] = BLYNK_AUTH_TOKEN;



int servoPosition=78;
int margin = 10;
int minPosition = 30;
int maxPosition = 130;
uint32_t delayMS;
uint32_t uploadTime=60000;
uint32_t timeStamp=0;
float temp;
float hum;

SoftwareSerial EspSerial(7, 8); // RX, TX 
ESP8266 wifi(&EspSerial);
Servo servo;
DHT_Unified dht(DHTPIN, DHTTYPE);

void myTimerEvent(){
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    temp = -1;
  }
  else {
    temp = event.temperature;
  }
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
   hum = -1;
  }
  else {
   hum = event.relative_humidity;
  }
  Blynk.virtualWrite(V2,servoPosition); // update the app on its current direction
  Blynk.virtualWrite(V1,hum);
  Blynk.virtualWrite(V0,temp);
  }


void setup() {
  // put your setup code here, to run once:



Serial.begin(115200);
servo.attach(5);
dht.begin();
sensor_t sensor;
 dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;
  if(uploadTime < delayMS)uploadTime = delayMS;

  EspSerial.begin(ESP8266_BAUD); //Serial between ESp and Arduino
  Blynk.begin(auth, wifi, ssid, pass);
  
  }

void loop() {
Blynk.run();
int left =  analogRead(A1);
int right =  analogRead(A0);

if(left >= right+margin){
servoPosition++;
if(servoPosition > maxPosition)servoPosition=maxPosition;
 }
 if( right >= left+margin){
servoPosition--;
if(servoPosition < minPosition)servoPosition=minPosition;
 }
 if((abs(left - right) < margin) || servoPosition == maxPosition || servoPosition == minPosition)
 {
  if (millis()-timeStamp > uploadTime){
    myTimerEvent();
    timeStamp = millis();
   }
  }
 servo.write(servoPosition);
 Serial.print(temp);
 Serial.print(" | ");
 Serial.println(hum);
delay(10);
}
