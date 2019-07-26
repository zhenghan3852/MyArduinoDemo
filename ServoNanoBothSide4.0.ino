#include <Servo.h>

#define YW 11 //移位
#define FX 10 //分选         //新车与老车不同
#define CAM A2
#define Gray A7
#define Arduino 4 //与mega板通讯的引脚
#define OP 2 //红外
#define Threshold_Gray 180 //灰度传感器阈值，红色约为270，绿色约为250，白色160
#define DQ 9//吊桥舵机 
#define ServoMode 3



int Servo1_Angle1 = 35;
int Servo1_Angle2 = 62;

int Servo2_start = 90;

int Servo2_Angle1 = 90; //初始位置

int Servo2_AngleRed = 0; //最左边的位置
int Servo2_RedReady = 20; //左边准备位置

int Servo2_AngleGreen = 180; //最右边的位置
int Servo2_GreenReady = 180; //右边准备位置

int Pots = 3;

int mode = 0;

int flag = 0; //花盆收集计数

Servo Servo1;
Servo Servo2;
Servo Bri;


void setup() {
  pinMode(YW, OUTPUT);
  pinMode(FX, OUTPUT);
  pinMode(Arduino, OUTPUT);
  pinMode(CAM, INPUT);
  pinMode(ServoMode, INPUT);

  Servo1.attach(YW);
  Servo2.attach(FX);
  Bri.attach(DQ);

  attachInterrupt(0, Change, RISING); //设置中断

  Servo1.write(Servo1_Angle1);
  Servo2.write(Servo2_start);   //舵机复位
  Bri.writeMicroseconds(2500);
  delay(2500);
  Bri.detach();
}

void loop() {
  if(digitalRead(ServoMode)==HIGH){
    delay(5);
    if(digitalRead(ServoMode)==HIGH){
      while(1){
        Leftloop();
        }
      }
    }
  else{
    delay(5);
    if(digitalRead(ServoMode)==LOW){
      while(1){
        Rightloop();
        }
      }
    }
}

void Leftloop() {
  while (1) {
    if (flag != 10) {
      if (mode == 0) {       //红色准备模式
        if (digitalRead(CAM) == HIGH) {
          Servo2.write(Servo2_RedReady);
        }
        else {               //绿色准备模式
          Servo2.write(Servo2_GreenReady);
        }
      }
      else if (mode == 1) {  //执行
        MyServo();
        flag++;
        mode = 0;
      }
    }
    else {
      delay(300);
      Servo2.write(Servo2_start);
      while (1) {
        if (digitalRead(CAM) == HIGH) {
          digitalWrite(Arduino, HIGH);
        }
        else {
          digitalWrite(Arduino, LOW);
        }
      }
    }
  }
}

void Rightloop() {
  while (1) {
    if (flag != 10) {
      if (mode == 0) {       //红色准备模式
        if (digitalRead(CAM) == LOW) {
          Servo2.write(Servo2_RedReady);
        }
        else {               //绿色准备模式
          Servo2.write(Servo2_GreenReady);
        }
      }
      else if (mode == 1) {  //执行
        MyServo2();
        flag++;
        mode = 0;
      }
    }
    else {
      Servo2.write(Servo2_start);
      while (1) {
        if (digitalRead(CAM) == HIGH) {
          digitalWrite(Arduino, HIGH);
        }
        else {
          digitalWrite(Arduino, LOW);
        }
      }
    }
  }
}

void MyServo() {
  if (digitalRead(CAM) == HIGH) {
    Red();
  }

  else if (digitalRead(CAM) == LOW) {
    Green();
  }
}

void MyServo2() {
  if (digitalRead(CAM) == HIGH) {
    Green();
  }

  else if (digitalRead(CAM) == LOW) {
    Red();
  }
}

void FastServo() {
  if (digitalRead(CAM) == HIGH) {
    flag++;
    if (flag >= Pots) {
      digitalWrite(Arduino, HIGH);
    }
    FastRed();
  }

  else if (digitalRead(CAM) == LOW) {
    flag++;
    if (flag >= Pots) {
      digitalWrite(Arduino, HIGH);
    }
    FastGreen();
  }
}

void Green() {
  Servo2.write(Servo2_GreenReady);
  delay(50);

  Slow();
  
  Servo1.write(Servo1_Angle1);
  for (int i = Servo2_GreenReady; i > Servo2_AngleRed ; i--) {
    Servo2.write(i);
    delay(4);
  }
}

void FastGreen() {
  Servo2.write(Servo2_GreenReady);
  delay(50);
}

void Red() {
  Servo2.write(Servo2_AngleRed);
  delay(50);

  Slow();

  Servo1.write(Servo1_Angle1);
  for (int i = Servo2_AngleRed; i < Servo2_AngleGreen; i++) {
    Servo2.write(i);
    delay(4);
  }
}

void FastRed() {
  Servo2.write(Servo2_RedReady);
  delay(50);

  Servo1.write(Servo1_Angle2);
  delay(100);

  Servo1.write(Servo1_Angle1);
  Servo2.write(Servo2_AngleGreen);
  delay(600);

  Servo2.write(Servo2_Angle1);
}

void Slow() {
  int i;
  for (i = Servo1_Angle1; i < Servo1_Angle2; i++) {
    Servo1.write(i);
    delay(12);
  }
}

void Change() {
  delayMicroseconds(1000);
  if (digitalRead(OP) == HIGH) {
    mode = 1;
  }
  else
    mode = 0;
}
