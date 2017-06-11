/**************************************************************************/
/*!
This is a demo for the Adafruit MCP9808 breakout
----> http://www.adafruit.com/products/1782
Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!
 for Feather install the required CP2104 USB driver to have the COM/Serial port appear properly


/Documents/Arduino/mcp9808topython/mcp9808topython.ino
*/
/**************************************************************************/

#include <Wire.h>
#include "Adafruit_MCP9808.h"
#include <ESP8266WiFi.h>

const char* ssid     = "Actiontec";
const char* password = "downstairsAction";
const char* host = "192.168.0.102";
const char* location_temp = "outside";
const int  sample_int = 20000;  // 600000 ms = 1 min
long huzid = 0L;

// Create the MCP9808 temperature sensor object
// Making global  
// WILL THIS GET INITIALISED in DEEP SLEEP ???
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();


void wifi_setup() {
 
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
}

void setup() {
  Serial.begin(115200);
  Serial.println("MCP9808 to python interface v3");
  wifi_setup();
    
  // Make sure the sensor is found, you can also pass in a different i2c
  // address with tempsensor.begin(0x19) for example
  if (!tempsensor.begin()) {
    Serial.println("Couldn't find MCP9808!");
    while (1);
  }
  
}


float get_temp_c() {

  //Serial.println("wake up MCP9808.... "); // wake up MSP9808 - power consumption ~200 mikro Ampere

  tempsensor.shutdown_wake(0);   // Don't remove this line! required before reading temp

  // Read temperature
  float c = tempsensor.readTempC();
  delay(250);
  //Serial.print("Temp in get_temp_c: "); Serial.print(c); Serial.println("*C\t");
  
  //Serial.println("Shutdown MCP9808.... ");
  tempsensor.shutdown_wake(1); // shutdown MSP9808 - power consumption ~0.1 mikro Ampere

   
  return c;
}


float get_bat_volts() {
  //#define VBATPIN A9
  int VBATPIN = 19;
   
  float measuredvbat = analogRead(A0);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
  Serial.print("VBat: " ); Serial.println(measuredvbat);
  return measuredvbat;
}


void webpush_temp_c(float temp_c, long huzid) {
    // Use WiFiClient class to create TCP connections
  WiFiClient client;

  Serial.print("webpush connecting to ");
  Serial.println(host);

  const int httpPort = 8000;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // We now create a URI for the request
  String url = String("/add_reading?location='")+String(location_temp)+String("'&temp_c=")+String(temp_c)+"&huzid="+String(huzid);
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(500);
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
}



void loop() {
  huzid += 1;
  float temp_c = get_temp_c();
  delay(250);
  float f = temp_c * 9.0 / 5.0 + 32.0;
  Serial.print("Temp: "); Serial.print(temp_c); Serial.print("*C\t"); 
  Serial.print(f); Serial.println("*F");
  float vbat = get_bat_volts();
  Serial.print(vbat); Serial.println("BatV");

  webpush_temp_c(temp_c, huzid);
  
  delay(sample_int); // 600000 ms = 1 min
}




