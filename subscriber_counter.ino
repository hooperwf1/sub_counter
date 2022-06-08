#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <YoutubeApi.h>
#include <ArduinoJson.h>

#define SSID_NAME ""
#define PASSWORD ""

#define API_KEY ""
#define CHANNEL_ID ""

WiFiClientSecure cli;
YoutubeApi api(API_KEY, cli);

#define SER 16
#define OE 5
#define RCLK 4
#define SRCLK 0
#define SRCLR 2

// Last bit doesn't matter
char nums[] = {0b001110111, 0b01000001, 0b00111011, 0b01101011, 0b01001101, 0b01101110, 0b01111110, 0b01000011, 0xff, 0b11101111};

void initDigits(){
  pinMode(SER, OUTPUT);
  pinMode(OE, OUTPUT);
  pinMode(RCLK, OUTPUT);
  pinMode(SRCLK, OUTPUT);
  pinMode(SRCLR, OUTPUT);

  digitalWrite(SER, HIGH);
  digitalWrite(OE, LOW);
  digitalWrite(RCLK, LOW);
  digitalWrite(SRCLK, LOW);
  digitalWrite(SRCLR, HIGH);
}

void clearDigits(){
  digitalWrite(SRCLR, LOW);
  delay(1);
  digitalWrite(SRCLR, HIGH);
}

void writeDigit(int n){
  char digit = nums[n];

  for(int i = 0; i < 8; i++){
    if(digit & (1 << i)){
      digitalWrite(SER, HIGH);
    } else {
      digitalWrite(SER, LOW);
    }

    digitalWrite(SRCLK, HIGH);
    delay(1);
    digitalWrite(SRCLK, LOW);
  }   
   
  digitalWrite(RCLK, HIGH);
  delay(1);
  digitalWrite(RCLK, LOW);
}

void setup() {
  initDigits();

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  delay(50);

  WiFi.begin(SSID_NAME, PASSWORD);
  while(WiFi.status() != WL_CONNECTED){
    Serial.print("Connecting...");
    delay(250);
  }
  cli.setInsecure();
}

int count = 0;
void loop() {
  if(api.getChannelStatistics(CHANNEL_ID)){
    count = api.channelStats.subscriberCount;
  }
  
  char digits[8];
  int copy = count;
  for(int i = 0; i < 8; i++){
    digits[7-i] = copy % 10;
    copy /= 10;
  }

  for(int i = 0; i < 8; i++){
    writeDigit(digits[i]);
  }

  // Check once per hour
  delay(1000 * 60 * 60);
}
