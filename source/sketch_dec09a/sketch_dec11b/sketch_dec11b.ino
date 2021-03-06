#include "DHT.h"


// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

#define DHTIN 2     // what pin we're connected to
#define DHTOUT 3

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

DHT dht(DHTIN,DHTOUT, DHTTYPE);

void setup() {
  delay(3000);
  Serial.begin(9600); 
  Serial.println("DHT11 test!");
 
  dht.begin();
}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit
  float f = dht.readTemperature(true);
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
    
//  String jsonValue = "{\"t\":\""; //       {"t":"
//  jsonValue = jsonValue + (int) t; //      {"t":"24
//  jsonValue = jsonValue + "\",\"h\":\""; //{"t":"24","h":"
//  jsonValue = jsonValue + (int) h;       //{"t":"24","h":"60  
//  jsonValue = jsonValue + "\"}";         //{"t":"24","h":"60"}
//  String pvcloudCommand = "node /home/root/pvcloud_api.js action='add_value' value='"+jsonValue+"' value_type='JSON_TH' value_label='DHT11_READING' captured_datetime='2015-03-09+21:00' >> log.txt";
//  Serial.println(pvcloudCommand);
//  
//  system ( pvcloudCommand.buffer );
  
  // Compute heat index
  // Must send in temp in Fahrenheit!
  float hi = dht.computeHeatIndex(f, h);
  Serial.print("Humidity: "); 
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hi);
  Serial.println(" *F");
}
