#include "sim800Lib.h"

#include <Arduino.h>
#include <SoftwareSerial.h>

/**********************************PUBLIC
 * FUNCTIONS**********************************/

void Sim800Lib::setWaitingTime(int wTime) {
  if (wTime >= 0) {
    waitingTime = wTime;  // in ms
  }
}

/*
  get the waiting time of the integrated sim
*/
int Sim800Lib::getWaitingTime() { return waitingTime; }

/*
  search and modify a key word
*/
void Sim800Lib::setKeyWord(String word) { keyWord = word; }

/*
  get a key word
*/
String Sim800Lib::getKeyWord() { return keyWord; }

/*
  Set the server expiration time
*/
void Sim800Lib::setServerTimeOut(int outTime) {
  if (outTime >= 0) {
    serverTimeOut = outTime;  // en ms
  }
}
/*
  get the server expiration time
*/
int Sim800Lib::getServerTimeOut() { return serverTimeOut; }

/*
  reset the time expiration time
*/
void Sim800Lib::setResetTime(int rTime)  // rTime en minute
{
  if (rTime >= 1) {
    REPOP_TIME = rTime * 60000;  // en ms
  }
}

/*
  get the server reset time
*/
int Sim800Lib::getResetTime() { return REPOP_TIME; }

/*
  count the number of messages sent on your sim
*/
int Sim800Lib::getMsgCount() {
  mySerial->println("AT+CPMS=\"SM\"");  // command to get the number of messages
                                        // sent and the capacity of the sim
  String _rep = get_result(true);       // We recover in _rep the answer of the command
  // the answer is processed to extract the number of messages sent
  if (_rep != "") {
    _rep.replace("\r\n", " ");
    _rep.replace('\0', ' ');
    _rep.replace("OK", " ");
    int _pos[10];
    int _j = 0;
    int _t = _rep.length();
    for (int _i = 0; _i < _t; _i++) {
      if (_rep[_i] == ',') {
        _pos[_j] = _i;
        _j++;
      }
    }
    return (stringToInt(getPart(_pos[1], _pos[2], _rep)));
  }
}

/*
  get the maximum number of SMS that the sim can receive
*/
int Sim800Lib::getMaxiMsgCount() {
  mySerial->println("AT+CPMS=\"SM\"");  // command to get the number of messages
                                        // sent and the capacity of the sim
  String _rep = get_result(true);       // We recover in _rep the answer of the command
  // the answer is processed to extract the maximum number of messages that can
  // be sent
  if (_rep != "") {
    _rep.replace("\r\n", " ");
    _rep.replace('\0', ' ');
    _rep.replace("OK", " ");
    int _pos[10];
    int _j = 0;
    int _t = _rep.length();
    for (int _i = 0; _i < _t; _i++) {
      if (_rep[_i] == ',') {
        _pos[_j] = _i;
        _j++;
      }
    }
    return (stringToInt(getPart(_pos[0], _pos[1], _rep)));
  }
}

/*
  get Remaining Message number of SMS that the sim can receive
*/
int Sim800Lib::getRemainingMessage() { return (getMaxiMsgCount() - getMsgCount()); }

/*
  initialisation of instance
*/
Sim800Lib::Sim800Lib(int rx,
                     int tx)  // This is the constructor of the class with its initializations
{
  Rx       = rx;
  Tx       = tx;
  mySerial = new SoftwareSerial(Rx, Tx);
  //
  lost_compteur  = 0;
  reset_compteur = 0;
  start_time     = 0;
  MAX_TIME       = 3000;  // in ms et MAX_TIME = QUERY_DELAY*1000
  QUERY_DELAY    = "2";   // in s
  RESET_COUNT    = 15;
  //
  waitingTime   = 100;      // in ms
  serverTimeOut = 5000;     // in ms
  msgTimeOut    = 5000;     // in ms
  REPOP_TIME    = 1200000;  // 20 min
  //
}

void Sim800Lib::startModule() {
  start();  // We call the private function that starts the module
}

/*
  send an sms to phone number
*/
void Sim800Lib::sendSMS(String number, String msg) {
  mySerial->println("AT");           // AT is done to make sure the module works
  Serial.println(get_result(true));  // The module's response is displayed in the serial monitor
  delay(waitingTime);                // waiting time

  mySerial->println("AT+CMGF=1");  // This command allows you to select the message format
  Serial.println(get_result(true));
  delay(waitingTime);

  mySerial->println("AT+CMGS=\"" + number + "\"");  // This command sends the message to number
  Serial.println(get_result(true));
  delay(waitingTime);

  mySerial->println(msg);  // the message to send is defined here
  delay(100);
  mySerial->println((char)26);
  Serial.println(get_result(true));
  delay(waitingTime);
}

