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

  String message = "I love Youpilab";
  Sim.sendSMS(phone_number, message); //Send a sms to a phone number
  Sim.getLastMsgContent() //Get the last message
  Sim.getLastMsgSender(); // Get the last message you had sent
  Sim.getLastMsgDate(); //Get the date of the last message sent
  Sim.getRemainingMessage(); // get the number of messages remaining
  Sim.getMaxiMsgCount(); // get the maximum number of messages your sim can contain
 
}

void loop() 
{
 
}
