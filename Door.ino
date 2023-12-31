//@farsmd
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <SPI.h>
#include <MFRC522.h>
#define LED_BUILTIN 2 
#define D 25 
#define RST_PIN 4
#define SS_PIN 5
byte readCard[4];
String My_ID = "********";  
String ID = "";
const char *ssid = "Door";
const char *password = "11223344";
WiFiServer server(80);
MFRC522 mfrc522(SS_PIN, RST_PIN);
void setup() {
  SPI.begin(); 
  mfrc522.PCD_Init();
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(D, OUTPUT);
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");
  if (!WiFi.softAP(ssid, password)) {
    log_e("Soft AP creation failed.");
    while(1);
  }
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();
  Serial.println("Server started");
}
void loop() {
  WiFiClient client = server.available();   
  while (getID()) {
  Serial.println(ID);
  }
  if (client) {                          
    Serial.println("New Client.");         
    String currentLine = "";               
    while (client.connected()) {          
      if (client.available()) {           
        char c = client.read();             
        Serial.write(c);                   
        if (c == '\n') {                  
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print("<h1 style='font-size: 6VW;'> Door <a href=\"/H\">Open</a> <br></h1>");
            client.println("<h1 style='font-size: 6VW;'> Door <a href=\"/L\">Close</a> <br></h1>");
            client.print("<h1 style='font-size: 6VW;'> Last ID : ");
            client.print(ID);
            client.print("</h1>");
            client.println();
            break;
          } else { 
            currentLine = "";
          }
        } else if (c != '\r') {  
          currentLine += c;    
        }
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(LED_BUILTIN, HIGH);
          digitalWrite(D, HIGH);             
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(LED_BUILTIN, LOW);
          digitalWrite(D, LOW);
        }
      }
    }
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
boolean getID() 
{
  if ( ! mfrc522.PICC_IsNewCardPresent()) { 
  return false;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) { 
  return false;
  }
  ID = "";
  for ( uint8_t i = 0; i < 4; i++) { 
  //readCard[i] = mfrc522.uid.uidByte[i];
  ID.concat(String(mfrc522.uid.uidByte[i], HEX)); 
  }
  ID.toUpperCase();
  mfrc522.PICC_HaltA(); 
  return true;
}
