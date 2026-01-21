/// 8 relays module controller using websocket
/// 3/8/2022
//////
#include <Arduino.h>
#include "WiFi.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include <Arduino_JSON.h>
#include <AsyncElegantOTA.h>
#include <EEPROM.h>
AsyncWebServer OTAserver(88);
String message = "";
RTC_NOINIT_ATTR int r1 = 0;
RTC_NOINIT_ATTR int r2 = 0;
RTC_NOINIT_ATTR int r3 = 0;
RTC_NOINIT_ATTR int r4 = 0;
RTC_NOINIT_ATTR int r5 = 0;
RTC_NOINIT_ATTR int r6 = 0;
RTC_NOINIT_ATTR int r7 = 0;
RTC_NOINIT_ATTR int r8 = 0;
String relayValue1 = "0";
String relayValue2 = "0";
String relayValue3 = "0";
String relayValue4 = "0";
String relayValue5 = "0";
String relayValue6 = "0";
String relayValue7 = "0";
String relayValue8 = "0";


// {21, 19, 32, 18, 5,26,27,4};
#define relay1 13
#define relay2 12
#define relay3 14
#define relay4 27
#define relay5 26
#define relay6 25
#define relay7 33
#define relay8 32
#define NUM_RELAYS 8
#define RELAY_NO true
int relayGPIOs[NUM_RELAYS] = {13, 12, 14, 27, 26, 25, 33, 32};
byte x[NUM_RELAYS];
bool savelaststate = true;
// Json Variable to Hold Relays Values
JSONVar relaysValues;
JSONVar monday;

// Replace with your network credentials
const char *ssid = "Wood House 3";
const char *password = "jerd1977";



// const char *ssid = "Om39@!!";
// const char *password = "rahafsara@@$$321";

// http bridg3 192.168.7.92
IPAddress local_IP(7, 7, 7, 24);
IPAddress gateway(7, 7, 7, 246);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);   // optiona7
IPAddress secondaryDNS(8, 8, 4, 4); // optional

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
// Create a WebSocket object

AsyncWebSocket ws("/ws");


void savevalues()
{
  r1 = relayValue1.toInt();
  r2 = relayValue2.toInt();
  r3 = relayValue3.toInt();
  r4 = relayValue4.toInt();
  r5 = relayValue5.toInt();
  r6 = relayValue6.toInt();
  r7 = relayValue7.toInt();
  r8 = relayValue8.toInt();
}

void resetvalues()
{
  r1 = 0;
  r2 = 0;
  r3 = 0;
  r4 = 0;
  r5 = 0;
  r6 = 0;
  r7 = 0;
  r8 = 0;
  savevalues();
}


/*
void reset_(int i)
{

  switch (i)
  {
  case 1:
    digitalWrite(relay1, HIGH);
    relayValue1 = "0";
    break;
  case 2:
    digitalWrite(relay2, HIGH);
    relayValue2 = "0";
    break;
  case 3:
    digitalWrite(relay3, HIGH);
    relayValue3 = "0";
    break;
  case 4:
    digitalWrite(relay4, HIGH);
    relayValue4 = "0";
    ;
    break;
  case 5:
    digitalWrite(relay5, HIGH);
    relayValue5 = "0";
    break;
  case 6:
    digitalWrite(relay6, HIGH);
    relayValue6 = "0";
    break;
  case 7:
    digitalWrite(relay7, HIGH);
    relayValue7 = "0";
    break;
  case 8:
    digitalWrite(relay8, HIGH);
    relayValue8 = "0";
    break;
  }
}

void set_(int i)
{
  switch (i)
  {
  case 1:
    digitalWrite(relay1, LOW);
    relayValue1 = "1";
    break;
  case 2:
    digitalWrite(relay2, LOW);
    relayValue2 = "1";
    break;
  case 3:
    digitalWrite(relay3, LOW);
    relayValue3 = "1";
    break;
  case 4:
    digitalWrite(relay4, LOW);
    relayValue4 = "1";
    ;
    break;
  case 5:
    digitalWrite(relay5, LOW);
    relayValue5 = "1";
    break;
  case 6:
    digitalWrite(relay6, LOW);
    relayValue6 = "1";
    break;
  case 7:
    digitalWrite(relay7, LOW);
    relayValue7 = "1";
    break;
  case 8:
    digitalWrite(relay8, LOW);
    relayValue8 = "1";
    break;
  }
}
*/

