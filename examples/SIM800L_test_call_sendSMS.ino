#include <sim800Lib.h>

Sim800Lib Sim(10,11);// Le module est connecté aux pins

String phone_number = "+22962901009"; // this is phone number
int response=9;

void setup() 
{

 Serial.begin(9600);
 Sim.startModule();
  // 
  Sim.setWaitingTime(300);
  Sim.makeCall(phone_number);
  delay(5000);

  String message = "I love YOupilab";
  Sim.sendSMS(phone_number, message); //Send a sms to a phone number
  
 
}

void loop() 
{
 
}
