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

static char ssid[] = "xxx";           //SSID of the wireless network
static char pass[] = "xxx";           //password of that network
int status = WL_IDLE_STATUS;          // the Wifi radio's status

const static char pubkey[] = "xxx";   //get this from your PUbNub account
const static char subkey[] = "xxx";   //get this from your PubNub account

const static char pubChannel[] = "PublishingChannel";   //choose a name for the channel to publish messages to

unsigned long lastRefresh = 0;
int publishRate = 1000;

int FLEX_PIN = A0;  // flex sensor is attached to pin A0
int FlexPinVal;     // this int holds the value of the of the sensor value 

void setup() 
{
  Serial.begin(9600);
  pinMode(FLEX_PIN, INPUT); // flex sensor is an INPUT (the OUTPUT is on the other circuit) 
  connectToServer();        // connect to pubnub 
}

void loop() 
{
  if(millis()-lastRefresh>=publishRate)   //timer used to publish the values at a given rate
  {
  publishToPubNub();                      // publish to pubnub
  lastRefresh=millis();
  
  FlexPinVal = analogRead(FLEX_PIN);      // this is the int that hold raw value of the flex sensor
  
  Serial.println(FlexPinVal);             // this the int that holds the raw value
  }
  
}


void connectToServer()
{
  WiFi.setPins(8,7,4,2);                   //This is specific to the feather M0
 
  status = WiFi.begin(ssid, pass);         // attempt to connect to the network
  Serial.println("***Connecting to WiFi Network***");

 for(int trys = 1; trys<=10; trys++)       // use a loop to attempt the connection more than once
 { 
    if ( status == WL_CONNECTED)           // check to see if the connection was successful
    {
      Serial.print("Connected to ");
      Serial.println(ssid);
  
      PubNub.begin(pubkey, subkey);        // connect to the PubNub Servers
      Serial.println("PubNub Connected"); 
      break;                               // exit the connection loop     
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


void publishToPubNub()
{
  WiFiClient *client;
  StaticJsonBuffer<800> messageBuffer;                    // create a memory buffer to hold a JSON Object
  JsonObject& pMessage = messageBuffer.createObject();    // create a new blank JSON object in that buffer called pMessage
  
 ///the imporant bit where you feed in values
  pMessage["FlexValue"] = FlexPinVal; // add a new property and give it a value

  pMessage.prettyPrintTo(Serial);     //uncomment this to see the messages in the serial monitor
  
  int mSize = pMessage.measureLength()+1;    //determine the size of the JSON Message
  char msg[mSize];                           //create a char array to hold the message 
  pMessage.printTo(msg,mSize);               //convert the JSON object into simple text (needed for the PN Arduino client)
  
  client = PubNub.publish(pubChannel, msg);  //publish the message to PubNub

  if (!client)                               //error check the connection
  {
    Serial.println("client error");
    delay(500);
    return;
  }
  
  if (PubNub.get_last_http_status_code_class() != PubNub::http_scc_success)  // check that it worked
  {
    Serial.print("Got HTTP status code error from PubNub, class:");
    Serial.print(PubNub.get_last_http_status_code_class(), DEC);
  }
  
  while (client->available()) 
  {
    Serial.write(client->read());
  }
  client->stop();
  Serial.println("Successful Publish");
}
