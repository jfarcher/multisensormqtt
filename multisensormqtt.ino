#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "TSL2561.h"
#include "config.h"
int pirPin = 7;
int val;

TSL2561 tsl(TSL2561_ADDR_FLOAT); 
// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };



EthernetClient ethClient;
PubSubClient client(MQTT_SERVER, 1883, 0, ethClient);
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

void setup() {
  Ethernet.begin(mac);
  if(client.connect(CLIENT_ID)){
  client.publish(MQTT_PIR_TOPIC,"Sensor UP");
  }
   if (tsl.begin()){
     client.publish("LS1","Sensor UP");
  }

}

void loop() {
  val = digitalRead(pirPin); //read state of the PIR
  
  if (val == LOW) {
    client.publish(MQTT_PIR_TOPIC, "0");
    delay(1000);
  }
  else {
    client.publish(MQTT_PIR_TOPIC,"1");
  delay(3000);
  }
  
  
  client.loop();
   getLum();
}