/*
  make call to phone number
*/
void Sim800Lib::makeCall(String number) {
  mySerial->println("AT");           // AT is done to make sure the module works
  Serial.println(get_result(true));  // The module's response is displayed in the serial monitor
  delay(waitingTime);                // waiting time

  mySerial->println("ATD" + number + ";");  // Command to make a call
  Serial.println(get_result(true));
  delay(waitingTime);
}

/*
  delete an sms, from the sim
*/
void Sim800Lib::deleteMsg(int n) {
  mySerial->println("AT+CMGD=" + intToString(n));  // Command to delete the message n
  Serial.println(get_result(true));
  delay(waitingTime);
}

/*
  delete all sms, from the sim
*/
void Sim800Lib::deleteAllMsg() {
  mySerial->println("AT+CMGD=1,4");  // Command to delete all messages
  Serial.println(get_result(true));
  delay(waitingTime);
}

/*
  listening to an incoming call
*/
int Sim800Lib::inListening(String number)  // Listening to the module: it can do everything
{
  boolean hasRespond = false;     // booleen used to know the module has received information
  int     ret        = -1;        // variable of return
  if (mySerial->available() > 0)  // We get the answer character by character...
  {
    char c = mySerial->read();
    rep += c;
    mySerial->flush();
    if (isCALL(rep)) {
      hasRespond = true;
      goto bailout;
    }
    if (isMSG(rep)) {
      hasRespond = true;
      goto bailout;
    }
    if (is_end(rep)) {
      hasRespond = true;
      goto bailout;
    }
  }
bailout:
  if (hasRespond == true) {
    if (isCALL(rep)) {
      get_CALL(ret, number);  // private function that handles incoming calls.

    } else if (isMSG(rep)) {
      get_SMS(ret,
              number);  // private function that handles the receipt of message
    } else {
      rep.replace("\r\n", " ");
      rep.replace('\0', ' ');
      Serial.println(rep);
      Serial.println(rep);  // write the answer of the module if it is neither a
                            // call nor a message
    }
    rep = "";
  }
  return ret;
}

/*
  get all message from sim
*/
String Sim800Lib::getAllMsg() {
  String s      = "";  // return variable
  int    _count = getMsgCount();
  for (int i = 1; i < _count + 1; i++) {
    // We concatenate the different information of each message
    s += " MESSAGE " + intToString(i) + "\n";
    s += "Sender : " + getMsgSender(i) + "\n";
    s += "Date : " + getMsgDate(i) + "\n";
    s += "Hour : " + getMsgHour(i) + "\n";
    s += "Content : " + getMsgContent(i) + "\n";
  }
  return s;
}

/*
  get a USSD request
*/

String Sim800Lib::getUSSDRequest(String request) {
 
  mySerial->println("AT+CUSD=1,\"" + request + "\"");  // Command which allows to get USSD request
  String        _rep     = "";
  unsigned long _startMS = millis();
  unsigned long _t_rest  = 0;
  while (_t_rest < msgTimeOut)  // We retrieve the information character by character
  {
    if (mySerial->available() > 0) {
      char cc = mySerial->read();
      _rep += cc;
      mySerial->flush();
    }
    _t_rest = millis() - _startMS;
  }
  // We process the information to extract the content
  int _pos[5];
  if (_rep != "") {
    _rep.replace("\r\n", " ");
    _rep.replace('\0', ' ');
    _rep.replace("OK", " ");

    int _j = 0;
    int _t = _rep.length();
    for (int _i = 0; _i < _t; _i++) {
      if (_rep[_i] == '"') {
        _pos[_j] = _i;
        _j++;
      }
    }
  }
  return (getPart(_pos[2], _pos[3], _rep));
  // return(_rep+"azerty");
}

/*
  see the content of a sms in the sim.
*/
String Sim800Lib::getMsgContent(int n) {
  mySerial->println("AT+CMGR=" + intToString(n));  // Command which allows to have the
                                                   // information concerning the message n
  String        _rep     = "";
  unsigned long _startMS = millis();
  unsigned long _t_rest  = 0;
  while (_t_rest < msgTimeOut)  // We retrieve the information character by character
  {
    if (mySerial->available() > 0) {
      char cc = mySerial->read();
      _rep += cc;
      mySerial->flush();
    }
    _t_rest = millis() - _startMS;
  }
  // We process the information to extract the content
  if (_rep != "") {
    _rep.replace("\r\n", " ");
    _rep.replace('\0', ' ');
    _rep.replace("OK", " ");

    int _pos[10];
    int _j = 0;
    int _t = _rep.length();
    for (int _i = 0; _i < _t; _i++) {
      if (_rep[_i] == '"') {
        _pos[_j] = _i;
        _j++;
      }
    }
    return (getPart(_pos[7], _t, _rep));
  }
}

