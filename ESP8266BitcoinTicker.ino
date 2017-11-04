/** 
 * ESP8266 Bitcoin ticker   
 * Created by: Steven Smethurst 
 * For more information https://blog.abluestar.com/esp8266-bitcoin-ticker 
 */ 

static const unsigned long SETTING_POLLING_FREQUENCY = 60 * 5 * 1000 ; 
static const char * const SETTING_CURRECNY = "CAD"; 
static const float SETTING_PURCHASED_RATE = 10146.18f ; 
static const float    SETTING_PURCHASED_AMOUNT_IN_BTC =  1.0649218f; 


#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

// WiFiManager
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

// HTTP Client 
#include <ESP8266HTTPClient.h>

// JSON 
#include <ArduinoJson.h>

// OLED
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// SCL GPIO5
// SDA GPIO4
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);

    WiFiManager wifiManager;
    wifiManager.autoConnect("ESP8266 Bitcoin Tracker");
    
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
    Serial.print("IP address: "); Serial.println(WiFi.localIP());

    // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
    display.clearDisplay();    
    display.display();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status

    String url = "http://api.coindesk.com/v1/bpi/currentprice/"+ String( SETTING_CURRECNY ) + ".json" ; 
    Serial.println("FYI: Doing HTTP request. URL=[" + url + "]");
    
    HTTPClient http;  // Declare an object of class HTTPClient
    http.begin(url);  // Specify request destination
    int httpCode = http.GET();// Send the request
    Serial.println("FYI: Request sent, waiting for reply...");
 
    if (httpCode > 0) { //Check the returning code 
      Serial.println("FYI: Got rely, httpCode=" + String( httpCode) );
      String payload = http.getString();   // Get the request response payload
      Serial.println(payload);             // Print the response payload 

      StaticJsonBuffer<1024*2> jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(payload);
      if (root.success()) {
        const char* rate = root["bpi"]["CAD"]["rate_float"] ; 
        const char* rateCode = root["bpi"]["CAD"]["code"] ; 
        const float currentRate = atof( rate ); 
        const float purchasedCurrentValue = currentRate * SETTING_PURCHASED_AMOUNT_IN_BTC ; 
        const float purchasedProfit = (currentRate- SETTING_PURCHASED_RATE) * SETTING_PURCHASED_AMOUNT_IN_BTC ; 
        
        // Debug print to serial 
        Serial.print("FYI: 1 BTC = $");
        Serial.print( rate ) ;
        Serial.print( " " ); 
        Serial.println( rateCode);        
        Serial.print("FYI: purchasedCurrentValue = $" );
        Serial.print( purchasedCurrentValue ) ;
        Serial.print( " (" ); 
        Serial.print( purchasedProfit ) ;
        Serial.print( ") " );         
        Serial.println( rateCode);
        

        // Update the display 
        // Clear the buffer.
        display.clearDisplay();
       
        // text display tests
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(32,10);
        display.print(currentRate, 2);
        // display.setCursor(70,10);
        // display.print(rateCode);

        display.setCursor(32,25);
        display.print(purchasedProfit, 2);
        display.display();        
      } else {
        Serial.println( "Error: Could not decode the JSON payload");
      }
    } 
    Serial.println("FYI: Done with the HTTP request.");
    http.end();   //Close connection 
  }
 
  delay(SETTING_POLLING_FREQUENCY);    
  Serial.println("\n\n");
}
