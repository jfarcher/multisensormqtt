#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "TSL2561.h"
#include "DHT.h"
#include "config.h"
#include <Adafruit_BMP085.h>

//Set Pins
int DHTPIN = 6;
int pirPin = 2;
int val;

#define DHTTYPE DHT11   // DHT 11 
DHT dht(DHTPIN, DHTTYPE);
TSL2561 tsl(TSL2561_ADDR_FLOAT); 
// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
char charSensorVal[20];  
Adafruit_BMP085 bmp;

EthernetClient ethClient;
PubSubClient client(MQTT_SERVER, 1883, 0, ethClient);
void getBMP()
{
 char bmpt_buffer[100];
 char bmpp_buffer[100];
 char bmpa_buffer[100];
 float bmptf = bmp.readTemperature(); 
 char* bmpt = dtostrf(bmptf,4,2,bmpt_buffer);
 float bmppf = bmp.readPressure(); 
 char* bmpp = dtostrf(bmppf,4,2,bmpp_buffer);
 float bmpaf = bmp.readAltitude();
 char* bmpa = dtostrf(bmpaf,4,2,bmpa_buffer);
 client.publish(MQTT_BMPT_TOPIC, bmpt);
 client.publish(MQTT_BMPP_TOPIC, bmpp);
 client.publish(MQTT_BMPA_TOPIC, bmpa);
 delay(5000);
}

void getLum()
{


  tsl.setGain(TSL2561_GAIN_16X);      // set 16x gain (for dim situations)
  tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);  // shortest integration time (bright light) 
  uint16_t x = tsl.getLuminosity(TSL2561_FULLSPECTRUM);

  char* lum;
  char message_buffer[100];
  lum = dtostrf(x,4,2,message_buffer);
  client.publish(MQTT_LUM_TOPIC, lum);
  delay(5000);

}
void getDHT(){
float t = dht.readTemperature();
float h = dht.readHumidity();
char temp[10], hum[10]; 
String ttt, hhh;
int tempC = t;
ttt = String(tempC);
ttt.toCharArray(temp, 10);

int humy = h; hhh = String(humy); hhh.toCharArray(hum, 10);
client.publish(MQTT_TEMP_TOPIC, temp);

client.publish(MQTT_HUM_TOPIC, hum);
}
void setup() {

attachInterrupt(0,pir,HIGH);
Serial.begin(9600);

  Ethernet.begin(mac);
  byte ip = Ethernet.localIP();
  
  if(client.connect(CLIENT_ID)){
  client.publish(MQTT_ROOM_TOPIC,"PIR Sensor UP");
  client.publish(MQTT_ROOM_TOPIC,"DHT Sensor Up");
  }
  if (tsl.begin()){
    client.publish(MQTT_ROOM_TOPIC,"LUM Sensor UP");
    
  }
    if (bmp.begin()){ 
      client.publish(MQTT_ROOM_TOPIC,"BMP Sensor UP");
      } 
  dht.begin();
      
   

}
void pir(){
    val = digitalRead(pirPin); //read state of the PIR
  
  if (val == LOW) {
    client.publish(MQTT_PIR_TOPIC, "0");
    
  }
  else {
    client.publish(MQTT_PIR_TOPIC,"1");
  
  }
}

void loop() {

     getDHT();
     getLum();
     getBMP();
  client.loop();
delay(500);
}
