
#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "Seeed_BME280.h"
#include <Wire.h>

BME280 bme280;
char auth[] = "xXxXxXx";

WiFiClient client;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "xXxXxXx";
char pass[] = "xXxXxXx";
const char* server = "api.thingspeak.com";
const char* api_key = "xXxXxXx";

void setup()
{
  Serial.begin(74880);
  Serial.println("Initialising....");
  Blynk.begin(auth, ssid, pass);
  if (!bme280.init()) {
    Serial.println("Device error!");
  }
  WiFi.begin(ssid, pass);
}

void postData(float temperature, float humidity, float pressure) {
  // Send data to ThingSpeak
  if (client.connect(server, 80)) {
    Serial.println("Connect to ThingSpeak - OK");

    String dataToThingSpeak = "";
    dataToThingSpeak += "GET /update?api_key=";
    dataToThingSpeak += api_key;

    dataToThingSpeak += "&field1=";
    dataToThingSpeak += String(temperature);

    dataToThingSpeak += "&field2=";
    dataToThingSpeak += String(humidity);

    dataToThingSpeak += "&field3=";
    dataToThingSpeak += String(pressure);

    dataToThingSpeak += " HTTP/1.1\r\nHost: a.c.d\r\nConnection: close\r\n\r\n";
    dataToThingSpeak += "";
    client.print(dataToThingSpeak);

    int timeout = millis() + 5000;
    while (client.available() == 0) {
      if (timeout - millis() < 0) {
        Serial.println("Error: Client Timeout!");
        client.stop();
        return;
      }
    }
  }
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
}

void loop()
{
  Blynk.run();
  delay(100);
  //get and print temperatures
  float temp = bme280.getTemperature();
  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.println("C");//The unit for  Celsius because original arduino don't support speical symbols
  Blynk.virtualWrite(0, temp); // virtual pin 0
  Blynk.virtualWrite(4, temp); // virtual pin 4

  //get and print atmospheric pressure data
  float pressure = bme280.getPressure(); // pressure in Pa
  float p = pressure / 100.0 ; // pressure in hPa
  Serial.print("Pressure: ");
  Serial.print(p);
  Serial.println("hPa");
  Blynk.virtualWrite(1, p); // virtual pin 1

  //get and print altitude data
  float altitude = bme280.calcAltitude(pressure);
  Serial.print("Altitude: ");
  Serial.print(altitude);
  Serial.println("m");
  Blynk.virtualWrite(2, altitude); // virtual pin 2

  //get and print humidity data
  float humidity = bme280.getHumidity();
  Serial.println("Humidity: " + String(humidity) + "%");
  Serial.println();
  Blynk.virtualWrite(3, humidity); // virtual pin 3

  postData(temp, humidity, pressure);
  // deepSleep time is defined in microseconds. Multiply seconds by 1e6
  // MINUTES x SECONDS x MICROSECONDS
  ESP.deepSleep(1 * 60 * 1000000); // 

}
