#define BLYNK_PRINT Serial
#include "DHT.h"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TridentTD_LineNotify.h>
#include <NewPing.h>

//Ultrasonic1
#define TRIGGER_PIN  D6
#define ECHO_PIN     D5
#define MAX_DISTANCE 400

////Ultrasonic2
//#define TRIGGER_PIN1  D4
//#define ECHO_PIN1    D8
//#define MAX_DISTANCE1 400

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); //วัดระดับน้ำตู้ปลา

//NewPing sonar1(TRIGGER_PIN1, ECHO_PIN1, MAX_DISTANCE1);

int Relay1 = D0;
int Relay2 = D1;
int Relay3 = D2;
int Relay4 = D3;
int Relay5 = D7;

int setTemp;
int setHumid;
int setWarter;

bool _button_ma = false;
bool _button1 = false;
bool _button2 = false;
bool _button3 = false;
bool _button4 = false;
bool _button5 = false;

WidgetLED led1(V30);
WidgetLED led2(V31);
WidgetLED led3(V32);
WidgetLED led4(V33);
WidgetLED led5(V34);

BLYNK_WRITE(V22) //temperature slider in app on pin V22
{
  setWarter = param.asInt();
}
BLYNK_WRITE(V23) //temperature slider in app on pin V23
{
  setHumid = param.asInt();
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
    digitalWrite (Relay1, LOW);
    led1.on();
  } else {
    _button1 = false;
    digitalWrite (Relay1, HIGH);
    led1.off();
  }
}

BLYNK_WRITE(V41) {

  if (param.asInt() == 1) {
    _button2 = true;
    digitalWrite (Relay2, LOW);
    led2.on();
  } else {
    _button2 = false;
    digitalWrite (Relay2, HIGH);
    led2.off();
  }
}

BLYNK_WRITE(V42) {

  if (param.asInt() == 1) {
    _button3 = true;
    digitalWrite (Relay3, LOW);
    led3.on();
  } else {
    _button3 = false;
    digitalWrite (Relay3, HIGH);
    led3.off();
  }
}

BLYNK_WRITE(V43) {

  if (param.asInt() == 1) {
    _button4 = true;
    digitalWrite (Relay4, LOW);
    led4.on();
  } else {
    _button4 = false;
    digitalWrite (Relay4, HIGH);
    led4.off();
  }
}

BLYNK_WRITE(V44) {

  if (param.asInt() == 1) {
    _button5 = true;
    digitalWrite (Relay5, LOW);
    led5.on();
  } else {
    _button5 = false;
    digitalWrite (Relay5, HIGH);
    led4.on();
  }
}


char auth[] = "S16zGrFiUlNTOS4sr6OXHskRitG245Ez";
char ssid[] = "iot-2";
char pass[] = "12345678";

#define LINE_TOKEN  "jsEOzo8eNy3n2kn2wDfGx6anhHmIpMNDyUEY5GgFq6k"

//char server[] = "oasiskit.com";
//int port = 8080;

BLYNK_CONNECTED() {
  Blynk.syncAll();
}

BlynkTimer timer;

void setup() {
  Serial.begin(9600);
  pinMode (Relay1, OUTPUT);
  pinMode (Relay2, OUTPUT);
  pinMode (Relay3, OUTPUT);
  pinMode (Relay4, OUTPUT);
  pinMode (Relay5, OUTPUT);

  digitalWrite (Relay1, HIGH);
  digitalWrite (Relay2, HIGH);
  digitalWrite (Relay3, HIGH);
  digitalWrite (Relay4, HIGH);
  digitalWrite (Relay5, HIGH);

  Blynk.begin(auth, ssid, pass, "blynk.honey.co.th", 8080);
  LINE.setToken(LINE_TOKEN);
  //  dht1.begin();

  //  timer.setInterval(2000L, sendsensor1); // probobly not needed twice, timers are wierd things... wiki..
}

void loop() {
  //Ultrasonic1
  Serial.print("Ping: ");
  Serial.print(sonar.ping_cm());
  Serial.println("cm");
  Blynk.virtualWrite(V50, sonar.ping_cm());

  Blynk.run();
  timer.run();

  //auto
  if ( _button_ma == true) {
    if (sonar.ping_cm() >= setWarter ) {
      digitalWrite (Relay1, HIGH);
      led1.off();
    }
    else if (sonar.ping_cm() < setWarter)
    {
      digitalWrite (Relay1, LOW);
      led1.on();
    }
    if (sonar.ping_cm() >= setWarter ) {
      digitalWrite (Relay2, HIGH);
      led2.off();
    }
    else if (sonar.ping_cm() < setWarter)
    {
      digitalWrite (Relay2, LOW);
      led2.on();
    }
    if (sonar.ping_cm() >= setWarter ) {
      digitalWrite (Relay3, HIGH);
      led3.off();
    }
    else if (sonar.ping_cm() < setWarter)
    {
      digitalWrite (Relay3, LOW);
      led3.on();
    }
    if (sonar.ping_cm() >= setWarter ) {
      digitalWrite (Relay4, HIGH);
      led4.off();
    }
    else if (sonar.ping_cm() < setWarter)
    {
      digitalWrite (Relay4, LOW);
      led4.on();
    }
    if (sonar.ping_cm() >= setWarter ) {
      digitalWrite (Relay5, LOW);
      led5.on();
    }
    else if (sonar.ping_cm() < setWarter)
    {
      digitalWrite (Relay5, HIGH);
      led5.off();
    }

  } else {  //manual
    if (_button1 == true) {
      digitalWrite(Relay1, LOW);
      led1.on();
    } else {
      digitalWrite(Relay1, HIGH);
      led1.off();
    }
    if (_button2 == true) {
      digitalWrite(Relay2, LOW);
      led2.on();
    } else {
      digitalWrite(Relay2, HIGH);
      led2.off();
    }
    if (_button3 == true) {
      digitalWrite(Relay3, LOW);
      led3.on();
    } else {
      digitalWrite(Relay3, HIGH);
      led3.off();
    }
    if (_button4 == true) {
      digitalWrite(Relay4, LOW);
      led4.on();
    } else {
      digitalWrite(Relay4, HIGH);
      led4.off();
    }
    if (_button5 == true) {
      digitalWrite(Relay5, LOW);
      led5.on();
    } else {
      digitalWrite(Relay5, HIGH);
      led5.off();
    }
  }

// //เงือนไขแจ้งเตือนไลน์ค่าวัดระดับน้ำในตู้ปลา
//  if (sonar.ping_cm() > 15) {
//    LINE.notify("แจ้งเตือน ! ระดับน้ำในตู้ปลาลดลงกว่าปกติ" + String(sonar.ping_cm()) + " CM");
//  }

}
