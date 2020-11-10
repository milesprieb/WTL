#include <Arduino.h> //Should be removed if using the arduino IDE
//const char* ssid = "Redmi"; //Enter SSID
//const char* password = "9514ce588c78"; //Enter Password

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

IPAddress    apIP(54, 54, 54, 54);  // Defining a static IP address: local & gateway
                                    // Default IP in AP mode is 192.168.4.1

/* This are the WiFi access point settings. Update them to your likin */
const char* APssid = "ESP8266ESP12";
const char* APpassword = "0123456789";


const char* ssid = "809xFI"; //Enter SSID
const char* password = "NoPlaceLikeHome809!#!"; //Enter Password

// Define a web server at port 80 for HTTP
ESP8266WebServer server(80);


const int garment = LED_BUILTIN; // builtin CNL represents garment off/on button
bool cnlState = false;

//Set the pins on the ESP-12 for each channel
//Just change the pin here if you use different pins and the code should work.
uint8_t CNL1pin = D4; //Garment Channel
bool CNL1status = LOW;

uint8_t CNL2pin = D5;  //Torso High
bool CNL2status = LOW;

uint8_t CNL3pin = D6;  //Torso Low
bool CNL3status = LOW;


String SendHTML(uint8_t cnl1stat,uint8_t cnl2stat,uint8_t cnl3stat){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>CNL Control</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #1abc9c;}\n";
  ptr +=".button-on:active {background-color: #16a085;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>Voice Assistant Garment Web Server</h1>\n";
  ptr +="<h3>Using Access Point(AP) Mode</h3>\n";
  
  if(cnl1stat)
  {ptr +="<p>Garment Status: ON</p><a class=\"button button-off\" href=\"/cnl1off\">OFF</a>\n";}
  else
  {ptr +="<p>Garment Status: OFF</p><a class=\"button button-on\" href=\"/cnl1on\">ON</a>\n";}

  if(cnl2stat)
  {ptr +="<p>Torso Status: High</p><a class=\"button button-off\" href=\"/cnl2off\">OFF</a>\n";}
  else
  {ptr +="<p>Torso Status: OFF</p><a class=\"button button-on\" href=\"/cnl2on\">ON</a>\n";}

  if(cnl3stat)
  {ptr +="<p>Torso Status: Low</p><a class=\"button button-off\" href=\"/cnl3off\">OFF</a>\n";}
  else
  {ptr +="<p>Torso Status: OFF</p><a class=\"button button-on\" href=\"/cnl3on\">ON</a>\n";}

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}

void handle_OnConnect() {
  digitalWrite ( LED_BUILTIN, 1 );
  CNL1status = LOW;
  CNL2status = LOW;
  CNL3status = LOW;
  Serial.println("tcnl1 Status: OFF | tcnl2 Status: OFF | tcnl3 Status: OFF | shoulder Status: OFF");
  server.send ( 200, "text/html", SendHTML(CNL1status,CNL2status,CNL3status) );
  
}

void handle_cnl1on() {
  CNL1status = HIGH;
  Serial.println("Garment Status: ON");
  server.send(200, "text/html", SendHTML(true,CNL2status,CNL3status)); 
}

void handle_cnl1off() {
  CNL1status = LOW;
  Serial.println("Garment Status: OFF");
  server.send(200, "text/html", SendHTML(false,CNL2status,CNL3status)); 
}

void handle_cnl2on() {
  CNL2status = HIGH;
  Serial.println("Torso Status: High");
  server.send(200, "text/html", SendHTML(CNL1status,true,CNL3status)); 
}

void handle_cnl2off() {
  CNL2status = LOW;
  Serial.println("Torso Status: OFF");
  server.send(200, "text/html", SendHTML(CNL1status,false,CNL3status)); 
}

void handle_cnl3on() {
  CNL3status = HIGH;
  Serial.println("Torso Status: Low");
  server.send(200, "text/html", SendHTML(CNL1status,CNL2status,true)); 
}

void handle_cnl3off() {
  CNL3status = LOW;
  Serial.println("Torso Status: OFF");
  server.send(200, "text/html", SendHTML(CNL1status,CNL2status,false)); 
}

void handleNotFound() {
  digitalWrite ( LED_BUILTIN, 0 );
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
  digitalWrite ( LED_BUILTIN, 1 ); //turn the built in CNL on pin DO of NodeMCU off
}

int init_wifi() {
  int retries = 0;

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while ((WiFi.status() != WL_CONNECTED) && (retries < 10)){
    retries++;
    delay(500);
    Serial.print(".");
  }

  return WiFi.status(); // return the WiFi connection status
}

void setup() {
  pinMode ( garment, OUTPUT );
  digitalWrite ( garment, 0 );

  pinMode(CNL1pin, OUTPUT);
  pinMode(CNL2pin, OUTPUT);
  pinMode(CNL3pin, OUTPUT);
  
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  //set-up the custom IP address
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  
  Serial.println("Configuring access point...");
  //You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(APssid, APpassword);
  
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  if (init_wifi() == WL_CONNECTED) {
    Serial.print("Connetted to ");
    Serial.print(ssid);
    Serial.print("--- IP: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.print("Error connecting to: ");
    Serial.println(ssid);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(APssid, APpassword);
    Serial.print("AP IP address: ");
    Serial.println(myIP);
  }

 
  server.on ( "/", handle_OnConnect);
  
  server.on("/cnl1on", handle_cnl1on);
  server.on("/cnl1off", handle_cnl1off);
  server.on("/cnl2on", handle_cnl2on);
  server.on("/cnl2off", handle_cnl2off);
  server.on("/cnl3on", handle_cnl3on);
  server.on("/cnl3off", handle_cnl3off);
  
  server.onNotFound ( handleNotFound );
  
  server.begin();
  Serial.println("HTTP server started");

   // Print the IP address to screen
     
  Serial.println("Local IP Address");
  Serial.println(WiFi.localIP());
  Serial.println("Soft Access Point IP Address");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  server.handleClient();
  if(CNL1status)
  {
    if(CNL2status)
    {digitalWrite(CNL2pin, HIGH);}
    else
    {digitalWrite(CNL2pin, LOW);}
    
    if(CNL3status)
    {digitalWrite(CNL3pin, HIGH);}
    else
    {digitalWrite(CNL3pin, LOW);}  
  }
  
  else
  {digitalWrite(CNL2pin, LOW);
   digitalWrite(CNL3pin, LOW);}
}

