## A repository to quickly use the different features of SIM800L

We have made many functions for GMS utilities and HTTP

### GSM

- Get USSD request
- Make a call
- Send a SMS
- Get count of all messages send
- Get the maximum messages which can be sent
- Get the content of the last message
- Get the sender of the last message
- Get the date of the last message
- Get the hour of the last message
- Get list of all messages
- Search a key word in a specific message
- Search a key word in the last message
- Delete a specific message 
- Delete all messages

All these functions have been properly commented to your comprehension

```
 #include <sim800Lib.h>

 Sim800Lib Sim(10,11);

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
```

### GPRS

[//]: # (struct HTTPResponse {)

[//]: # (int status = 0;)

[//]: # (int size = 0;)

[//]: # (String response = "";)

[//]: # (};)

- one
- two

1- uint32_t HTTPRequestTimeout = 10000L;

2 - String getAPNInfos();

3- boolean isAPNInfosSet(String apn);

4- void setAPNInfos(String apn, String user, String pwd);

5- HTTPResponse sendRequest(String& url, int type);

6- HTTPResponse GET(String& url);

7- HTTPResponse POST(String& url);

