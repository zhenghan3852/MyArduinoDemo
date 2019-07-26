#include "Adafruit_VL53L0X.h"
#include "DFRobot_I2CMultiplexer.h"

#define Start 5

#define N1 6
#define F1 7
#define N2 8
#define F2 9

#define LaserMode 10

int distance1;
int distance2;
int distance3;
int distance4;

int Yuzhi1 = 139;
int Yuzhi2 = 165;
int Yuzhi3 = 107;
int Yuzhi4 = 100;
int Far = 700;

int wc = 10;


DFRobot_I2CMultiplexer I2CMulti(0x70);
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
void(*resetFunc)(void) = 0;

void setup() {
  pinMode(N1, OUTPUT);
  pinMode(N2, OUTPUT);
  pinMode(F1, OUTPUT);
  pinMode(F2, OUTPUT);

  pinMode(LaserMode, INPUT);

  Serial.begin(9600);

  pinMode(Start, INPUT);  //接受Mega发送的信号
  //-------------激光初始化----------------
  I2CMulti.selectPort(1);       //Select Port 1
  Serial.println("SelectPort1");
  if (!lox.begin()) {
    Serial.println("01Fail");
    resetFunc();
  }
  I2CMulti.selectPort(2);      //Select Port 2
  Serial.println("SelectPort2");
  if (!lox.begin()) {
    Serial.println("02Fail");
    resetFunc();
  }
  I2CMulti.selectPort(3);       //Select Port 3
  Serial.println("SelectPort3");
  if (!lox.begin()) {
    Serial.println("03Fail");
    resetFunc();
  }
  I2CMulti.selectPort(7);      //Select Port 4
  Serial.println("SelectPort4");
  if (!lox.begin()) {
    Serial.println("04Fail");
    resetFunc();
  }                               //*/

  Serial.println("Success");
}

void loop() {
  if (digitalRead(LaserMode) == HIGH) { //开关断开，左边出发
    delay(5);
    if (digitalRead(LaserMode) == HIGH) {
      while (1) {
        Laser12();
      }
    }
  }
  else {                             //开关闭合，右边出发
    if (digitalRead(LaserMode) == LOW) {
      delay(5);
      if (digitalRead(LaserMode) == LOW) {
        while (1) {
          Laser34();
        }
      }
    }
  }
}

void Laser12() {
  VL53L0X_RangingMeasurementData_t measure;
  /*-------------------select port 0 and code----------------------*/
  I2CMulti.selectPort(1);
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    distance1 = measure.RangeMilliMeter;
    Serial.print("distance1="); Serial.println(distance1);
  }
  /*---------------------------select port 3 and code---------------------------*/
  I2CMulti.selectPort(2);
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    distance2 = measure.RangeMilliMeter;
    Serial.print("distance2="); Serial.println(distance2);
  }
  Combo();
}

void Laser34() {
  VL53L0X_RangingMeasurementData_t measure;
  /*---------------------------select port 5 and code---------------------------*/
  I2CMulti.selectPort(3);
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    distance3 = measure.RangeMilliMeter;
    Serial.print("distance3="); Serial.println(distance3);
  }
  /*---------------------------select port 7 and code---------------------------*/
  I2CMulti.selectPort(7);
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    distance4 = measure.RangeMilliMeter;
    Serial.print("distance4="); Serial.println(distance4);
  }
  Combo2();
}

void Combo() {
  if (distance1 > Yuzhi1 + wc) {  //右下模块距离墙壁过远
    digitalWrite(N1, LOW);
    digitalWrite(F1, HIGH);
  }
  else if (distance1 < Yuzhi1 - wc) { //右下模块距离墙壁过近
    digitalWrite(N1, HIGH);
    digitalWrite(F1, LOW);
  }
  else {
    digitalWrite(N1, LOW);
    digitalWrite(F1, LOW);
  }

  if (distance2 > Yuzhi2 + wc) { //左下模块距离墙壁过远
    digitalWrite(N2, LOW);
    digitalWrite(F2, HIGH);
  }
  else if (distance2 < Yuzhi2 - wc) { //左下模块距离墙壁过近
    digitalWrite(N2, HIGH);
    digitalWrite(F2, LOW);
  }
  else {
    digitalWrite(N2, LOW);
    digitalWrite(F2, LOW);
  }

  if (distance1 > Far && distance2 > Far) {
    digitalWrite(N1, HIGH);
    digitalWrite(F1, HIGH);
    digitalWrite(N2, HIGH);
    digitalWrite(F2, HIGH);
  }
}

void Combo2() {
  if (distance3 > Yuzhi3 + wc) { //右上模块距离墙壁过远
    digitalWrite(N1, HIGH);
    digitalWrite(F1, LOW);
  }
  else if (distance3 < Yuzhi3 - wc) { //右上模块距离墙壁过近
    digitalWrite(N1, LOW);
    digitalWrite(F1, HIGH);
  }
  else {
    digitalWrite(N1, LOW);
    digitalWrite(F1, LOW);
  }

  if (distance4 > Yuzhi4 + wc) { //左上模块距离墙壁过远
    digitalWrite(N2, HIGH);
    digitalWrite(F2, LOW);
  }
  else if (distance4 < Yuzhi4 - wc) { //左上模块距离墙壁过近
    digitalWrite(N2, LOW);
    digitalWrite(F2, HIGH);
  }
  else {
    digitalWrite(N2, LOW);
    digitalWrite(F2, LOW);
  }

  if (distance3 > Far && distance4 > Far) {
    digitalWrite(N1, HIGH);
    digitalWrite(F1, HIGH);
    digitalWrite(N2, HIGH);
    digitalWrite(F2, HIGH);
  }
}
