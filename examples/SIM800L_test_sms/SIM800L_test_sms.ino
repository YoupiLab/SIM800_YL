#include <YLSim800Lib.h>

YLSim800Lib Sim(10,11);  // Le module est connecté aux pins

String phone_number = ""; // Enter a phone number here

void setup()
{
    Serial.begin(9600);

    Sim.startModule(); // Start module to start using module's functionalities

    Sim.setWaitingTime(300); // Set a waiting time

    Sim.makeCall(phone_number); // make a call

    delay(5000);

    String message = "I love YoupiLab";

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
