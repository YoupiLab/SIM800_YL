#ifndef Sim800Lib_h
#define sim800Lib_h

#include <Arduino.h>
#include <SoftwareSerial.h>

struct HTTPResponse {
  int    status   = 0;
  int    size     = 0;
  String response = "";
};

class Sim800Lib {
 public:
  Sim800Lib(int rx, int tx);
  void    setWaitingTime(int wTime);
  int     getWaitingTime();
  void    setServerTimeOut(int outTime);
  int     getServerTimeOut();
  void    setResetTime(int rTime);
  int     getResetTime();
  String  getKeyWord();
  void    setKeyWord(String word);
  int     getMsgCount();
  int     getMaxiMsgCount();
  int     getRemainingMessage(); //
  void    startModule();
  void    sendSMS(String number, String msg);
  void    makeCall(String number);
  int     inListening(String number = "");
  String  getUSSDRequest(String request);
  String  getAllMsg();
  String  getMsgContent(int n);
  String  getMsgSender(int n);
  String  getMsgDate(int n);
  String  getMsgHour(int n);
  String  getLastMsgContent();
  String  getLastMsgSender();
  String  getLastMsgDate();
  String  getLastMsgHour();
  void    deleteMsg(int n);
  void    deleteAllMsg();
  boolean isKeyWordInMsg(int n);
  boolean isKeyWordInLastMsg();

  // gprs functions public functions
  uint32_t     HTTPRequestTimeout = 10000L;
  String       getAPNInfos();
  boolean      isAPNInfosSet(String apn);
  void         setAPNInfos(String apn, String user, String pwd);
  HTTPResponse sendRequest(String& url, int type, String contentType);
  HTTPResponse GET(String& url, String contentType = "");
  HTTPResponse POST(String& url, String contentType = "");
  // gprs

 private:
  int             Rx;
  int             Tx;
  SoftwareSerial* mySerial;
  String          chiefNumber;
  String          keyWord;
  int             msgCount;
  String          rep = "";
  //
  int           lost_compteur;
  int           reset_compteur;
  unsigned long start_time;
  unsigned long MAX_TIME;
  String        QUERY_DELAY;
  int           RESET_COUNT;
  //
  unsigned long waitingTime;
  unsigned long serverTimeOut;
  unsigned long msgTimeOut;
  unsigned long REPOP_TIME;

  // gprs

  // gprs

  void   start();
  String get_result(boolean _clear);
  String get_send_result(boolean _clear);
  String get_call_result(boolean _clear);
  void   restart_module();
  void   reset_module();
  /*void clearBuffer(boolean _clear);*/
  void    get_SMS(int& ret, String number);
  void    get_CALL(int& ret, String number);
  String  getPart(int dep, int fin, String& text);
  String  extractPart(int dep, int taille, String& text);
  boolean isMSG(String& text);
  boolean isCALL(String& text);
  boolean isKeyWordIn(String content);
  boolean is_end(String& rep);
  int     STARTS_WITH(int dep, String& key, String& text);
  String  getFeedBack(String& rep);
  String  get_action_result(boolean _clear);
  boolean is_action_end(String& rep);

  // gprs private functions
  String  get_result(boolean _clear, uint32_t timeout);
  String  extractAPNInfos(String& text);
  boolean isHTTP(String& text);
  String  HTTPAnswer(uint32_t timeout, bool debug = false);
  String  HTTPAnswerAction(uint32_t timeout);
  String  HTTPAnswerRead(uint32_t timeout);
  void    workOnHTTPAction(String response, int& code, int& sz);
  void    workOnHTTPRead(String response, String& serverRes, int sz);
  // gprs
};

int    charToInt(char c);
char   intToChar(int n);
int    pow(int n);
int    stringToInt(String s);
String intToString(int n);

#endif
