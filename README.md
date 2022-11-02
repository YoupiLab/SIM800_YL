# GSM

## Tested functions

1- String getUSSDRequest(String request): fonctionne.

2- void doCall(String number): fonctionne

3- void sendSMS(String number, String msg):

4- int getMsgCount(): fonctionne

5- int getMaxiMsgCount(): fonctionne

6- String getLastMsgContent(): fonctionne

7- String getLastMsgSender(): fonctionne

8- String getLastMsgDate(): fonctionne

9- String getLastMsgHour(): fonctionne

10- String getAllMsg(): pas très bien (ne liste pas tous les messages). Mais en récupérant le nombre de messages qu'il y a on peut ensuite itérer depuis le début (1) et utiliser les fonctions listées en haut

11 - boolean isKeyWordInMsg(int n): fonctionne

12- boolean isKeyWordInLastMsg(): fonctionne

13- void deleteMsg(int n): fonctionne

14- void deleteAllMsg(): fonctionne

# GPRS

struct HTTPResponse {
int status = 0;
int size = 0;
String response = "";
};

1- uint32_t HTTPRequestTimeout = 10000L;

2 - String getAPNInfos();

3- boolean isAPNInfosSet(String apn);

4- void setAPNInfos(String apn, String user, String pwd);

5- HTTPResponse sendRequest(String& url, int type);

6- HTTPResponse GET(String& url);

7- HTTPResponse POST(String& url);
