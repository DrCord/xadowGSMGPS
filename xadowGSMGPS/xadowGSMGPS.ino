/*
    Eugene Maker Space NearSpace Balloon Project Module
    GPS Locator Program Module
    By DrCord @ 5-19-2016
    
   This program is for the GPS Locator Program Module that is built 
   with the Xadow GSM+BLE and Xadow GPS v2 modules from the RePhone GEO kit by Seeed.
   http://www.seeedstudio.com/wiki/Rephone
   
   Program Functionality:
   1. The device regularly sends it's GPS data via serial USB in a parseable format.
   2. A message with the content 'GPS' (case insensitive) will trigger your RePhone
      to send the GPS information back to the sender.
*/

// Include the necessary libraries.
#include <LCheckSIM.h>
#include <LGSM.h>
#include <LGPS.h>
#include <stdio.h>
#include <string.h>

char num[20] = {0};
char buf_contex[100];
char buffer[200] = {0,};
unsigned char *utc_date_time = 0;
int loopCounter = 0;

void setup(){
    Serial.begin(115200);
    Serial.print("\r\n");
    Serial.print("GPS Locator Module Online\r\n");
}

//set a loop
void loop(){
    unsigned KeyValue = 0;

    // If received a SMS from another cellphone.
    if(LSMS.available()){
        LSMS.remoteNumber(num, 20); 
        LSMS.remoteContent(buf_contex, 50);
        
        sprintf(buffer, "Received new sms, content: %s, number: %s \r\n", buf_contex, num);
        Serial.println(buffer);

        // If the SMS content is 'GPS' (case insensitive), 
        // then send the GPS information of device back to the sender.
        if(String("GPS").equalsIgnoreCase((char*)buf_contex)){
            if(LGPS.check_online()){
              outputGPS();
              Serial.println(buffer);
                
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
    // Write GPS location to serial every 20 loops
    if(loopCounter > 20){
      outputGPS();
      Serial.println(buffer);
      loopCounter = 0;
    }
    else{
      loopCounter++;
    }
    delay(100);  
}

void outputGPS(){
  if(LGPS.check_online()){
    utc_date_time = LGPS.get_utc_date_time();
    sprintf(buffer, "GPS list:\r\nUTC:%d-%d-%d  %d:%d:%d,\r\nlatitude: %c:%f,\r\nlongitude: %c:%f,\r\naltitude: %f,\r\nspeed: %f,\r\ncourse: %f.", 
                       utc_date_time[0], utc_date_time[1], utc_date_time[2], utc_date_time[3], utc_date_time[4], utc_date_time[5], 
                       LGPS.get_ns(), LGPS.get_latitude(), LGPS.get_ew(), LGPS.get_longitude(), LGPS.get_altitude(), LGPS.get_speed(), LGPS.get_course());
  }
}