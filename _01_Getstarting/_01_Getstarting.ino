#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#include "DHT.h"
////////////////////////////////////////////////////////////////////////////////
//////////Define Pin///////////////////////////////////////////////////////////
#define USE_SERIAL Serial
int DHTPIN = 2;          //setpin of DHT at D4
int DHTTYPE = DHT11;     //set Dht Type 
/////////////////-----------------------------------//////////////////////////
int Pump = 4;             //setpin of AC_Pump at D2
int Valve = 5;               //setpin of Valve to D1
int Water_Max = 14;            //setpin of MAX at D5
int Water_Min = 12;            //setpin of Min at D6
//int Led_Blue = 13;            //setpin of Led Red at D7
///////////////////////////////////////////////////////////////////////////////
void SendData(float h,float t,String w);     //Define Senddata Voide
void go_auto(String w);
////////////Setup//////////////////////////////////////////////////////////////

const char* ssid     = "true_home2G_792";            //Set ssid
const char* password = "ilovestudy";                    //Set Password
const char* Server   = "192.168.1.49";           //set Server Domain or Server ip
ESP8266WiFiMulti WiFiMulti;
DHT dht(DHTPIN, DHTTYPE);                             //Start DHT

void setup() 
{
    USE_SERIAL.begin(115200);
      for(uint8_t t = 6; t > 0; t--) 
      {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
      }
    WiFiMulti.addAP(ssid, password);    //Set SSID and Password (SSID, Password)
    WiFi.begin(ssid, password);         //Set starting for Wifi
    Serial.println(WiFi.localIP());
    dht.begin();
    ////////////////////////////////SetuoPinMode//////////////////////////////////////////
    pinMode(Pump,OUTPUT);
    pinMode(Valve,OUTPUT);
    pinMode(Water_Min,INPUT);
    pinMode(Water_Max,INPUT);
    
    
                                            
}

////////////////////Loop////////////////////////////////////////////////////////////////////

void loop() 
{ 
    //float h = 30;
    //float t = 25;
    float h = dht.readHumidity();      //Read Humidity
    float t = dht.readTemperature();   //Read Temperature
    String w = "";
  if (isnan(t) || isnan(h)) 
  {
    Serial.println("FaiLed to read from DHT");
  } 
  else
  {
      if(digitalRead(Water_Min)==1&&digitalRead(Water_Max)==1)
     {
        w = "LOW_Water";
      }
      if(digitalRead(Water_Min)==0&&digitalRead(Water_Max)==1)
      {
        w = "OK_Water";
      }
      if(digitalRead(Water_Min)==0&&digitalRead(Water_Max)==0)
      {
        w ="HIGH_Water";
      }
      
      SendData(h,t,w);
  }
  

}
void go_auto(String w )
{
  Serial.println("AUTO");
  if(w=="HIGH_Water"){
    digitalWrite(Pump,HIGH);
    Serial.println("OnPump");
    delay(10000);
    digitalWrite(Pump,LOW);
    Serial.println("OffPump");
  }
  if(w=="OK_Water")
  {
   digitalWrite(Pump,HIGH);
   Serial.println("OnPump");
   delay(15000);
   digitalWrite(Pump,LOW);
   Serial.println("OffPump"); 
   delay(60000);
  }
  if(w=="LOW_Water")
  {
    digitalWrite(Valve,HIGH);
    Serial.println("OnPump");
    delay(5000);
    digitalWrite(Valve,LOW);
    Serial.println("OffPump");
  }
}
///////////////////////SednData//////////////////////////////////////////////////////////////////
void SendData(float h,float t,String w) 
{
  
  // wait for WiFi connection
    if((WiFiMulti.run() == WL_CONNECTED)) 
    {
        HTTPClient http;
        String str = "http://" +String(Server)+":5000" +"/data/" + String(t)+"/"+String(h)+"/"+w;
        Serial.println(str);
        http.begin(str);
        int httpCode = http.GET();
        USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);
        if(httpCode > 0) 
        {
            if(httpCode == HTTP_CODE_OK) 
              {
                String payload = http.getString();
      //-------------------Control----------------------------//
                USE_SERIAL.print("payload");
                USE_SERIAL.println(payload);
                String x = payload.substring(4,7);
                Serial.println(payload.substring(0,3));
                Serial.println(payload.substring(4,7));
                Serial.println (payload.substring(8));
                
                if( payload.substring(8)=="M")
                {
                  Serial.println("Manual");
                   if(payload.substring(0,3)=="101")
                   {
                     //OnPump;
                     digitalWrite(Pump,HIGH);
                     Serial.println("OnPump");
                    }
                   if(payload.substring(4,7)=="201")
                    {
                      //OnValve;
                      digitalWrite(Valve,HIGH);
                      Serial.println("OnValve");                    
                    }
                   if(payload.substring(0,3)=="100")
                    {
                      //OffPump;
                      digitalWrite(Pump,LOW);
                      Serial.println("OffPump");
                    }
                   if(payload.substring(4,7)=="200")
                    {
                      //OffValve;
                      digitalWrite(Valve,LOW);
                      Serial.println("OffValve");
                    }
                }
                else
                {
                  go_auto(w);
                }      
              }
              
              
        }
        
        http.end();
        
    }
    else
        {
          go_auto(w);
        }
    delay(1000);
  }