// Get Slider Values
String getRelaysValues()
{
  relaysValues["relayValue1"] = String(relayValue1);
  relaysValues["relayValue2"] = String(relayValue2);
  relaysValues["relayValue3"] = String(relayValue3);
  relaysValues["relayValue4"] = String(relayValue4);
  relaysValues["relayValue5"] = String(relayValue5);
  relaysValues["relayValue6"] = String(relayValue6);
  relaysValues["relayValue7"] = String(relayValue7);
  relaysValues["relayValue8"] = String(relayValue8);
  String jsonString = JSON.stringify(relaysValues);

  //////

  return jsonString;
}

void wificonnect()
{

  Serial.println("Connecting to Wifi...");
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
  {
    Serial.println("STA Failed to configure");
  }
  // Connect to Wi-Fi
  Serial.println("Starting wifi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
    WiFi.begin(ssid, password);
    Serial.printf("connect ssid:%s , Pass:%s \n",ssid,password);
  }

  // Print ESP32 Local IP Address
  Serial.println("Connected to Wifi successfully.");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.dnsIP());
  Serial.println(WiFi.gatewayIP());
  Serial.println(WiFi.gatewayIP());
}

void notifyClients(String sliderValues)
{
  ws.textAll(sliderValues);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {
    data[len] = 0;
    message = (char *)data;
    Serial.println(message);

    if (strcmp((char *)data, "getValues") == 0)
    {
      notifyClients(getRelaysValues());
    }

    if (message.indexOf("1s") >= 0) // Relay1
    {
      relayValue1 = message.substring(message.length() - 1);
      Serial.println("relayValue1 = " + relayValue1);

      Serial.println(getRelaysValues());
      // Serial.println(relayValue1.length());
      if (relayValue1 == "1")
      {
        Serial.println("Relay1 ON");
        digitalWrite(relay1, LOW);
      }
      else
      {
        Serial.println("Relay1 OFF");
        digitalWrite(relay1, HIGH);
      }
      notifyClients(getRelaysValues());
    }

    if (message.indexOf("2s") >= 0) // Relay2
    {
      relayValue2 = message.substring(message.length() - 1);
      Serial.println("relayValue2 = " + relayValue2);
      Serial.println(getRelaysValues());
      // Serial.println(relayValue2.length());
      if (relayValue2 == "1")
      {
        Serial.println("Relay2 ON");
        digitalWrite(relay2, LOW);
      }
      else
      {
        Serial.println("Relay2 OFF");
        digitalWrite(relay2, HIGH);
      }
      notifyClients(getRelaysValues());
    }

    if (message.indexOf("3s") >= 0) // Relay3
    {
      relayValue3 = message.substring(message.length() - 1);
      Serial.println("relayValue3 = " + relayValue3);
      Serial.println(getRelaysValues());
      if (relayValue3 == "1")
      {
        Serial.println("Relay3 ON");
        digitalWrite(relay3, LOW);
      }
      else
      {
        Serial.println("Relay3 OFF");
        digitalWrite(relay3, HIGH);
      }
      notifyClients(getRelaysValues());
    }

    if (message.indexOf("4s") >= 0) // Relay4
    {
      relayValue4 = message.substring(message.length() - 1);
      Serial.println("relayValue4 = " + relayValue4);
      Serial.println(getRelaysValues());

      if (relayValue4 == "1")
      {
        Serial.println("Relay4 ON");
        digitalWrite(relay4, LOW);
      }
      else
      {
        Serial.println("Relay4 OFF");
        digitalWrite(relay4, HIGH);
      }
      notifyClients(getRelaysValues());
    }

    if (message.indexOf("5s") >= 0) // Relay15
    {
      relayValue5 = message.substring(message.length() - 1);
      Serial.println("relayValue5 = " + relayValue5);
      Serial.println(getRelaysValues());

      if (relayValue5 == "1")
      {
        Serial.println("Relay5 ON");
        digitalWrite(relay5, LOW);
      }
      else
      {
        Serial.println("Relay5 OFF");
        digitalWrite(relay5, HIGH);
      }
      notifyClients(getRelaysValues());
    }

    if (message.indexOf("6s") >= 0) // Relay6
    {
      relayValue6 = message.substring(message.length() - 1);
      Serial.println("relayValue6 = " + relayValue6);
      Serial.println(getRelaysValues());

      if (relayValue6 == "1")
      {
        Serial.println("Relay6 ON");
        digitalWrite(relay6, LOW);
      }
      else
      {
        Serial.println("Relay6 OFF");
        digitalWrite(relay6, HIGH);
      }

      notifyClients(getRelaysValues());
    }

    if (message.indexOf("7s") >= 0) // Relay7
    {
      relayValue7 = message.substring(message.length() - 1);
      Serial.println("relayValue7 = " + relayValue7);
      Serial.println(getRelaysValues());
      if (relayValue7 == "1")
      {
        Serial.println("Relay7 ON");
        digitalWrite(relay7, LOW);
      }
      else
      {
        Serial.println("Relay7 OFF");
        digitalWrite(relay7, HIGH);
      }

      notifyClients(getRelaysValues());
    }

    if (message.indexOf("8s") >= 0) // Relay8
    {
      relayValue8 = message.substring(message.length() - 1);
      Serial.println("relayValue8 = " + relayValue8);
      Serial.println(getRelaysValues());

      if (relayValue8 == "1")
      {
        Serial.println("Relay8 ON");
        digitalWrite(relay8, LOW);
      }
      else
      {
        Serial.println("Relay8 OFF");
        digitalWrite(relay8, HIGH);
      }
      notifyClients(getRelaysValues());
    }

    if (message.indexOf("checkbox") >= 0  )
    {
      Serial.print("message=");
      Serial.println(message);
      Serial.println("Checkbox selected..");
      
    JSONVar var=   JSON.parse(message);
    

    Serial.println(var["checkbox"]);
    Serial.println(var["value"]);
     String name = JSON.stringify(var["checkbox"]);
     name.replace("\"","");
    
    String value = JSON.stringify(var["value"]);
    monday[name]=value;

    Serial.print("check boxes states:");
    Serial.println(JSON.stringify(monday));
    Serial.println(JSON.stringify(relaysValues));

  

    
    
     }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  switch (type)
  {
  case WS_EVT_CONNECT:
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    break;
  case WS_EVT_DISCONNECT:
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
    break;
  case WS_EVT_DATA:
    handleWebSocketMessage(arg, data, len);
    break;
  case WS_EVT_PONG:
  case WS_EVT_ERROR:
    break;
  }
}

void initWebSocket()
{
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void loadvalues()
{
  relayValue1 = String(r1);
  relayValue2 = String(r2);
  relayValue3 = String(r3);
  relayValue4 = String(r4);
  relayValue5 = String(r5);
  relayValue6 = String(r6);
  relayValue7 = String(r7);
  relayValue8 = String(r8);
  getRelaysValues();
}

void setRelaysValues()
{
  
      if (relayValue1 == "1")
      {
        Serial.println("Relay1 ON");
        digitalWrite(relay1, LOW);
      }
      else
      {
        Serial.println("Relay1 OFF");
        digitalWrite(relay1, HIGH);
      }
	  
	  if (relayValue2 == "1")
      {
        Serial.println("Relay2 ON");
        digitalWrite(relay2, LOW);
      }
      else
      {
        Serial.println("Relay2 OFF");
        digitalWrite(relay2, HIGH);
      }
	  
	  if (relayValue3 == "1")
      {
        Serial.println("Relay3 ON");
        digitalWrite(relay3, LOW);
      }
      else
      {
        Serial.println("Relay3 OFF");
        digitalWrite(relay3, HIGH);
      }
	  
	  if (relayValue4 == "1")
      {
        Serial.println("Relay4 ON");
        digitalWrite(relay4, LOW);
      }
      else
      {
        Serial.println("Relay4 OFF");
        digitalWrite(relay4, HIGH);
      }
	  
	  if (relayValue5 == "1")
      {
        Serial.println("Relay5 ON");
        digitalWrite(relay5, LOW);
      }
      else
      {
        Serial.println("Relay5 OFF");
        digitalWrite(relay5, HIGH);
      }
	  
      if (relayValue6 == "1")
      {
        Serial.println("Relay6 ON");
        digitalWrite(relay6, LOW);
      }
      else
      {
        Serial.println("Relay6 OFF");
        digitalWrite(relay6, HIGH);
      }
	  
	  if (relayValue7 == "1")
      {
        Serial.println("Relay7 ON");
        digitalWrite(relay7, LOW);
      }
      else
      {
        Serial.println("Relay7 OFF");
        digitalWrite(relay7, HIGH);
      }
	  
	   if (relayValue8 == "1")
      {
        Serial.println("Relay8 ON");
        digitalWrite(relay8, LOW);
      }
      else
      {
        Serial.println("Relay8 OFF");
        digitalWrite(relay8, HIGH);
      }
      
}
void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  esp_reset_reason_t reason = esp_reset_reason();
  if ((reason != ESP_RST_DEEPSLEEP) && (reason != ESP_RST_SW))
  {
    resetvalues();
    Serial.println("Hello hard");
  }

  loadvalues();
  Serial.println(getRelaysValues());
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  pinMode(relay5, OUTPUT);
  pinMode(relay6, OUTPUT);
  pinMode(relay7, OUTPUT);
  pinMode(relay8, OUTPUT);
  pinMode(BUILTIN_LED, OUTPUT);

  setRelaysValues();
  // digitalWrite(relay1, HIGH);
  // digitalWrite(relay2, HIGH);
  // digitalWrite(relay3, HIGH);
  // digitalWrite(relay4, HIGH);
  // digitalWrite(relay5, HIGH);
  // digitalWrite(relay6, HIGH);
  // digitalWrite(relay7, HIGH);
  // digitalWrite(relay8, HIGH);
  Serial.println("######################################");
  Serial.println("Hello , Started HTTP 8Relay Controller , With OTA");
  Serial.println("######################################");

  // Initialize SPIFFS
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  wificonnect();

  OTAserver.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(200, "text/plain", "Hi! I am ESP32."); });
  AsyncElegantOTA.begin(&OTAserver); // Start ElegantOTA
  OTAserver.begin();                 // added on 13/7/2022
  Serial.println("HTTP server started");

  initWebSocket();

  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", "text/html"); });

   server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/timer.html", "text/html"); });

  server.serveStatic("/", SPIFFS, "/");

  // Start server
  server.begin();
  notifyClients(getRelaysValues());
}

unsigned long lastWifiConnect = 0;

void loop()
{

  savevalues();
  // put your main code here, to run repeatedly:
  ws.cleanupClients();

  if (WiFi.status() == 3)
  {
    digitalWrite(BUILTIN_LED, HIGH);
    lastWifiConnect = millis();
  }

  if (WiFi.status() != 3)
  {
    digitalWrite(BUILTIN_LED, LOW);

    if (millis() - lastWifiConnect >= 15000) // after 15 sec no wiif
    {
      lastWifiConnect = millis();
      Serial.println("Connection to Wifi lost , Reconnecting...");
      // ESP.restart();
      wificonnect();
    }
  }

 
}