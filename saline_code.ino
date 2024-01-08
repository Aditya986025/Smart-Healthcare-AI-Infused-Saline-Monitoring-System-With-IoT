
#include <LiquidCrystal.h>
#include<SoftwareSerial.h>
#include "HX711.h"
LiquidCrystal lcd(13,12,11,10,9,8);   
//   defining lcd pins ( first is RS pin of LCD(4),second is Enable pin of LCD (6) last 4 digitd are data
//   pin 13 of Aurdino to pin 4 of LCD   RS
//   pin 12 of Aurdino to pin 6 of LCD   E
//   pin 11 of Aurdino to pin 11 of LCD
//   pin 10 of Aurdino to pin 12 of LCD
//   pin 9 of Aurdino to pin 13 of LCD
//   pin 8 of Aurdino to pin 14 of LCD
SoftwareSerial esp(4, 5);  

String apiKey = "0T0UN6LJFYI55O8T";   
String server = "api.thingspeak.com/update?"; // www.example.com
String uri = "apiKey=0T0UN6LJFYI55O8T";// our example is /esppost.php
String pesti="";
String data1; 
byte dat [6];

long writeTime = 17;
long startTime = 0;
long waitTime = 0;


String ssid     = "Sushant";
String password = "12345678";

const char* host = "api.thingspeak.com";
char thingSpeakAddress[] = "api.thingspeak.com";
long reading = 0;
// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;
const int sw1 = 6;
HX711 scale;
bool state,lb,oz;
void setup()
{
  //Serial.begin(57600);
  Serial.begin(9600);      // used for GSM 
  esp.begin(115200 );  
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  pinMode(sw1, INPUT_PULLUP);
  lcd.begin(16, 2);
  lcd.print(" Saline ");
  lcd.setCursor(0,1);
  lcd.print(" Measurement ");
  delay(1000);
  lcd.clear();
  reset();   // function to reset wifi module
  connectWifi(); // connect to user name and password of mobile 
  //---------------------------------------
  gsm_init();
  Serial.println("AT+CNMI=2,2,0,0,0");
  lcd.print("GSM Ready");
  delay(2000);
  //---------------------------------------
}
void reset() 
{
 esp.println("AT+RST");
 delay(1000);
 if(esp.find("OK") ) Serial.println("Module Reset      ");
 lcd.clear();
 lcd.print("Module Reset    ");
 delay(100);
 esp.println("AT+CWMODE=1");
}

void connectWifi() 
{
 lcd.print("Connecting ... ");
 String cmd = "AT+CWJAP=\"" +ssid+"\",\"" + password + "\"\r\n";
 esp.println(cmd);
 //Serial.println(cmd);
 delay(4000);
 if(esp.find("OK")) 
 {
  //Serial.println("Connected!      ");
  lcd.clear();
  lcd.print("Module Connected");
 }
else 
{
 connectWifi();
 //Serial.println("Cannot connect to wifi");
 lcd.clear();
 lcd.print("Cannot connect wifi");
}
}

//////////////////////////////////////////
                                     //  10


///////////////////////////