/*
  see the content of a sms that was sent in the sim.
*/
String Sim800Lib::getMsgSender(int n) {
  mySerial->println("AT+CMGR=" + intToString(n));  // Command which allows to have the
                                                   // information concerning the message n
  String        _rep     = "";
  unsigned long _startMS = millis();
  unsigned long _t_rest  = 0;
  while (_t_rest < msgTimeOut) {
    if (mySerial->available() > 0)  // We retrieve the information character by character
    {
      char cc = mySerial->read();
      _rep += cc;
      mySerial->flush();
    }
    _t_rest = millis() - _startMS;
  }
  // We process the information to extract the sender
  if (_rep != "") {
    _rep.replace("\r\n", " ");
    _rep.replace('\0', ' ');
    _rep.replace("OK", " ");
    int _pos[10];
    int _j = 0;
    int _t = _rep.length();
    for (int _i = 0; _i < _t; _i++) {
      if (_rep[_i] == '"') {
        _pos[_j] = _i;
        _j++;
      }
    }
    return (getPart(_pos[2], _pos[3], _rep));
  }
}

/*
  get the time of reception of an sms
*/
String Sim800Lib::getMsgHour(int n) {
  mySerial->println("AT+CMGR=" + intToString(n));  // Command which allows to have the
                                                   // information concerning the message n
  String        _rep     = "";
  unsigned long _startMS = millis();  // We retrieve the information character by character
  unsigned long _t_rest  = 0;
  while (_t_rest < msgTimeOut) {
    if (mySerial->available() > 0) {
      char cc = mySerial->read();
      _rep += cc;
      mySerial->flush();
    }
    _t_rest = millis() - _startMS;
  }

  // We process the information to extract the hour
  if (_rep != "") {
    _rep.replace("\r\n", " ");
    _rep.replace('\0', ' ');
    _rep.replace("OK", " ");

    /*treat rep*/
    int _pos[10];
    int _j = 0;
    int _t = _rep.length();
    for (int _i = 0; _i < _t; _i++) {
      if (_rep[_i] == '"') {
        _pos[_j] = _i;
        _j++;
      }
    }
    int    ind;
    String s = getPart(_pos[6], _pos[7], _rep);

    int _tt = s.length();
    for (int i = 0; i < _tt; i++) {
      if (s[i] == ',') {
        ind = i;
      }
    }

    return (extractPart(ind + 1, _tt - 1 - ind, s));
  }
}

/*
  get the date of reception of an sms
*/
String Sim800Lib::getMsgDate(int n) {
  mySerial->println("AT+CMGR=" + intToString(n));  // Command which allows to have the
                                                   // information concerning the message n
  String        _rep     = "";
  unsigned long _startMS = millis();  // We retrieve the information character by character
  unsigned long _t_rest  = 0;
  while (_t_rest < msgTimeOut) {
    if (mySerial->available() > 0) {
      char cc = mySerial->read();
      _rep += cc;
      mySerial->flush();
    }
    _t_rest = millis() - _startMS;
  }

  // We process the information to extract the date
  if (_rep != "") {
    _rep.replace("\r\n", " ");
    _rep.replace('\0', ' ');
    _rep.replace("OK", " ");

    /*treat rep*/
    int _pos[10];
    int _j = 0;
    int _t = _rep.length();
    for (int _i = 0; _i < _t; _i++) {
      if (_rep[_i] == '"') {
        _pos[_j] = _i;
        _j++;
      }
    }
    int    ind;
    String s = getPart(_pos[6], _pos[7], _rep);

    int _tt = s.length();
    for (int i = 0; i < _tt; i++) {
      if (s[i] == ',') {
        ind = i;
      }
    }

    return (extractPart(0, ind, s));
  }
}

/*
get content the last sms received in the sim
*/
String Sim800Lib::getLastMsgContent() { return getMsgContent(getMsgCount()); }

/*
get content the last sms sent in the sim
*/
String Sim800Lib::getLastMsgSender() { return getMsgSender(getMsgCount()); }

/*
  get the date of the last message sent
*/
String Sim800Lib::getLastMsgDate() { return getMsgDate(getMsgCount()); }

/*
  get the hours of the last message sent
*/
String Sim800Lib::getLastMsgHour() { return getMsgHour(getMsgCount()); }

