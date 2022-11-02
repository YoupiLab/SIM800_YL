#include <sim800Lib.h>

Sim800Lib Sim(10,11);// Le module est connecté aux pins

void doCall(String number);
int response=9;

void setup() 
{

 Serial.begin(9600);
 Sim.startModule();
    
//void setKeyWord(String word);
//int getMsgCount();
//int getMaxiMsgCount();
//int getStayMsgCount();//remaining
//void sendSMS(String number, String msg);
//void doCall(String number); //Make call
//String getUSSDRequest(String request);
//String getAllMsg();
//String getMsgContent(int n);
//String getMsgSender(int n);
//String getMsgDate(int n);
//String getMsgHour(int n);
//String getLastMsgContent();
//String getLastMsgSender();
//String getLastMsgDate();
//String getLastMsgHour();
//void deleteMsg(int n);
//void deleteAllMsg();
//boolean isKeyWordInMsg(int n);
//boolean isKeyWordInLastMsg();
// 
Sim.setWaitingTime(300);
  //Sim.doCall("69064632");
  delay(5000);
  Serial.println("Waiting time:");
  
 Serial.print(Sim.getWaitingTime() );
 Serial.println(" ");
 delay(5000);
 
  delay(10000);
  Serial.println("*124#");
  Serial.println(Sim.getUSSDRequest("*124#") );
   delay(10000);
   /*
  Serial.println("*400#");
  Serial.println(Sim.getUSSDRequest("*400#") );
  Serial.println(Sim.getUSSDRequest("1") );
  Serial.println(Sim.getUSSDRequest("1") );
  Serial.println(Sim.getUSSDRequest("69064632") );
  Serial.println(Sim.getUSSDRequest("69064632") );
  Serial.println(Sim.getUSSDRequest("1") );
  Serial.println(Sim.getUSSDRequest("relicat") );
  Serial.println(Sim.getUSSDRequest("02019") );
  delay(5000);
  */
    Serial.println(Sim.getUSSDRequest("*130*1*1*1#") );
    Serial.println(Sim.getUSSDRequest("1") );
    
  
 
}

String numero="69064632";
double lat=6.1254;
double lon=2.21456;
int sat=4864;

void loop() 
{
String message_entier;
message_entier +=lat;
message_entier +=" ";
message_entier +=lon;
message_entier +=" ";
message_entier +=sat;
  
Sim.sendSMS(numero, message_entier);
 
}


//*****************************************************************************************************
void contact_server()
{
 /*
Sim.println("AT+SAPBR=1,1\r\n");
Serial.println("AT+SAPBR=1,1\r\n");
delay(3000);

Sim.println("AT+SAPBR=2,1\r\n");
Serial.println("AT+SAPBR=2,1\r\n");
delay(3000);

Sim.println("AT+HTTPINIT\r\n");
Serial.println("AT+HTTPINIT\r\n");
delay(3000);
juniora="";
juniora="AT+HTTPPARA=\"URL\",\"http://studentsprojects.generalinvasion.com/gtuc_fall_alert/index.php?device_id=";
juniora +=array_index;
juniora +="\"\r\n"; 

Sim.println(juniora);
Serial.println(juniora);
delay(3000);

Sim.println("AT+HTTPACTION=0\r\n");
Serial.println("AT+HTTPACTION=0\r\n");

Sim.println(0X1A); // execute all  the previous commands
delay(4000);

Sim.println(0X1A); // execute all  the previous commands
delay(8000);  
*/
}