void loop()
{                                             // loop open
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // api.thingspeak.com
  cmd += "\",80";
  esp.println(cmd);
        
  if(esp.find("Error"))
  {
   //Serial.println("AT+CIPSTART error");
   lcd.print("AT+CIPSTART error");
   return;
  }
  String getStr = "GET /update?api_key=";
  /////////////////////////////////////////////////////////
  if (scale.is_ready())
  {
    lcd.clear();
    //long reading = scale.read();
    long reading = scale.read()-155547;
    reading=reading/440;
    if(reading <= 57)
    {
      reading = 0;
    }
    Serial.print("HX711 reading: ");
    Serial.println(reading);
    lcd.setCursor(0,0);
    lcd.print("LEVEL: ");
    lcd.setCursor(6,0);
    lcd.print(reading);
  //}                         //////////////////////////////////////////////
  

  if(reading >= 101 && reading <= 300)
  { 
      Serial.print("Valu isless than 300");// if open main   1
      tracking();
      delay(400);
      Serial.print("SMS Bottle Half send");
      String getStr = "GET /update?api_key=";
      getStr += apiKey;
      getStr +="&field1=";
      getStr += String(reading);
      getStr += "\r\n";
    
      cmd = "AT+CIPSEND=";
      cmd += String(getStr.length());    
      esp.println(cmd);
      if(esp.find(">"))
      {                                                                // if open 1    2
        esp.print(getStr);
        lcd.clear();
        lcd.print("Data send... ");
        Serial.print("Data send... ");
        delay(500);
        String message = "";
        while(esp.available())
        {                                         // while open   3
          String line = esp.readStringUntil('\n');
          if(line.length() == 1)
          {                                       // if open      4
            message = esp.readStringUntil('\n');   
          }                                       // if close     5 
        }                                         // while close  6 
        Serial.println("message received");
        lcd.print("message received");
     }                                                                   // if close 1    7
     else
     {                                           // else open      8 
       esp.println("AT+CIPCLOSE");
     }                                           // else close     9
                                                                               //  if close main 10
  }
    if(reading >= 57 && reading <= 100)
    {   
      Serial.print("Valu isless than 100");// if open main   1
      tracking2();
      delay(400);
      Serial.print("SMS Empty send");
      String getStr = "GET /update?api_key=";
      getStr += apiKey;
      getStr +="&field2=";
      getStr += String(reading);
      getStr += "\r\n";
    
      cmd = "AT+CIPSEND=";
      cmd += String(getStr.length());    
      esp.println(cmd);
      if(esp.find(">"))
      {                                                //  2
      esp.print(getStr);
      lcd.clear();
      lcd.print("Data send... ");
      delay(500);
      String message = "";
      while(esp.available())
      {                                                //  3
        String line = esp.readStringUntil('\n');
        if(line.length() == 1)
        {                                              //  4
          message = esp.readStringUntil('\n');   
        }                                              //  5
      }                                                //  6
      Serial.println("message received");
      lcd.print("message received");
     }                                                 //  7
     else
     {                                                 //   8
       esp.println("AT+CIPCLOSE");
     }                                                 //  9
  }                                                    //  10
  }
  delay(1500);
  
}                  // loop close


//-------------------------------------------------
void gsm_init()
{
  lcd.clear();
  lcd.print("Finding GSM Module..");
  boolean at_flag=1;
  while(at_flag)
  {
    Serial.println("AT");
    while(Serial.available()>0)
    {
      if(Serial.find("OK"))
      at_flag=0;
    }
    
    delay(1000);
  }

  lcd.clear();
  lcd.print("GSM Connected..");
  delay(1000);
  boolean echo_flag=1;
  while(echo_flag)
  {
    Serial.println("ATE0");
    while(Serial.available()>0)
    {
      if(Serial.find("OK"))
      echo_flag=0;
    }
    delay(1000);
  }

  lcd.clear();
  lcd.print("Finding Network..");
  boolean net_flag=1;
  while(net_flag)
  {
    Serial.println("AT+CPIN?");
    while(Serial.available()>0)
    {
      if(Serial.find("+CPIN: READY"))
      net_flag=0;
    }
    delay(1000);
  }
  lcd.clear();
  lcd.print("Network Found..");
  delay(1000);
  lcd.clear();

}

void tracking()
{
    //init_sms();
    Serial.println("AT+CMGF=1");
    delay(1000);
    Serial.println("AT+CMGS=\"+917350142430\"\r");   // use your 10 digit cell no. here
    delay(1000);
    Serial.println("emergency message");
    Serial.println("Saline Level Half");
    Serial.println((char)26);// ASCII code of CTRL+Z
    Serial.write(26);
    delay(2000);    
}
void tracking2()
{
    Serial.println("AT+CMGF=1");
    delay(1000);
    Serial.println("AT+CMGS=\"+917350142430\"\r");   // use your 10 digit cell no. here
    delay(1000);
    Serial.println("emergency message");
    Serial.println("Saline Empty");
    Serial.println((char)26);// ASCII code of CTRL+Z
    delay(2000);
    
}
