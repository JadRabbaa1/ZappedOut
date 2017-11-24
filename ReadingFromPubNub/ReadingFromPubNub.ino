/*
 * Zapped Out Project - Experiment 4 (Messaging & Notifications)
 * Kylie Caraway / Jad Rabbaa
 * 
 * Creation & Computation - Digital Futures, OCAD University
 * Kate Hartman / Nick Puckett
 * 
 * publish flex sensor value that is mapped to a ligh pin value to PubNub at a given interval
 * 
 */
 
#include <ArduinoJson.h> 
#include <SPI.h>

#include <WiFi101.h>
#define PubNub_BASE_CLIENT WiFiClient
#include <PubNub.h>

static char ssid[] = "xxx";      //SSID of the wireless network
static char pass[] = "xxx";    //password of that network
int status = WL_IDLE_STATUS;                // the Wifi radio's status

const static char pubkey[] = "xxx";  //get this from your PUbNub account
const static char subkey[] = "xxx";  //get this from your PubNub account

const static char subChannel[] = "SubscribingChannel"; //choose a name for the channel to publish messages to

unsigned long lastRefresh = 0;
int publishRate = 1000;

int ledPin10 = 10; // level 1
int ledPin11 = 11; // level 2
int ledPin12 = 12; // level 3

int FlexPinVal1;

void setup() 
{
  Serial.begin(9600);
  pinMode(ledPin10, OUTPUT);
  pinMode(ledPin11, OUTPUT);
  pinMode(ledPin12, OUTPUT);
  connectToServer();
}


void loop() 
{
    // the flex sensor is not flexed at all. (resting)
    if (FlexPinVal1>300){         
    digitalWrite(ledPin10, LOW);
    digitalWrite(ledPin11, LOW);
    digitalWrite(ledPin12, LOW);
    }

    // the flex sensor is not flexed a litte bit
    if (FlexPinVal1>285 && FlexPinVal1 < 299){ 
    digitalWrite(ledPin10, HIGH);
    digitalWrite(ledPin11, LOW);
    digitalWrite(ledPin12, LOW);
    }

    // the flex sensor is not flexed emore
    if (FlexPinVal1>271 && FlexPinVal1< 284){
    digitalWrite(ledPin10, HIGH);
    digitalWrite(ledPin11, HIGH);
    digitalWrite(ledPin12, LOW);
    }

     // the flex sensor is flexed to the maximum
    if (FlexPinVal1< 270){
    digitalWrite(ledPin10, HIGH);
    digitalWrite(ledPin11, HIGH);
    digitalWrite(ledPin12, HIGH);
    }

  if(millis()-lastRefresh>=publishRate)     //theTimer to trigger the reads 
  {
  readFromPubNub();
  //Serial.print("flexvalue ");
  //Serial.println(FlexPinVal1);
   
  lastRefresh=millis();   
  }

}


void connectToServer()
{
  WiFi.setPins(8,7,4,2); //This is specific to the feather M0
  status = WiFi.begin(ssid, pass);                    //attempt to connect to the network
  Serial.println("***Connecting to WiFi Network***");

 for(int trys = 1; trys<=10; trys++)                    //use a loop to attempt the connection more than once
 { 
    if ( status == WL_CONNECTED)                        //check to see if the connection was successful
    {
      Serial.print("Connected to ");
      Serial.println(ssid);
  
      PubNub.begin(pubkey, subkey);                      //connect to the PubNub Servers
      Serial.println("PubNub Connected"); 
      break;                                             //exit the connection loop     
    } 
    else 
    {
      Serial.print("Could Not Connect - Attempt:");
      Serial.println(trys);
    }

    if(trys==10)
    {
      Serial.println("I don't this this is going to work");
    }
    delay(1000);
 }

}

void readFromPubNub()
{
  StaticJsonBuffer<1200> inBuffer;                    //create a memory buffer to hold a JSON Object
  WiFiClient *sClient =PubNub.history(subChannel,1);

  if (!sClient) {
    Serial.println("message read error");
    delay(300);
    return;
  }

 while (sClient->connected()) 
  {
    while (sClient->connected() && !sClient->available()) ; // wait
    char c = sClient->read();
    JsonObject& sMessage = inBuffer.parse(*sClient);
    
    if(sMessage.success())
    {
      sMessage.prettyPrintTo(Serial); //uncomment to see the JSON message in the serial monitor
      FlexPinVal1 = sMessage["FlexValue"];  //this is the value of how much the flex sensor is being flexed on the other feather
      Serial.print("FlexValue");
      Serial.println(FlexPinVal1);
    }
   
  }
  
  sClient->stop();

}