/*
check the existence of a word in a sms
*/
boolean Sim800Lib::isKeyWordInMsg(int n) { return isKeyWordIn(getMsgContent(n)); }

/*
  check the existence of a word of the last sms
*/
boolean Sim800Lib::isKeyWordInLastMsg() { return isKeyWordInMsg(getMsgCount()); }

/************************** GPRS *************************/
/*
get the gprs results
*/
String Sim800Lib::get_result(boolean _clear, uint32_t timeout)  // Private function of reception of
                                                                // the answers of the sim800
{
  String        rep        = "";
  int           ret        = -1;
  boolean       hasRespond = false;
  unsigned long startMS    = millis();
  unsigned long t_rest     = 0;
  while (t_rest < timeout || mySerial->available() > 0) {
    if (mySerial->available() > 0) {
      char c = mySerial->read();
      rep += c;
      mySerial->flush();
      if (isCALL(rep)) {
        goto bailout;
      }
      if (isMSG(rep)) {
        goto bailout;
      }
      if (is_end(rep)) {
        hasRespond = true;
        goto bailout;
      }
    }
    t_rest = millis() - startMS;
  }
bailout:
  if (isCALL(rep)) {
    get_CALL(ret, "");
  } else if (isMSG(rep)) {
    get_SMS(ret, "");
  } else {
    if (hasRespond == false) {
      reset_module();
    } else {
      rep.replace("\r\n", " ");
      rep.replace('\0', ' ');
      return rep;
    }
  }
}
/*
  extract APN Infos
*/
String Sim800Lib::extractAPNInfos(String &text) {
  size_t nbquote  = 0;
  String apnInfos = "";
  for (size_t i = 0; i < text.length(); i++) {
    if (text[i] == '"') {
      nbquote += 1;
    }

    if (nbquote >= 1) apnInfos += text[i];
    if (nbquote == 6) return apnInfos;
  }

  return apnInfos;
}

/*
  get APN Infos
*/
String Sim800Lib::getAPNInfos() {
  mySerial->println("AT");
  Serial.println(get_result(true));
  delay(waitingTime);

  mySerial->println("AT+CSTT?");
  String apnInfos = get_result(true);
  delay(waitingTime);

  return extractAPNInfos(apnInfos);
}

