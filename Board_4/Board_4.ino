#include <ESP8266WiFi.h>
#include <Wire.h>
#include <OneWire.h>
#include <SimpleDHT.h>
#include <DallasTemperature.h>
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>
#include <LiquidCrystal_I2C.h>
#include <TridentTD_LineNotify.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

int Relay1 = D7;

int setTemp1;
bool _button_ma = false;
bool _button1 = false;

WidgetLED led1(V45);

#define ONE_WIRE_BUS D4 //กำหนดขาที่จะเชื่อมต่อ Sensor
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
BlynkTimer timer;

float Celsius = 0;
float Fahrenheit = 0;
float  temp = 0;
float calibration_value = 21.34;
int phval = 0;
unsigned long int avgval;
int buffer_arr[10], temph;

BLYNK_WRITE(V22) //temperature slider in app on pin V22
{
  setTemp1 = param.asInt();
}

BLYNK_WRITE(V0)
{
  if (param.asInt() == 1) {

    _button_ma = true;

  } else {

    _button_ma = false;

  }
}

BLYNK_WRITE(V40) {

  if (param.asInt() == 1) {
    _button1 = true;
  } else {
    _button1 = false;
  }
}


char auth[] = "lEU3wdGptZXqdM8rpJJC48zjdY8s6kHa";
char ssid[] = "iot-2";
char pass[] = "12345678";

#define LINE_TOKEN  "jsEOzo8eNy3n2kn2wDfGx6anhHmIpMNDyUEY5GgFq6k"

//#define mqtt_user "user"
//#define mqtt_password "password"






void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass, "blynk.honey.co.th", 8080);
  LINE.setToken(LINE_TOKEN);
  sensors.begin();
  // Setup a function to be called every second
  timer.setInterval(2000L, sendTemps);
  pinMode (Relay1, OUTPUT);
  digitalWrite (Relay1, HIGH);
  lcd.init();
  lcd.backlight();

}


void sendTemps()
{
  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);
  Serial.println(String("Sıcaklik=") + temp + String(" C"));
  Blynk.virtualWrite(V8, temp);
}

void loop() {

  //autoตู้อุปกรณ์
  if ( _button_ma == true) {
    if (temp <= setTemp1 )
    {
      digitalWrite (Relay1, LOW);
      led1.on();
    }
    else if (temp > setTemp1)
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
  
  Blynk.run();
  timer.run();


  // Wait a few seconds between measurements.
  delay(1000);

  Serial.println("Requesting temperatures...");
  sensors.requestTemperatures(); //อ่านข้อมูลจาก library
  Serial.print("Temperature is: ");
  Serial.print(sensors.getTempCByIndex(0)); // แสดงค่า อูณหภูมิ
  Serial.println(" *C");
  delay(1000);


  Celsius = sensors.getTempCByIndex(0);
  Fahrenheit = sensors.toFahrenheit(Celsius);
  lcd.setCursor(3, 0);
  lcd.print("TempWater");
  lcd.setCursor(3, 1);
  lcd.print("= ");
  lcd.print(Celsius);
  lcd.print(" C");
  delay(2000);
  lcd.clear();



  for (int i = 0; i < 10; i++)
  {
    buffer_arr[i] = analogRead(A0);


    delay(30);
  }
  for (int i = 0; i < 9; i++)
  {
    for (int j = i + 1; j < 10; j++)
    {
      if (buffer_arr[i] > buffer_arr[j])
      {
        temph = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temph;
      }
    }
  }
  avgval = 0;
  for (int i = 2; i < 8; i++)
    avgval += buffer_arr[i];
  float volt = (float)avgval * 5.0 / 1024 / 6;
  float ph_act = -5.70 * volt + calibration_value;
  Blynk.virtualWrite(V9, ph_act);

  lcd.setCursor(5, 0);
  lcd.print("PH Val");
  lcd.setCursor(3, 1);
  lcd.print("= ");
  lcd.print(ph_act);
  lcd.print(" PH");
  delay(2000);
  lcd.clear();

 //เงือนไขแจ้งเตือนไลน์ค่า PH
  if (ph_act > 14) {
    LINE.notify("แจ้งเตือน ! ความเป็นกรดด่างของน้ำสูงกว่า 14 PH ค่า PH เกินที่กำหนด " + String(ph_act) + " PH");
  }
  //เงือนไขแจ้งเตือนไลน์อุณหภูมิในน้ำ
  if (temp > 35) {
    LINE.notify("แจ้งเตือน ! อุณหภูมิน้ำในตู้ปลาสูงกว่า 35 C อุณหภูมิเกินที่กำหนด " + String(temp) + " C");
  }


}
