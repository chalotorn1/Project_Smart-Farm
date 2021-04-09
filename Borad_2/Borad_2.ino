#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Servo.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TridentTD_LineNotify.h>
#include <NewPing.h>


//Ultrasonic1
#define TRIGGER_PIN  D1  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     D2  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 400 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); //วัดระดับถังเก็บน้ำ

long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;
boolean ledState = LOW;
float calibrationFactor = 4.5;
volatile byte pulseCount;
byte pulse1Sec = 0;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
void IRAM_ATTR pulseCounter()
{
  pulseCount++;
}

int sensorValue = analogRead(A0);

int setLDR;
Servo servo;
char auth[] = "eAQsAxufonwwntIoF2QDc4ZXmOlTKZ82"; // ใส่รหัส Token Blynk
char ssid[] = "iot-2"; // ใส่ชื่อไวไฟที่บอร์ดจะใช้เชื่อมต่อ
char pass[] = "12345678"; // ใส่รหัสผ่านไวไฟ

#define LINE_TOKEN  "jsEOzo8eNy3n2kn2wDfGx6anhHmIpMNDyUEY5GgFq6k"

bool _button_ma = false;
bool _button1 = false;

WidgetLED led1(V30);

BLYNK_WRITE(V25) //temperature slider in app on pin V22
{
  setLDR = param.asInt();
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


#define SENSOR D4

int Relay1 = D6;

BlynkTimer timer;

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass, "blynk.honey.co.th", 8080);
  LINE.setToken(LINE_TOKEN);
  pinMode(Relay1, OUTPUT);
  digitalWrite (Relay1, HIGH);
  pinMode(SENSOR, INPUT_PULLUP);

  servo.attach(D5);
  servo.write(0);

  attachInterrupt(digitalPinToInterrupt(SENSOR), pulseCounter, FALLING);
}

void loop() {

  //Ultrasonic1
  Serial.print("Ping: ");
  Serial.print(sonar.ping_cm());
  Serial.println("cm");
  Blynk.virtualWrite(V10, sonar.ping_cm());


  //auto
  if ( _button_ma == true) {
    if (analogRead(A0) >= setLDR )
    {
      digitalWrite (Relay1, LOW);
      led1.on();
    }
    else if (analogRead(A0) < setLDR)
    {
      digitalWrite (Relay1, HIGH);
      led1.off();

    }
  } else {  //manual
    if (_button1 == true) {
      digitalWrite(Relay1, LOW);
      led1.on();
    } else {
      digitalWrite(Relay1, HIGH);
      led1.off();
    }
  }


  delay(1000);
  Serial.println(sensorValue);
  delay(500);


  currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    pulse1Sec = pulseCount;
    pulseCount = 0;
    flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) /
               calibrationFactor;
    previousMillis = millis();
    flowMilliLitres = (flowRate / 60) * 1000;
    totalMilliLitres += flowMilliLitres;
    Serial.print("Flow rate: ");
    Serial.print(int(flowRate));
    Serial.print("L/min");
    Serial.print("\t");
    Serial.print("Output Liquid Quantity: ");
    Serial.print(totalMilliLitres);
    Serial.print("mL / ");
    Serial.print(totalMilliLitres / 1000);
    Serial.println("L");
    Blynk.virtualWrite(V2, flowRate);
  }

  //เงือนไขแจ้งเตือนไลน์ค่าวัดระดับน้ำในตู้ปลา
  if (sonar.ping_cm() > 30) {
    LINE.notify("แจ้งเตือน ! ระดับน้ำในถังเก็บน้ำใกล้หมด ระยะน้ำอยู่ที่  " + String(sonar.ping_cm()) + " CM");
  }
  Blynk.run();
  timer.run();
}

BLYNK_WRITE(V3) // Button Widget writes to Virtual Pin V5
{
  servo.write(180);
  delay(100);
  servo.write(0);
  delay(100);
}