/*
  is APN Infos Set
*/
boolean Sim800Lib::isAPNInfosSet(String apn) {
  String apnInfos = getAPNInfos();

  if (apnInfos.indexOf(apn) >= 0) return true;

  if (apnInfos.indexOf("CMNET") >= 0) return false;

  return false;
}
/*
  set APN Infos
*/
void Sim800Lib::setAPNInfos(String apn, String user, String pwd) {
  // Serial.println(isAPNInfosSet(apn));
  if (!isAPNInfosSet(apn)) {
    mySerial->println("AT");
    Serial.println(get_result(true));  // The module's response is displayed in the serial monitor
    delay(waitingTime);                // waiting time

    mySerial->println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
    Serial.println(get_result(true));  // The module's response is displayed in the serial monitor
    delay(waitingTime);

    mySerial->println("AT+SAPBR=3,1,\"APN\",\"" + apn + "\"");
    Serial.println(get_result(true));  // The module's response is displayed in the serial monitor
    delay(waitingTime);

    mySerial->println("AT+SAPBR=3,1,\"USER\",\"" + user + "\"");
    Serial.println(get_result(true));  // The module's response is displayed in the serial monitor
    delay(waitingTime);

    mySerial->println("AT+SAPBR=3,1,\"PWD\",\"" + pwd + "\"");
    Serial.println(get_result(true));  // The module's response is displayed in the serial monitor
    delay(waitingTime);

    mySerial->println("AT+CGDCONT=1,\"IP\",\"" + apn + "\"");
    Serial.println(get_result(true));  // The module's response is displayed in the serial monitor
    delay(waitingTime);

    mySerial->println("AT+CGACT=1,1");
    Serial.println(get_result(true, 65000L));  // The module's response is displayed in the serial monitor
    delay(waitingTime);

    mySerial->println("AT+SAPBR=1,1");
    Serial.println(get_result(true, 90000L));  // The module's response is displayed in the serial monitor
    delay(waitingTime);

    mySerial->println("AT+SAPBR=2,1");
    Serial.println(get_result(true, 30000L));  // The module's response is displayed in the serial monitor
    delay(waitingTime);

    mySerial->println("AT+CGATT=1");
    Serial.println(get_result(true, 60000L));  // The module's response is displayed in the serial monitor
    delay(waitingTime);

    mySerial->println("AT+CIPMUX=1");
    Serial.println(get_result(true));  // The module's response is displayed in the serial monitor
    delay(waitingTime);

    mySerial->println("AT+CIPQSEND=1");  // trop cool
    Serial.println(get_result(true));    // The module's response is displayed in the serial monitor
    delay(waitingTime);

    mySerial->println("AT+CSTT=\"" + apn + "\",\"" + user + "\",\"" + pwd + "\"");
    Serial.println(get_result(true, 60000L));  // The module's response is displayed in the serial monitor
    delay(waitingTime);
  }
}
/*
  send an http request
*/
HTTPResponse Sim800Lib::sendRequest(String &url, int type, String contentType) {
  mySerial->println("AT");
  Serial.println(get_result(true));  // The module's response is displayed in the serial monitor
  delay(waitingTime);

  mySerial->println("AT+CIPQSEND=1");  // trop cool
  HTTPAnswer(200L);                    // The module's response is displayed in the serial monitor
  delay(waitingTime);

  mySerial->println("AT+HTTPINIT");
  HTTPAnswer(200L);
  delay(waitingTime);

  mySerial->println("AT+HTTPPARA=\"CID\",1");
  HTTPAnswer(200L);
  delay(waitingTime);

  mySerial->println("AT+HTTPSSL=1");
  HTTPAnswer(200L);
  delay(waitingTime);

  if (contentType.length() != 0) {
    mySerial->println("AT+HTTPPARA=\"CONTENT\",\"" + contentType + "\"");
    HTTPAnswer(200L);
    delay(waitingTime);
  }

  mySerial->println("AT+HTTPPARA=\"URL\",\"" + url + "\"");
  HTTPAnswer(500L);
  delay(waitingTime);

  HTTPResponse res;

  mySerial->println("AT+HTTPACTION=" + String(type));
  String resp = HTTPAnswerAction(HTTPRequestTimeout);
  delay(waitingTime);

  workOnHTTPAction(resp, res.status, res.size);

  if (res.size != 0) {
    mySerial->println("AT+HTTPREAD");
    String returned = HTTPAnswerRead(HTTPRequestTimeout);  // The module's response is displayed in the serial monitor
    // delay(waitingTime);
    workOnHTTPRead(returned, res.response, res.size);
  }

  mySerial->flush();

  mySerial->println("AT+HTTPTERM");
  HTTPAnswer(200L);

  mySerial->flush();

  return res;
}
/*
  get an http request
*/
HTTPResponse Sim800Lib::GET(String &url, String contentType) { return sendRequest(url, 0, contentType); }
/*
  post an http request
*/
HTTPResponse Sim800Lib::POST(String &url, String contentType) { return sendRequest(url, 1, contentType); }
/*
  verify is an http request
*/
boolean Sim800Lib::isHTTP(String &text)  // Check that a message is received
{
  for (int i = 0; i < text.length(); i++) {
    if (text[i] == '+') {
      if (extractPart(i, 11, text) == "+HTTPACTION") {
        return true;
      }
    }
  }
  return false;
}

/*
  work On HTTP Action
*/
void Sim800Lib::workOnHTTPAction(String response, int &code, int &sz) {
  // Serial.println(response);
  int index = response.indexOf("HTTPACTION:");

  int indexVirgule = response.indexOf(',', index);

  code = response.substring(indexVirgule + 1, indexVirgule + 4).toInt();
  sz   = response.substring(indexVirgule + 5).toInt();
}
/*
  work On HTTP Read
*/
void Sim800Lib::workOnHTTPRead(String response, String &serverRes, int sz) {
  int index = response.indexOf(String(sz));

  serverRes = response.substring(index + String(sz).length());
  serverRes.replace("\nOK", "");
  serverRes.replace("\r\nOK", "");
  serverRes.replace("OK", "");
}

/**********************************PRIVATE
 * FUNCTIONS**********************************/

void Sim800Lib::start() {
  // we restart with the new baud
  delay(2000);
  Serial.begin(9600);
  mySerial->begin(9600);
  delay(waitingTime);

  // we prepare the module to receive messages
  mySerial->println("AT");
  Serial.println(get_result(true));
  delay(waitingTime);
  mySerial->println("AT+CMGF=1");
  Serial.println(get_result(true));
  delay(waitingTime);

  // we define the location that will be used to store, read and delete messages
  mySerial->println("AT+CPMS=\"SM\"");
  Serial.println(get_result(true));
  delay(waitingTime);
}

