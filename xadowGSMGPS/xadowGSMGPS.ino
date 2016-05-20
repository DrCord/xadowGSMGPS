/*
  Eugene Maker Space NearSpace Balloon Project Module
  GPS Locator Program Module
  By DrCord @ 5-19-2016
    
  This program is for the GPS Locator Program Module that is built 
  with the Xadow GSM+BLE and Xadow GPS v2 modules from the RePhone GEO kit by Seeed.
  http://www.seeedstudio.com/wiki/Rephone
   
  // Arduino JSON library
  https://github.com/bblanchon/ArduinoJson

  Program Functionality:
  1. The device regularly sends it's GPS data via serial USB in JSON format.
  2. A message with the content 'GPS' (case insensitive) will trigger RePhone device
     to send the GPS information back to the sender's number.
*/

// Include the necessary libraries.
#include <LCheckSIM.h>
#include <LGSM.h>
#include <LGPS.h>
#include <stdio.h>
#include <string.h>
#include <ArduinoJson.h>

char num[20] = {0};
char buf_contex[100];
int loopCounter = 0;

void setup(){
    Serial.begin(115200);
    Serial.print("\r\n");
    Serial.print("GPS Locator Module Online\r\n");
}

void loop(){
  char buffer[200] = {0,};
  unsigned KeyValue = 0;
  unsigned char *utc_date_time = 0;
  char buffer_time[50] = {0,};
  char buffer_ns[50] = {0,};
  char buffer_ew[50] = {0,};
  
  // Memory pool for JSON object tree.
  //
  // Inside the brackets, 200 is the size of the pool in bytes.
  // If the JSON object is more complex, you need to increase that value.
  // StaticJsonBuffer<300> jsonBuffer;
  // StaticJsonBuffer allocates memory on the stack, it can be
  // replaced by DynamicJsonBuffer which allocates in the heap.
  // It's simpler but less efficient.
  //
  DynamicJsonBuffer jsonBuffer;

  // Create the root of the object tree.
  //
  // It's a reference to the JsonObject, the actual bytes are inside the
  // JsonBuffer with all the other nodes of the object tree.
  // Memory is freed when jsonBuffer goes out of scope.
  JsonObject& jsonOutput = jsonBuffer.createObject();
  
  // Most of the time, you can rely on the implicit casts.
  // In other case, you can do root.set<long>("time", 1351824120);
  jsonOutput["sensor"] = "gps";
  
  // If received a SMS text message.
  if(LSMS.available()){
    LSMS.remoteNumber(num, 20); 
    LSMS.remoteContent(buf_contex, 50);
        
    sprintf(buffer, "Received new sms, content: %s, number: %s \r\n", buf_contex, num);
    Serial.println(buffer);

    // If the SMS content is 'GPS' (case insensitive)
    // then send the GPS information of device back to the sender.
    if(String("GPS").equalsIgnoreCase((char*)buf_contex)){
      if(LGPS.check_online()){
        if(LGPS.check_online()){
          utc_date_time = LGPS.get_utc_date_time();
          sprintf(buffer, "GPS list:\r\nUTC:%d-%d-%d  %d:%d:%d,\r\nlatitude: %c:%f,\r\nlongitude: %c:%f,\r\naltitude: %f,\r\nspeed: %f,\r\ncourse: %f.", 
                             utc_date_time[0], utc_date_time[1], utc_date_time[2], utc_date_time[3], utc_date_time[4], utc_date_time[5], 
                             LGPS.get_ns(), LGPS.get_latitude(), LGPS.get_ew(), LGPS.get_longitude(), LGPS.get_altitude(), LGPS.get_speed(), LGPS.get_course());
        }
        //Serial.println(buffer);
                
        if(LSMS.ready()){
          LSMS.beginSMS(num);
          LSMS.print(buffer);
                    
          if(LSMS.endSMS()) Serial.println("SMS sent ok!");
          else Serial.println("SMS send fail!");
        }
          else Serial.println("SMS not ready!");
        }
      }
      LSMS.flush();
    }
    // Write GPS location to serial every 20 loops (roughly every 2 seconds).
    if(loopCounter > 20){
      if(LGPS.check_online()){
        utc_date_time = LGPS.get_utc_date_time();
        sprintf(buffer_time, "UTC: %d-%d-%d %d:%d:%d", utc_date_time[0], utc_date_time[1], utc_date_time[2], utc_date_time[3], utc_date_time[4], utc_date_time[5]);
        jsonOutput["time"] = buffer_time;
        
        JsonObject& latitudeObject = jsonOutput.createNestedObject("latitude");
        sprintf(buffer_ns, "%c", LGPS.get_ns());
        latitudeObject["direction"] = buffer_ns;
        latitudeObject["data"].set(LGPS.get_latitude(), 6);
        
        JsonObject& longitudeObject = jsonOutput.createNestedObject("longitude");
        sprintf(buffer_ew, "%c", LGPS.get_ew());
        longitudeObject["direction"] = buffer_ew;
        longitudeObject["data"].set(LGPS.get_longitude(), 6);        
        
        jsonOutput["altitude"].set(LGPS.get_altitude());
        jsonOutput["speed"].set(LGPS.get_speed());
        jsonOutput["course"].set(LGPS.get_course());
      }
      // Use for actual output.
      jsonOutput.printTo(Serial);
      // Use for debugging output.
      //jsonOutput.prettyPrintTo(Serial);
      loopCounter = 0;     
    }
    else{
      loopCounter++;
    }
    delay(100);  
}