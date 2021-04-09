#define BLYNK_PRINT Serial
#include "DHT.h"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SimpleDHT.h>
#include <TridentTD_LineNotify.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);


int Relay1 = D5;
int Relay2 = D6;


int setTemp1;
int setTemp2;
bool _button_ma = false;
bool _button_ma1 = false;
bool _button1 = false;
bool _button2 = false;


WidgetLED led1(V30);
WidgetLED led2(V31);


BLYNK_WRITE(V22) //temperature slider in app on pin V22
{
  setTemp1 = param.asInt();
}
BLYNK_WRITE(V23) //temperature slider in app on pin V23
{
  setTemp2 = param.asInt();
}

BLYNK_WRITE(V0)
{
  if (param.asInt() == 1) {

    _button_ma = true;

  } else {

    _button_ma = false;

  }
}

BLYNK_WRITE(V1)
{
  if (param.asInt() == 1) {

    _button_ma1 = true;

  } else {

    _button_ma1 = false;

  }
}

BLYNK_WRITE(V40) {

  if (param.asInt() == 1) {
    _button1 = true;
  } else {
    _button1 = false;
  }
}

BLYNK_WRITE(V41) {

  if (param.asInt() == 1) {
    _button2 = true;
  } else {
    _button2 = false;
  }
}

char auth[] = "PKamgPq_ubER5QbU1DfTiPcReLZ3soH0";
char ssid[] = "iot-2";
char pass[] = "12345678";

#define LINE_TOKEN  "jsEOzo8eNy3n2kn2wDfGx6anhHmIpMNDyUEY5GgFq6k"



BLYNK_CONNECTED() {
  Blynk.syncAll();
}
#define DHT1PIN D4
#define DHT2PIN D3
#define DHT1TYPE DHT11
#define DHT2TYPE DHT11 // probobly not needed to do twice, clean later


DHT dht1(DHT1PIN, DHT1TYPE);
DHT dht2(DHT2PIN, DHT2TYPE);

SimpleDHT11 dht11(DHT1PIN);
SimpleDHT11 dht112(DHT2PIN);

BlynkTimer timer;

void sendsensor1() {
  //DHT11.1
  float h1 = dht1.readHumidity();
  float t1 = dht1.readTemperature();
  t1 = dht1.readTemperature();
  t1 = t1;
  h1 = dht1.readHumidity();
  h1 = h1;
  Blynk.virtualWrite(V11, h1);
  Blynk.virtualWrite(V10, t1);

  //DHT11.2
  float h2 = dht2.readHumidity();
  float t2 = dht2.readTemperature();
  t2 = dht2.readTemperature();
  t2 = t2;
  h2 = dht2.readHumidity();
  h2 = h2;
  Blynk.virtualWrite(V12, h2);
  Blynk.virtualWrite(V13, t2);
}

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode (Relay1, OUTPUT);
  pinMode (Relay2, OUTPUT);
  digitalWrite (Relay1, HIGH);
  digitalWrite (Relay2, HIGH);

  Blynk.begin(auth, ssid, pass, "blynk.honey.co.th", 8080);
  LINE.setToken(LINE_TOKEN);

  dht1.begin();
  dht2.begin();
  timer.setInterval(2000L, sendsensor1); // probobly not needed twice, timers are wierd things... wiki..


}

void loop() {




  // Wait a few seconds between measurements.
  delay(2000);
  float h1 = dht1.readHumidity();
  float t1 = dht1.readTemperature();
  float f1 = dht1.readTemperature(true);

  if (isnan(h1) || isnan(t1) || isnan(f1)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  float hif1 = dht1.computeHeatIndex(f1, h1);
  float hic1 = dht1.computeHeatIndex(t1, h1, false);

  Serial.print(F("Humidity 1 : "));
  Serial.print(h1);
  Serial.print(F("%  Temperature 1 : "));
  Serial.print(t1);
  Serial.print(F("°C \n"));


  float h2 = dht2.readHumidity();
  float t2 = dht2.readTemperature();
  float f2 = dht2.readTemperature(true);

  if (isnan(h2) || isnan(t2) || isnan(f2)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  float hif2 = dht2.computeHeatIndex(f2, h2);
  float hic2 = dht2.computeHeatIndex(t2, h2, false);

  Serial.print(F("Humidity 2 : "));
  Serial.print(h2);
  Serial.print(F("%  Temperature 2 : "));
  Serial.print(t2);
  Serial.print(F("°C\n"));

  //autoตู้อุปกรณ์
  if ( _button_ma == true) {
    if (t1 >= setTemp1 )
    {
      digitalWrite (Relay1, LOW);
      led1.on();
    }
    else if (t1 < setTemp1)
    {
      digitalWrite (Relay1, HIGH);
      led1.off();
    }

  } else {  //manual
    if (_button1 == true) {
      digitalWrite(Relay1, HIGH);
      led1.off();
    } else {
      digitalWrite(Relay1, LOW);
      led1.on();
    }
  }

  //autoโรงเรือน
  if ( _button_ma1 == true) {
    if (t2 >= setTemp2 )
    {
      digitalWrite (Relay2, LOW);
      led2.on();
    }
    else if (t2 < setTemp2)
    {
      digitalWrite (Relay2, HIGH);
      led2.off();
    }

  } else {  //manual
    if (_button2 == true) {
      digitalWrite(Relay2, LOW);
      led2.on();
    } else {
      digitalWrite(Relay2, HIGH);
      led2.off();
    }
  }

  Blynk.run();
  timer.run();
  // read without samples.
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;


  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.print(SimpleDHTErrCode(err));
    Serial.print(","); Serial.println(SimpleDHTErrDuration(err)); delay(1000);
    return;
  }


  // DHT11 sampling rate is 1HZ.
  // Output on LCD
  lcd.setCursor(6, 0);
  lcd.print("House");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Humidity =  ");
  lcd.setCursor(4, 1);
  lcd.print("Temp =  ");
  lcd.setCursor(11, 0);
  lcd.print(humidity + String(" %"));
  lcd.setCursor(11, 1);
  lcd.print(temperature + String(" C"));
  delay(5000);
  lcd.clear();


  if ((err = dht112.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.print(SimpleDHTErrCode(err));
    Serial.print(","); Serial.println(SimpleDHTErrDuration(err)); delay(1000);
    return;
  }

  // DHT11 sampling rate is 1HZ.
  // Output on LCD
  lcd.setCursor(4, 0);
  lcd.print("Equipment");
  lcd.setCursor(5, 1);
  lcd.print("cabinet");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Humidity =  ");
  lcd.setCursor(4, 1);
  lcd.print("Temp =  ");
  lcd.setCursor(11, 0);
  lcd.print(humidity + String(" %"));
  lcd.setCursor(11, 1);
  lcd.print(temperature + String(" C"));
  delay(5000);
  lcd.clear();
  //เงือนไขแจ้งเตือนไลน์โรงเรือน
  if (t1 > 40) {
    LINE.notify("แจ้งเตือน ! อุณหภูมิโรงเรือนสูงกว่า 40 C อุณหภูมิเกินอยู่ที่ " + String(t1) + " C");
  }
  //เงือนไขแจ้งเตือนไลน์ตู้อุปกรณ์
  if (t2 > 40) {
    LINE.notify("แจ้งเตือน ! อุณหภูมิตู้อุปกรณ์สูงกว่า 40 C อุณหภูมิเกินอยู่ที่ " + String(t2) + " C");
  }
}