String Sim800Lib::get_result(boolean _clear)  // Private function of reception of
                                              // the answers of the sim800
{
  String        rep        = "";
  int           ret        = -1;
  boolean       hasRespond = false;
  unsigned long startMS    = millis();
  unsigned long t_rest     = 0;
  while (t_rest < serverTimeOut) {
    if (mySerial->available() > 0) {
      char c = mySerial->read();
      rep += c;
      mySerial->flush();
      if (isCALL(rep)) {
        goto bailout;
      }
      if (isMSG(rep)) {
        goto bailout;
      }
      if (is_end(rep)) {
        hasRespond = true;
        goto bailout;
      }
    }
    t_rest = millis() - startMS;
  }
bailout:
  if (isCALL(rep)) {
    get_CALL(ret, "");
  } else if (isMSG(rep)) {
    get_SMS(ret, "");
  } else {
    if (hasRespond == false) {
      reset_module();
    } else {
      rep.replace("\r\n", " ");
      rep.replace('\0', ' ');
      return rep;
    }
  }
}

boolean Sim800Lib::is_end(String &rep)  // Private functions detecting end of module response
{
  int t = rep.length();
  for (int i = 0; i < t; i++) {
    if (rep[i] == '+') {
      String _key = "+CME ERROR:58";
      if (STARTS_WITH(i, _key, rep) == 0) {
        return true;
      }
      String _key2 = "+CME ERROR:50";
      if (STARTS_WITH(i, _key2, rep) == 0) {
        return true;
      }
      String _key3 = "+CMS ERROR:";
      if (STARTS_WITH(i, _key3, rep) == 0) {
        return true;
      }
    }
    if (rep[i] == 'B') {
      String _key = "BUSY\r\n";
      if (STARTS_WITH(i, _key, rep) == 0) {
        return true;
      }
    }
    if (rep[i] == '\r') {
      String _key = "\r\nERROR\r\n";
      if (STARTS_WITH(i, _key, rep) == 0) {
        return true;
      }
    }
    if (rep[i] == '>') {
      String _key = "> ";
      if (STARTS_WITH(i, _key, rep) == 0) {
        return true;
      }
    }
    if (rep[i] == '}') {
      String _key = "}\r\n";
      if (STARTS_WITH(i, _key, rep) == 0) {
        return true;
      }
    }
    if (rep[i] == 'N') {
      String _key = "NO CARRIER\r\n";
      if (STARTS_WITH(i, _key, rep) == 0) {
        return true;
      }
    }
    if (rep[i] == 'O') {
      String _key = "OK\r\n";
      if (STARTS_WITH(i, _key, rep) == 0) {
        return true;
      }
    }
  }
  return false;
}

boolean Sim800Lib::is_action_end(String &rep)  // Private functions that detect the end
                                               // of a call or message module action
{
  int t = rep.length();
  for (int i = 0; i < t; i++) {
    if (rep[i] == '\r') {
      String _key = "\r\n";
      if (STARTS_WITH(i, _key, rep) == 0) {
        return true;
      }
    }
  }
  return false;
}

String Sim800Lib::getFeedBack(String &rep) {
  int t = rep.length();
  for (int i = 0; i < t; i++) {
    if (rep[i] == '+') {
      String _key = "+CME ERROR:58";
      if (STARTS_WITH(i, _key, rep) == 0) {
        return _key;
      }
      String _key2 = "+CME ERROR:50";
      if (STARTS_WITH(i, _key2, rep) == 0) {
        return _key2;
      }
      String _key3 = "+CMS ERROR:";
      if (STARTS_WITH(i, _key3, rep) == 0) {
        return _key3;
      }
    }
    if (rep[i] == 'B') {
      String _key = "BUSY\r\n";
      if (STARTS_WITH(i, _key, rep) == 0) {
        return _key;
      }
    }
    if (rep[i] == '\r') {
      String _key = "\r\nERROR\r\n";
      if (STARTS_WITH(i, _key, rep) == 0) {
        return _key;
      }
    }
    if (rep[i] == '>') {
      String _key = "> ";
      if (STARTS_WITH(i, _key, rep) == 0) {
        return _key;
      }
    }
    if (rep[i] == '}') {
      String _key = "}\r\n";
      if (STARTS_WITH(i, _key, rep) == 0) {
        return _key;
      }
    }
    if (rep[i] == 'N') {
      String _key = "NO CARRIER\r\n";
      if (STARTS_WITH(i, _key, rep) == 0) {
        return _key;
      }
    }
    if (rep[i] == 'O') {
      String _key = "OK\r\n";
      if (STARTS_WITH(i, _key, rep) == 0) {
        return _key;
      }
    }
  }
  return "";
}

