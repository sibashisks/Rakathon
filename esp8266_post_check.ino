// LIBRARY FILES TO INCLUDE

#include <ESP8266WiFi.h> // module to enable WiFi of ESP8266
#include <ESP8266HTTPClient.h> // module to send HTTP request from ESP8266
#include <ArduinoJson.h> // module to send JSON data from arduino
#include <SPI.h> // module to enable serial interface
#include <RFID.h> // module to enable MFRC522

// WIFI AND HTTP DATA

const char* ssid = "SKS"; // WiFi name
const char* password = "youareawesome"; // WiFi password
const char* host = "http://pulse-squad.herokuapp.com"; // host address 
String url = "/home/extra/dravail"; // extension address

// SET PINS

#define SS_PIN 2
#define RST_PIN 0
#define led LED_BUILTIN

RFID rfid(SS_PIN, RST_PIN); // input pins for RFID

String rfidCard;

void setup()
{
  Serial.begin(115200); 
  Serial.println("Starting the RFID Reader...");
  SPI.begin();
  rfid.init();
  pinMode(led, OUTPUT);
  delay(10); // connecting to a WiFi network 
  Serial.println(); 
  Serial.println();
  Serial.print("Connecting to "); 
  Serial.println(ssid); 
  WiFi.mode(WIFI_STA); // explicitly set the ESP8266 to be a WiFi-client 
  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) // if not connected display error message
  { 
    delay(500); 
    Serial.print("."); 
  } 
  Serial.println(""); 
  Serial.println("WiFi connected"); 
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());
  pinMode(A0,INPUT);
}

void loop()
{
  DynamicJsonDocument doc(2048); // object of JSON file
  if (rfid.isCard()) // if RFID card is detected
  {
    if (rfid.readCardSerial()) // read card serial number
    {
      rfidCard = String(rfid.serNum[0]) + String(rfid.serNum[1]) + String(rfid.serNum[2]) + String(rfid.serNum[3]); // store card number
      Serial.print("Connecting to ");
      Serial.println(host); // use WiFiClient class to create TCP connections 
      WiFiClient client;
      const int httpPort = 80;
      if (!client.connect(host, httpPort)) // send HTTP request only when RFID card is detected
      {
        Serial.println("connection failed"); 
        return; 
      }
      Serial.print("Requesting URL: "); 
      Serial.println(url); //Post Data 
      String address = host + url;
      // JSON data (key-value pairs)
      doc["doctor_id"] = rfidCard;
      doc["rfid_tag"] = rfidCard;
      // serialize JSON data
      String json;
      serializeJson(doc, json);
      // send POST request
      HTTPClient http; 
      http.begin(address); 
      http.addHeader("Content-Type", "application/json"); 
      auto httpCode = http.POST(json); // get return message
      // print HTTP code and message
      Serial.println(httpCode); // print HTTP return code 
      String payload = http.getString(); 
      Serial.println(payload); // print request response payload 
      http.end(); // close connection 
      Serial.println("Closing connection");
    }
    rfid.halt();
  }
}