int Sim800Lib::STARTS_WITH(int dep, String &key, String &text) {
  int key_size = key.length();
  if ((dep + key_size) <= text.length()) {
    String foundKey = "";
    for (int i = dep; i < (dep + key_size); i++) {
      foundKey += text[i];
    }
    char buf1[foundKey.length()];
    char buf2[key_size];
    foundKey.toCharArray(buf1, foundKey.length());
    key.toCharArray(buf2, key_size);
    return strcmp(buf1, buf2);
  } else {
    return 5;
  }
}

String Sim800Lib::extractPart(int dep, int taille,
                              String &text)  // returns the string contained in text, of
                                             // size taille to part of the dep index
{
  String res = "";
  for (int i = dep; i < (dep + taille); i++) {
    res += text[i];
  }
  return res;
}

boolean Sim800Lib::isKeyWordIn(String content)  // check if the keyword is in the string
{
  if (content.length() < keyWord.length()) {
    return false;
  } else if (content == keyWord) {
    return true;
  } else {
    for (int i = 0; i < content.length(); i++) {
      if (content[i] == keyWord[0]) {
        if (extractPart(i, keyWord.length(), content) == keyWord) return true;
      }
    }
  }
  return false;
}

String Sim800Lib::getPart(int dep, int fin,
                          String &text)  // returns the string contained in the text,
                                         // from the dep index to the fin index
{
  String res = "";
  for (int i = (dep + 1); i < fin; i++) {
    res += text[i];
  }
  return res;
}

boolean Sim800Lib::isMSG(String &text)  // Check that a message is received
{
  for (int i = 0; i < text.length(); i++) {
    if (text[i] == '+') {
      if (extractPart(i, 5, text) == "+CMTI") {
        return true;
      }
    }
  }
  return false;
}

boolean Sim800Lib::isCALL(String &text)  // Check that a call is received
{
  for (int i = 0; i < text.length(); i++) {
    if (text[i] == 'R') {
      if (extractPart(i, 4, text) == "RING") {
        return true;
      }
    }
  }
  return false;
}

String Sim800Lib::get_call_result(boolean _clear)  // private function called in case of call and
                                                   // which returns the number that called
{
  String        rep        = "";
  boolean       hasRespond = false;
  unsigned long startMS    = millis();
  while (millis() - startMS < serverTimeOut) {
    if (mySerial->available() > 0) {
      char c = mySerial->read();
      rep += c;
      mySerial->flush();
      if (is_end(rep)) {
        hasRespond = true;
        goto bailout;
      }
    }
  }
bailout:
  rep.replace("\r\n", " ");
  rep.replace('\0', ' ');
  int taille = rep.length();  //
  int pos[2];
  int j = 0;
  for (int i = 0; i < taille; i++) {
    if (rep[i] == '"') {
      pos[j] = i;
      j++;
    }
  }
  String parsed = getPart(pos[0], pos[1], rep);
  return parsed;
}

void Sim800Lib::reset_module() {
  // on démarre
  Serial.println("RESETING MODULE");
  startModule();
  mySerial->println("AT");
  Serial.println(get_result(true));
  delay(waitingTime);
}

// Private functions to decode messages and calls

void Sim800Lib::get_SMS(int &ret, String number) {
  String        rep     = "";
  unsigned long startMS = millis();
  unsigned long t_rest  = 0;
  while (t_rest < msgTimeOut) {
    if (mySerial->available() > 0) {
      char c = mySerial->read();
      rep += c;
      mySerial->flush();
    }
    t_rest = millis() - startMS;
  }

  String index = "";
  if (rep != "") {
    rep.replace("\r\n", " ");
    rep.replace('\0', ' ');

    int t = rep.length();
    for (int i = 0; i < t; i++) {
      if (rep[i] == ',') {
        index = rep[i + 1];
      }
    }
  }
  // on a ici l'index du dernier msg
  mySerial->println("AT+CMGR=" + index);
  String        _rep     = "";
  unsigned long _startMS = millis();
  unsigned long _t_rest  = 0;
  while (_t_rest < msgTimeOut) {
    if (mySerial->available() > 0) {
      char cc = mySerial->read();
      _rep += cc;
      mySerial->flush();
    }
    _t_rest = millis() - _startMS;
  }

  String h = "", dt = "", sd = "", ctn = "";
  if (_rep != "") {
    _rep.replace("\r\n", " ");
    _rep.replace('\0', ' ');
    _rep.replace("OK", " ");
    /*treat rep*/
    int _pos[10];
    int _j = 0;
    int _t = _rep.length();
    for (int _i = 0; _i < _t; _i++) {
      if (_rep[_i] == '"') {
        _pos[_j] = _i;
        _j++;
      }
    }
    /*end treat*/
    int    ind;
    String s = getPart(_pos[6], _pos[7], _rep);

    int _tt = s.length();
    for (int i = 0; i < _tt; i++) {
      if (s[i] == ',') {
        ind = i;
      }
    }

    dt  = extractPart(0, ind, s);
    h   = extractPart(ind + 1, s.length() - 1 - ind, s);
    sd  = getPart(_pos[2], _pos[3], _rep);
    ctn = getPart(_pos[7], _t, _rep);

    Serial.println("NEW MESSAGE ");
    Serial.println("Sender : " + sd);
    Serial.println("Date : " + dt);
    Serial.println("Hour : " + h);
    Serial.println("Content : " + ctn);
    ret = 2;
    if (sd == number) ret = 3;
  }
}

void Sim800Lib::get_CALL(int &ret, String number) {
  mySerial->println("AT");
  Serial.println(get_result(true));
  delay(waitingTime);

  mySerial->println("AT+CLCC");
  String numberOfCall = get_call_result(true);
  Serial.println("CALL OF : " + numberOfCall);
  ret = 0;
  if (numberOfCall == number) ret = 1;

  mySerial->println("ATH");
  Serial.println(get_result(true));
  delay(waitingTime);
}

String Sim800Lib::HTTPAnswer(uint32_t timeout, bool debug) {
  uint32_t startTime = millis();
  String   response  = "";

  while (millis() - startTime <= timeout || mySerial->available() > 0) {
    if (mySerial->available() > 0) {
      char c = (char)mySerial->read();

      response += c;

      if (debug) Serial.print(c);
    }
    // mySerial->flush();
  }

  return response;
}

String Sim800Lib::HTTPAnswerAction(uint32_t timeout) {
  uint32_t startTime    = millis();
  String   response     = "";
  size_t   pos          = 0;
  bool     ishttpAction = false;
  size_t   count        = 0;

  while (millis() - startTime <= timeout || mySerial->available() > 0) {
    if (mySerial->available() > 0) {
      char c = (char)mySerial->read();
      response += c;
      // if (debug) Serial.print(c);
      if (c == '+' && response[pos - 1] != 'T') ishttpAction = true;

      if (ishttpAction && (c == '\n' || c == '\r\n')) return response;

      pos += 1;
    }
  }

  return response;
}

String Sim800Lib::HTTPAnswerRead(uint32_t timeout) {
  uint32_t startTime  = millis();
  String   response   = "";
  size_t   pos        = 0;
  bool     ishttpRead = false;
  size_t   count      = 0;

  while (millis() - startTime <= timeout || mySerial->available() > 0) {
    if (mySerial->available() > 0) {
      char c = (char)mySerial->read();
      response += c;

      // if (debug)
      // Serial.print(c);

      if (c == '+' && response[pos - 1] != 'T') ishttpRead = true;

      if (ishttpRead && (c == 'K' && response[pos - 1] == 'O')) return response;

      pos += 1;
    }
  }

  return response;
}

// Functions for converting strings to int and inversely
int charToInt(char c) {
  if (c == '0') return 0;
  if (c == '1') return 1;
  if (c == '2') return 2;
  if (c == '3') return 3;
  if (c == '4') return 4;
  if (c == '5') return 5;
  if (c == '6') return 6;
  if (c == '7') return 7;
  if (c == '8') return 8;
  if (c == '9') return 9;
}

char intToChar(int n) {
  if (n == 0) return '0';
  if (n == 1) return '1';
  if (n == 2) return '2';
  if (n == 3) return '3';
  if (n == 4) return '4';
  if (n == 5) return '5';
  if (n == 6) return '6';
  if (n == 7) return '7';
  if (n == 8) return '8';
  if (n == 9) return '9';
}
//pow of n
int pow(int n) {
  int rep = 1;
  for (int i = 0; i < n; i++) {
    rep *= 10;
  }
  return rep;
}

int stringToInt(String s) {
  int n = 0;
  for (int i = s.length() - 1; i >= 0; i--) {
    n += charToInt(s[i]) * pow(s.length() - 1 - i);
  }
  return n;
}

String intToString(int n) {
  String s      = "";
  int    tab[5] = {0};
  int    i      = 0;
  while (n % 10 != n) {
    tab[i] = n % 10;
    i++;
    n /= 10;
  }
  tab[i] = n;

  for (int j = i; j >= 0; j--) s += intToChar(tab[j]);
  return s;
}
