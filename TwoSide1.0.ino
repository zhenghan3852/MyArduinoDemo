#include<Servo.h>
#include <MsTimer2.h> 
volatile int SPD = 100;       //速度延时
int Roll = 3200;    //摇杆一圈步数
int Step = 1;      //定义调整步数
int reSTP = 48;    //释放时的步数
int d[3];           //定义数组，存放四个数据
int gold = 0;
volatile int Pots = 0;
volatile int mode = 0;
int countSTP = 3000;
int CROSSLEFTSTEP = 30000; //定义向左斜穿的步数
int CrashWallStep = 40;      //From 15

int count = 0;      //行走计数

int en = 1;
/*--------------------------IO口宏定义-------------------------------*/
#define DIR1 41
#define STP1 43
#define EN1 45//1号电机（右后）

#define EN2 51
#define STP2 49
#define DIR2 47//2号电机（右前）

#define EN3 35
#define STP3 37
#define DIR3 39//3号电机（左前）

#define DIR4 25
#define STP4 27
#define EN4 23//4号电机（左后）

#define EN6 29
#define STP6 31
#define DIR6 33

#define EN5 48
#define STP5 46
#define DIR5 44  //左摇臂电机


#define DIS1F 22
#define DIS1N 24 //右边近端和远端信号口 

#define DIS2F 32
#define DIS2N 30 //左边近端和远端信号口

#define Nano 6  //Nano的高电平
#define OP2 2  //中断口接入红外
#define Laser 7  //与激光nano之间的通讯
#define RunMode 12   //模式开关断开，12号引脚高电平，走左边；模式开关闭合，12号引脚低电平走右边


#define Pro1 34
#define Pro2 36   //两行程开关引脚

#define S1 50     //开门舵机
#define S2 52

#define Moto1 4
#define Moto2 5      //直流电机接口

#define Moto3 8
#define Moto4 9

Servo Servo1;
Servo Servo2;
/*EN为蓝色，STP为黄色，DIR为绿色*/
/*--------------------------初始化-------------------------------*/
void setup() {
  pinMode(DIR1, OUTPUT);
  pinMode(STP1, OUTPUT);
  pinMode(EN1, OUTPUT); //1号电机（右后）

  pinMode(DIR2, OUTPUT);
  pinMode(STP2, OUTPUT);
  pinMode(EN2, OUTPUT); //2号电机（右后）

  pinMode(DIR3, OUTPUT);
  pinMode(STP3, OUTPUT);
  pinMode(EN3, OUTPUT); //3号电机（右后）

  pinMode(DIR4, OUTPUT);
  pinMode(STP4, OUTPUT);
  pinMode(EN4, OUTPUT); //4号电机（右后）

  pinMode(DIR5, OUTPUT);
  pinMode(STP5, OUTPUT);
  pinMode(EN5, OUTPUT); //4号电机（右后）

  pinMode(Nano, INPUT); //ServoNano发出的信号
  pinMode(RunMode, INPUT); //模式开关信号
  pinMode(Laser, OUTPUT); //向激光nano发出信号

  pinMode(OP2, INPUT); //OP2接受红外信号

  pinMode(DIS2F, INPUT);
  pinMode(DIS2N, INPUT);
  pinMode(DIS1F, INPUT);
  pinMode(DIS1N, INPUT);

  pinMode(Pro1, INPUT);
  pinMode(Pro2, INPUT);  //行程开关引脚输入输出

  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);

  pinMode(Moto1, OUTPUT);
  pinMode(Moto2, OUTPUT);
  pinMode(Moto3, OUTPUT);
  pinMode(Moto4, OUTPUT);

  Servo1.attach(S1);
  Servo2.attach(S2);

  attachInterrupt(0, Change, LOW); //中断有时会去掉
  MsTimer2::set(6000, NoGold);

  while (1) {   //初始化舵机复位
    if (digitalRead(RunMode) == HIGH) {  //左边出发时，舵机复位到0
      delay(5);    //去抖
      if (digitalRead(RunMode) == HIGH) {
        Servo1.write(0);
        Servo2.write(0);   //开门舵机复位
        delay(200);
        break;
      }
    }
    else if (digitalRead(RunMode) == LOW) {  //右边出发时，舵机复位到180
      delay(5);   //去抖
      if (digitalRead(RunMode) == LOW) {
        Servo1.write(160);
        Servo2.write(160);   //开门舵机复位
        delay(200);
        break;
      }
    }
  }
}
/*--------------------------子程序-------------------------------*/
//------运动程序--------
/*前进*/
void NoGold(){
  for (int i = 0; i < 10; ++i)
  {
    FastLongLeft();
  }
  for (int i = 0; i < 50; ++i)
  {
    FastLongLeft2();
  }
  MsTimer2::stop();
  while(1)
    ;
  
}
void Forward() {
  int x;

  digitalWrite(EN1, LOW);
  digitalWrite(EN2, HIGH);
  digitalWrite(EN3, LOW);
  digitalWrite(EN4, HIGH);
  digitalWrite(EN5, HIGH);

  digitalWrite(DIR2, LOW);
  digitalWrite(DIR4, HIGH); //2.4号电机旋转

  for (x = 0; x < 600; x++) {
    digitalWrite(STP2, HIGH);
    digitalWrite(STP4, HIGH);
    delayMicroseconds(SPD);

    digitalWrite(STP2, LOW);
    digitalWrite(STP4, LOW);
    delayMicroseconds(SPD);
  }
}
/*后退*/
void Back() {
  int x;

  digitalWrite(EN1, LOW);
  digitalWrite(EN2, HIGH);
  digitalWrite(EN3, LOW);
  digitalWrite(EN4, HIGH);
  digitalWrite(EN5, HIGH);

  digitalWrite(DIR2, HIGH);
  digitalWrite(DIR4, LOW); //2.4号电机旋转

  for (x = 0; x < 600; x++) {
    digitalWrite(STP2, HIGH);
    digitalWrite(STP4, HIGH);
    delayMicroseconds(SPD);

    digitalWrite(STP2, LOW);
    digitalWrite(STP4, LOW);
    delayMicroseconds(SPD);
  }
}

void FastBack() {
  int x;

  digitalWrite(EN1, LOW);
  digitalWrite(EN2, HIGH);
  digitalWrite(EN3, LOW);
  digitalWrite(EN4, HIGH);
  digitalWrite(EN5, HIGH);

  digitalWrite(DIR2, HIGH);
  digitalWrite(DIR4, LOW); //2.4号电机旋转

  for (x = 0; x < 600; x++) {
    digitalWrite(STP2, HIGH);
    digitalWrite(STP4, HIGH);
    delayMicroseconds(SPD);

    digitalWrite(STP2, LOW);
    digitalWrite(STP4, LOW);
    delayMicroseconds(SPD);
  }
}
void CrossLeft() { //完成花盆收集之后，向左斜侧行走

  int period = 100;
  digitalWrite(EN1, HIGH);
  digitalWrite(EN3, HIGH);
  digitalWrite(EN2, HIGH);
  digitalWrite(EN4, HIGH);

  digitalWrite(DIR1, HIGH);
  digitalWrite(DIR3, LOW); //1.3号电机旋转方向
  digitalWrite(DIR2, LOW);
  digitalWrite(DIR4, HIGH);

  for (int x = 0; x < CROSSLEFTSTEP; x++) {
    if (period > 30 && x % 50 == 0) {
      period--;
    }

    d[0] = digitalRead(DIS2F);
    d[1] = digitalRead(DIS2N);
    d[2] = digitalRead(DIS1F);
    d[3] = digitalRead(DIS1N);

    if (d[0] && d[1] && d[2] && d[3])
    {
      for (int i = 0; i < CrashWallStep; ++i)
      {
        Forward();
      }
      break;
    }
    digitalWrite(STP1, HIGH);
    digitalWrite(STP3, HIGH);
    digitalWrite(STP2, HIGH);
    digitalWrite(STP4, HIGH);
    delayMicroseconds(period);

    digitalWrite(STP1, LOW);
    digitalWrite(STP3, LOW);
    digitalWrite(STP2, LOW);
    digitalWrite(STP4, LOW);
    delayMicroseconds(period);

    digitalWrite(STP1, HIGH);
    digitalWrite(STP3, HIGH);
    delayMicroseconds(period);

    digitalWrite(STP1, LOW);
    digitalWrite(STP3, LOW);
    delayMicroseconds(period);
  }
}

void CrossRight() { //完成花盆收集之后，向右斜侧行走

  int period = 100;
  digitalWrite(EN1, HIGH);
  digitalWrite(EN3, HIGH);
  digitalWrite(EN2, HIGH);
  digitalWrite(EN4, HIGH);

  digitalWrite(DIR1, HIGH);
  digitalWrite(DIR3, LOW); //1.2.3.4号电机旋转方向
  digitalWrite(DIR2, HIGH);
  digitalWrite(DIR4, LOW);

  for (int x = 0; x < CROSSLEFTSTEP; x++) {
    if (period > 30 && x % 50 == 0) {
      period--;
    }

    d[0] = digitalRead(DIS2F);
    d[1] = digitalRead(DIS2N);
    d[2] = digitalRead(DIS1F);
    d[3] = digitalRead(DIS1N);

    if (d[0] && d[1] && d[2] && d[3])
    {
      for (int i = 0; i < CrashWallStep; ++i)
      {
        Back();
      }
      break;
    }
    digitalWrite(STP1, HIGH);
    digitalWrite(STP3, HIGH);
    digitalWrite(STP2, HIGH);
    digitalWrite(STP4, HIGH);
    delayMicroseconds(period);

    digitalWrite(STP1, LOW);
    digitalWrite(STP3, LOW);
    digitalWrite(STP2, LOW);
    digitalWrite(STP4, LOW);
    delayMicroseconds(period);

    digitalWrite(STP1, HIGH);
    digitalWrite(STP3, HIGH);
    delayMicroseconds(period);

    digitalWrite(STP1, LOW);
    digitalWrite(STP3, LOW);
    delayMicroseconds(period);
  }
}
//------抓取程序--------
/*双摇杆运动*/
void Catch() {
  int x;

  digitalWrite(EN5, HIGH);

  digitalWrite(DIR5, HIGH);

  for (x = 0; x < 600; x++) {
    digitalWrite(STP5, HIGH);
    delayMicroseconds(SPD);

    digitalWrite(STP5, LOW);
    delayMicroseconds(SPD);
  }
}
/*边抓边走*/
void CatchAndRun() {  //有各种摇杆电机相对于行走电机的速度
  int x;

  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, LOW);
  digitalWrite(EN3, HIGH);
  digitalWrite(EN4, LOW);
  digitalWrite(EN5, HIGH);

  digitalWrite(DIR1, LOW);
  digitalWrite(DIR3, HIGH); //1.3号电机旋转方向

  digitalWrite(DIR5, HIGH);
  digitalWrite(DIR6, LOW);


  for (x = 0; x < Roll; x++) {
    //-----------车运动的三倍速（车速为SPD）-------------
    /*digitalWrite(STP1,HIGH);
      digitalWrite(STP3,HIGH);
      digitalWrite(STP5,HIGH);
      digitalWrite(STP6,HIGH);
      delayMicroseconds(SPD/3);

      digitalWrite(STP5,LOW);
      digitalWrite(STP6,LOW);
      delayMicroseconds(SPD/3);

      digitalWrite(STP1,LOW);
      digitalWrite(STP3,LOW);
      digitalWrite(STP5,HIGH);
      digitalWrite(STP6,HIGH);
      delayMicroseconds(SPD/3);

      digitalWrite(STP1,LOW);
      digitalWrite(STP3,LOW);
      digitalWrite(STP5,LOW);
      digitalWrite(STP6,LOW);
      delayMicroseconds(SPD/3);

      digitalWrite(STP5,HIGH);
      digitalWrite(STP6,HIGH);
      delayMicroseconds(SPD/3);

      digitalWrite(STP5,LOW);
      digitalWrite(STP6,LOW);
      delayMicroseconds(SPD/3);*/

    //-----------车运动的两倍速（车速为SPD）-------------
    /*digitalWrite(STP1,HIGH);
      digitalWrite(STP3,HIGH);
      digitalWrite(STP5,HIGH);
      digitalWrite(STP6,HIGH);
      delayMicroseconds(SPD/2);

      digitalWrite(STP5,LOW);
      digitalWrite(STP6,LOW);
      delayMicroseconds(SPD/2);

      digitalWrite(STP1,LOW);
      digitalWrite(STP3,LOW);
      digitalWrite(STP5,HIGH);
      digitalWrite(STP6,HIGH);
      delayMicroseconds(SPD/2);

      digitalWrite(STP5,LOW);
      digitalWrite(STP6,LOW);
      delayMicroseconds(SPD/2);*/
    //-----------车运动的一倍速（车速为SPD）-------------
    digitalWrite(STP1, HIGH);
    digitalWrite(STP3, HIGH);
    digitalWrite(STP5, HIGH);
    delayMicroseconds(SPD);

    digitalWrite(STP1, LOW);
    digitalWrite(STP3, LOW);
    digitalWrite(STP5, LOW);
    delayMicroseconds(SPD);
    //-----------车运动的0.5倍速（车速为SPD）-------------
    /*digitalWrite(STP1,HIGH);
      digitalWrite(STP3,HIGH);
      digitalWrite(STP5,HIGH);
      digitalWrite(STP6,HIGH);
      delayMicroseconds(SPD);

      digitalWrite(STP1,LOW);
      digitalWrite(STP3,LOW);
      delayMicroseconds(SPD);

      digitalWrite(STP1,HIGH);
      digitalWrite(STP3,HIGH);
      digitalWrite(STP5,LOW);
      digitalWrite(STP6,LOW);
      delayMicroseconds(SPD);

      digitalWrite(STP1,LOW);
      digitalWrite(STP3,LOW);
      delayMicroseconds(SPD);*/
    //-----------车运动的1/4倍速（车速为SPD）-------------
    /*digitalWrite(STP1,HIGH);
      digitalWrite(STP3,HIGH);
      digitalWrite(STP5,HIGH);
      digitalWrite(STP6,HIGH);
      delayMicroseconds(SPD);

      digitalWrite(STP1,LOW);
      digitalWrite(STP3,LOW);
      delayMicroseconds(SPD);

      digitalWrite(STP1,HIGH);
      digitalWrite(STP3,HIGH);
      delayMicroseconds(SPD);

      digitalWrite(STP1,LOW);
      digitalWrite(STP3,LOW);
      delayMicroseconds(SPD);

      digitalWrite(STP1,HIGH);
      digitalWrite(STP3,HIGH);
      digitalWrite(STP5,LOW);
      digitalWrite(STP6,LOW);
      delayMicroseconds(SPD);

      digitalWrite(STP1,LOW);
      digitalWrite(STP3,LOW);
      digitalWrite(STP5,LOW);
      digitalWrite(STP6,LOW);
      delayMicroseconds(SPD);*/
  }
}

void SlowHalfCatch() {     //稳定型的金花盆收取，速度较慢
  int x;
  delayMicroseconds(1000);
  if (digitalRead(OP2) == LOW) {
    digitalWrite(EN1, HIGH);
    digitalWrite(EN2, LOW);
    digitalWrite(EN3, HIGH);
    digitalWrite(EN4, LOW);
    digitalWrite(EN5, HIGH);
    digitalWrite(EN6, HIGH);

    digitalWrite(DIR1, LOW);
    digitalWrite(DIR3, HIGH); //1.3号电机旋转方向
    digitalWrite(DIR5, HIGH);
    digitalWrite(DIR6, HIGH);

    for (x = 0; x < 1000; x++) { //300步的小前进，以确定盆能进入到爪内
      digitalWrite(STP1, HIGH);  //一倍速度
      digitalWrite(STP3, HIGH);
      delayMicroseconds(SPD);

      digitalWrite(STP1, LOW);
      digitalWrite(STP3, LOW);
      delayMicroseconds(SPD);
    }

    for (x = 0; x < 1300; x++) {
      digitalWrite(STP1, HIGH); //0.5倍速，1000步，用低速让花盆能略微抬起，不至于直接进行高速
      digitalWrite(STP3, HIGH);
      digitalWrite(STP5, HIGH);
      digitalWrite(STP6, HIGH);
      delayMicroseconds(SPD);

      digitalWrite(STP1, LOW);
      digitalWrite(STP3, LOW);
      delayMicroseconds(SPD);

      digitalWrite(STP1, HIGH);
      digitalWrite(STP3, HIGH);
      digitalWrite(STP5, LOW);
      digitalWrite(STP6, LOW);
      delayMicroseconds(SPD);

      digitalWrite(STP1, LOW);
      digitalWrite(STP3, LOW);
      delayMicroseconds(SPD);
    }
    Pots = 1;
  }
}

void HalfRelease() {     //金花盆的释放，一半释放
  int x;

  digitalWrite(EN1, LOW);
  digitalWrite(EN2, LOW);
  digitalWrite(EN3, LOW);
  digitalWrite(EN4, LOW);
  digitalWrite(EN5, HIGH);
  digitalWrite(EN6, HIGH);

  digitalWrite(DIR1, LOW);
  digitalWrite(DIR2, LOW);
  digitalWrite(DIR3, LOW);
  digitalWrite(DIR4, LOW);
  digitalWrite(DIR5, LOW);
  digitalWrite(DIR6, LOW);

  for (x = 0; x < 1200; x++) {
    digitalWrite(STP5, HIGH);
    digitalWrite(STP6, HIGH);
    delayMicroseconds(SPD * 2);

    digitalWrite(STP5, LOW);
    digitalWrite(STP6, LOW);
    delayMicroseconds(SPD * 2);
  }
}

void Shut() {                     //所有电机全部所锁死
  digitalWrite(EN1, LOW);
  digitalWrite(EN2, LOW);
  digitalWrite(EN3, LOW);
  digitalWrite(EN4, LOW);
  digitalWrite(EN5, LOW);
  digitalWrite(EN6, LOW);
}
/*逐渐加速起步*/
void Speedup() {
  int x;

  digitalWrite(EN1, HIGH);
  digitalWrite(EN3, HIGH);

  digitalWrite(DIR1, LOW);
  digitalWrite(DIR3, HIGH);

  for (x = 0; x < 600; x++) {
    digitalWrite(STP1, HIGH);
    digitalWrite(STP3, HIGH);
    delayMicroseconds(200);
    digitalWrite(STP1, LOW);
    digitalWrite(STP3, LOW);
    delayMicroseconds(200);
  }
  for (x = 0; x < 600; x++) {
    digitalWrite(STP1, HIGH);
    digitalWrite(STP3, HIGH);
    delayMicroseconds(150);
    digitalWrite(STP1, LOW);
    digitalWrite(STP3, LOW);
    delayMicroseconds(150);
  }
  for (x = 0; x < 600; x++) {
    digitalWrite(STP1, HIGH);
    digitalWrite(STP3, HIGH);
    delayMicroseconds(100);
    digitalWrite(STP1, LOW);
    digitalWrite(STP3, LOW);
    delayMicroseconds(100);
  }
}

void Left() {     //向左行驶（无摇杆动作）
  int x;

  digitalWrite(EN1, HIGH);
  digitalWrite(EN3, HIGH);

  digitalWrite(DIR1, HIGH);
  digitalWrite(DIR3, LOW); //1.3号电机旋转方向

  for (x = 0; x < Step; x++) {
    digitalWrite(STP1, HIGH);
    digitalWrite(STP3, HIGH);
    delayMicroseconds(SPD);

    digitalWrite(STP1, LOW);
    digitalWrite(STP3, LOW);
    delayMicroseconds(SPD);
  }
}

void FastLongLeft() {
  int x;

  digitalWrite(EN1, HIGH);
  digitalWrite(EN3, HIGH);

  digitalWrite(DIR1, HIGH);
  digitalWrite(DIR3, LOW); //1.3号电机旋转方向

  for (x = 0; x < 500; x++) {
    digitalWrite(STP1, HIGH);
    digitalWrite(STP3, HIGH);
    delayMicroseconds(30);

    digitalWrite(STP1, LOW);
    digitalWrite(STP3, LOW);
    delayMicroseconds(30);
  }
}

void FastLongLeft1() {    //起步时速度较慢的左行
  int x;

  digitalWrite(EN1, HIGH);
  digitalWrite(EN3, HIGH);

  digitalWrite(DIR1, HIGH);
  digitalWrite(DIR3, LOW); //1.3号电机旋转方向

  for (x = 0; x < 500; x++) {
    digitalWrite(STP1, HIGH);
    digitalWrite(STP3, HIGH);
    delayMicroseconds(SPD);

    digitalWrite(STP1, LOW);
    digitalWrite(STP3, LOW);
    delayMicroseconds(SPD);
  }
}

void FastLongLeft2() {     //起步时速度较快的左行
  int x;

  digitalWrite(EN1, HIGH);
  digitalWrite(EN3, HIGH);

  digitalWrite(DIR1, HIGH);
  digitalWrite(DIR3, LOW); //1.3号电机旋转方向

  for (x = 0; x < 500; x++) {
    digitalWrite(STP1, HIGH);
    digitalWrite(STP3, HIGH);
    delayMicroseconds(20);

    digitalWrite(STP1, LOW);
    digitalWrite(STP3, LOW);
    delayMicroseconds(20);
  }
}

void Right() {     //向右行驶（无摇杆动作）
  int x;

  digitalWrite(EN1, HIGH);
  digitalWrite(EN3, HIGH);

  digitalWrite(DIR1, LOW);
  digitalWrite(DIR3, HIGH); //1.3号电机旋转方向

  for (x = 0; x < Step; x++) {
    digitalWrite(STP1, HIGH);
    digitalWrite(STP3, HIGH);
    delayMicroseconds(SPD);

    digitalWrite(STP1, LOW);
    digitalWrite(STP3, LOW);
    delayMicroseconds(SPD);
  }
}

void Left_left(int steptogo) { //左行同时向左调整方向
  digitalWrite(EN1, HIGH);
  digitalWrite(EN3, HIGH); //1、3号电机使能

  digitalWrite(DIR1, HIGH);
  digitalWrite(DIR3, LOW); //1.3号电机旋转方向

  for (int x = 0; x < steptogo / 2; x++) {
    digitalWrite(STP1, HIGH);
    delayMicroseconds(SPD / 2);
    digitalWrite(STP1, LOW);
    delayMicroseconds(SPD / 2);
    digitalWrite(STP1, HIGH);
    digitalWrite(STP3, HIGH);
    delayMicroseconds(SPD / 2);
    digitalWrite(STP1, LOW);
    digitalWrite(STP3, LOW);
    delayMicroseconds(SPD / 2);
  }
}

void Left_right(int steptogo) { //左行同时向右调整方向
  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, HIGH);
  digitalWrite(EN3, HIGH); //1、3号电机使能

  digitalWrite(DIR1, HIGH);
  digitalWrite(DIR3, LOW); //1.3号电机旋转方向

  for (int x = 0; x < steptogo / 2; x++) {
    digitalWrite(STP3, HIGH);
    delayMicroseconds(SPD / 2);
    digitalWrite(STP3, LOW);
    delayMicroseconds(SPD / 2);
    digitalWrite(STP3, HIGH);
    digitalWrite(STP1, HIGH);
    delayMicroseconds(SPD / 2);
    digitalWrite(STP3, LOW);
    digitalWrite(STP1, LOW);
    delayMicroseconds(SPD / 2);
  }
}
void FastRight() {     //向右行驶（无摇杆动作）
  int x;

  digitalWrite(EN1, HIGH);
  digitalWrite(EN3, HIGH);

  digitalWrite(DIR1, LOW);
  digitalWrite(DIR3, HIGH); //1.3号电机旋转方向

  for (x = 0; x < 600; x++) {
    digitalWrite(STP1, HIGH);
    digitalWrite(STP3, HIGH);
    delayMicroseconds(SPD / 3);

    digitalWrite(STP1, LOW);
    digitalWrite(STP3, LOW);
    delayMicroseconds(SPD / 3);
  }
}
void FastLongRight() {     //向右行驶（无摇杆动作）
  int x;

  digitalWrite(EN1, HIGH);
  digitalWrite(EN3, HIGH);
  digitalWrite(EN4, HIGH);

  digitalWrite(DIR1, LOW);
  digitalWrite(DIR3, HIGH); //1.3号电机旋转方向
  digitalWrite(DIR4, HIGH);

  for (x = 0; x < 600; x++) {
    digitalWrite(STP1, HIGH);
    digitalWrite(STP3, HIGH);
    digitalWrite(STP4, HIGH);
    delayMicroseconds(SPD / 3);

    digitalWrite(STP1, LOW);
    digitalWrite(STP3, LOW);
    digitalWrite(STP4, LOW);
    delayMicroseconds(SPD / 3);
  }
}
void FastLongRight2() {     //向右行驶（无摇杆动作）
  int x;

  digitalWrite(EN1, HIGH);
  digitalWrite(EN3, HIGH);
  digitalWrite(EN4, HIGH);

  digitalWrite(DIR1, LOW);
  digitalWrite(DIR3, HIGH); //1.3号电机旋转方向
  digitalWrite(DIR4, LOW);

  for (x = 0; x < 600; x++) {
    digitalWrite(STP1, HIGH);
    digitalWrite(STP3, HIGH);
    digitalWrite(STP4, HIGH);
    delayMicroseconds(SPD / 3);

    digitalWrite(STP1, LOW);
    digitalWrite(STP3, LOW);
    digitalWrite(STP4, LOW);
    delayMicroseconds(SPD / 3);
  }
}
void RightRotate() {      //顺时针旋转
  int x;

  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, HIGH);
  digitalWrite(EN3, HIGH);
  digitalWrite(EN4, HIGH);
  digitalWrite(EN5, HIGH);

  digitalWrite(DIR1, HIGH);
  digitalWrite(DIR2, HIGH);
  digitalWrite(DIR3, HIGH);
  digitalWrite(DIR4, HIGH);

  for (x = 0; x < Step; x++) {
    digitalWrite(STP1, HIGH);
    digitalWrite(STP2, HIGH);
    digitalWrite(STP3, HIGH);
    digitalWrite(STP4, HIGH);
    delayMicroseconds(SPD);

    digitalWrite(STP1, LOW);
    digitalWrite(STP2, LOW);
    digitalWrite(STP3, LOW);
    digitalWrite(STP4, LOW);
    delayMicroseconds(SPD);
  }
}

void LeftRotate() {      //逆时针旋转
  int x;

  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, HIGH);
  digitalWrite(EN3, HIGH);
  digitalWrite(EN4, HIGH);
  digitalWrite(EN5, HIGH);

  digitalWrite(DIR1, LOW);
  digitalWrite(DIR2, LOW);
  digitalWrite(DIR3, LOW);
  digitalWrite(DIR4, LOW);

  for (x = 0; x < Step; x++) {
    digitalWrite(STP1, HIGH);
    digitalWrite(STP2, HIGH);
    digitalWrite(STP3, HIGH);
    digitalWrite(STP4, HIGH);
    delayMicroseconds(SPD);

    digitalWrite(STP1, LOW);
    digitalWrite(STP2, LOW);
    digitalWrite(STP3, LOW);
    digitalWrite(STP4, LOW);
    delayMicroseconds(SPD);
  }
}

void SmallForward() {
  int x;

  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, HIGH);
  digitalWrite(EN3, HIGH);
  digitalWrite(EN4, HIGH);
  digitalWrite(EN5, HIGH);

  digitalWrite(DIR2, LOW);
  digitalWrite(DIR4, HIGH); //2.4号电机旋转

  for (x = 0; x < Step; x++) {
    digitalWrite(STP2, HIGH);
    digitalWrite(STP4, HIGH);
    delayMicroseconds(SPD / 2);

    digitalWrite(STP2, LOW);
    digitalWrite(STP4, LOW);
    delayMicroseconds(SPD / 2);
  }
}

void SmallBack() {
  int x;

  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, HIGH);
  digitalWrite(EN3, HIGH);
  digitalWrite(EN4, HIGH);
  digitalWrite(EN5, HIGH);

  digitalWrite(DIR2, HIGH);
  digitalWrite(DIR4, LOW); //2.4号电机旋转

  for (x = 0; x < Step; x++) {
    digitalWrite(STP2, HIGH);
    digitalWrite(STP4, HIGH);
    delayMicroseconds(SPD);

    digitalWrite(STP2, LOW);
    digitalWrite(STP4, LOW);
    delayMicroseconds(SPD);
  }
}

void Right_right(int steptogo) { //右行同时向右调整方向
  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, HIGH);
  digitalWrite(EN3, HIGH); //使能1号、3号电机

  digitalWrite(DIR1, LOW);
  digitalWrite(DIR2, LOW);
  digitalWrite(DIR3, HIGH); //设置1号、3号电机的转向，使其向右

  for (int x = 0; x < steptogo; x++) {
    digitalWrite(STP1, HIGH);
    delayMicroseconds(SPD / 2);
    digitalWrite(STP1, LOW);
    digitalWrite(STP3, HIGH);
    digitalWrite(STP2, HIGH);
    delayMicroseconds(SPD / 2);
    digitalWrite(STP1, HIGH);
    delayMicroseconds(SPD / 2);
    digitalWrite(STP1, LOW);
    digitalWrite(STP3, LOW);
    digitalWrite(STP2, LOW);
    delayMicroseconds(SPD / 2);
  }
}
void Right_left(int steptogo) { //右行同时向左调整方向
  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, HIGH);
  digitalWrite(EN3, HIGH); //使能1号、3号电机

  digitalWrite(DIR1, LOW);
  digitalWrite(DIR2, HIGH);
  digitalWrite(DIR3, HIGH); //设置1号、3号电机的转向，使其向右

  for (int x = 0; x < steptogo; x++) {
    digitalWrite(STP3, HIGH);
    delayMicroseconds(SPD / 2);
    digitalWrite(STP3, LOW);
    delayMicroseconds(SPD / 2);
    digitalWrite(STP3, HIGH);
    digitalWrite(STP2, HIGH);
    digitalWrite(STP1, HIGH);
    delayMicroseconds(SPD / 2);
    digitalWrite(STP3, LOW);
    digitalWrite(STP2, LOW);
    digitalWrite(STP1, LOW);
    delayMicroseconds(SPD / 2);
  }
}


void Mymoto() {
  digitalWrite(Moto1, HIGH);
  digitalWrite(Moto2, LOW);       //往下
  delay(1200);
  digitalWrite(Moto1, LOW);
  digitalWrite(Moto2, LOW);
  delay(1200);
  digitalWrite(Moto1, LOW);
  digitalWrite(Moto2, HIGH);      //往上
  delay(1200);
}

void MotoGo() {
  digitalWrite(Moto1, LOW);
  digitalWrite(Moto2, HIGH);      //往上
  delay(1200);

  digitalWrite(Moto1, LOW);
  digitalWrite(Moto2, LOW);
}

void MotoGo2() {
  digitalWrite(Moto1, HIGH);
  digitalWrite(Moto2, LOW);      //往下
}
void MotoGo3() {
  digitalWrite(Moto1, LOW);
  digitalWrite(Moto2, HIGH);      //往上
}

void ShakeMoto() {
  digitalWrite(Moto3, HIGH);
  digitalWrite(Moto4, LOW);
}

void ShakeStop() {
  digitalWrite(Moto3, LOW);
  digitalWrite(Moto4, LOW);
}
void CloseGate() {
  digitalWrite(Moto1, HIGH);
  digitalWrite(Moto2, LOW);

}

void ShakeRight() {
  int x;

  digitalWrite(EN1, HIGH);
  digitalWrite(EN3, HIGH);

  digitalWrite(DIR1, LOW);
  digitalWrite(DIR3, HIGH); //1.3号电机旋转方向

  for (x = 0; x < 600; x++) {
    digitalWrite(STP1, HIGH);
    digitalWrite(STP3, HIGH);
    delayMicroseconds(SPD);

    digitalWrite(STP1, LOW);
    digitalWrite(STP3, LOW);
    delayMicroseconds(SPD);
  }
  delay(10);
}

void Change() {
  mode = 1;
  detachInterrupt(0);
}

void AdjustLeft(int goal) { //带轨迹调整功能的右移运动
  while (goal)
  {
    d[0] = digitalRead(DIS2F);
    d[1] = digitalRead(DIS2N);
    d[2] = digitalRead(DIS1F);
    d[3] = digitalRead(DIS1N);

    if (!d[0] & !d[1] & !d[2] & d[3]) { //0001
      Left_right(10);
    }
    else if (!d[0] & !d[1] & d[2] & !d[3]) { //0010
      Left_left(10);
    }
    else if (!d[0] & d[1] & !d[2] & !d[3]) { //0100
      Left_left(10);
    }
    else if (!d[0] & d[1] & !d[2] & d[3]) { //0101
      SmallForward();
    }
    else if (!d[0] & d[1] & d[2] & !d[3]) { //0110
      Left_left(10);
    }
    else if (d[0] & !d[1] & !d[2] & !d[3]) { //1000
      Left_right(10);
    }
    else if (d[0] & !d[1] & !d[2] & d[3]) { //1001
      Left_right(10);
    }
    else if (d[0] & !d[1] & d[2] & !d[3]) { //1010
      SmallBack();
    }
    else { //0000
      Left();
      goal -= 1;
    }
  }
}

void AdjustRight(int goal) { //带轨迹调整功能的右移运动
  while (goal)
  {
    d[0] = digitalRead(DIS2F);
    d[1] = digitalRead(DIS2N);
    d[2] = digitalRead(DIS1F);
    d[3] = digitalRead(DIS1N);

    if (!d[0] & !d[1] & !d[2] & d[3]) { //0001
      Right_right(1);
    }
    else if (!d[0] & !d[1] & d[2] & !d[3]) { //0010
      Right_left(1);
    }
    else if (!d[0] & d[1] & !d[2] & !d[3]) { //0100
      Right_left(1);
    }
    else if (!d[0] & d[1] & !d[2] & d[3]) { //0101
      SmallForward();
    }
    else if (!d[0] & d[1] & d[2] & !d[3]) { //0110
      Right_left(1);
    }
    else if (d[0] & !d[1] & !d[2] & !d[3]) { //1000
      Right_right(1);
    }
    else if (d[0] & !d[1] & !d[2] & d[3]) { //1001
      Right_right(1);
    }
    else if (d[0] & !d[1] & d[2] & !d[3]) { //1010
      SmallBack();
    }
    else { //0000
      Right();
      goal -= 1;
    }
  }
}

void LeftRelease() {
  int x;
  SPD = 40;

  /*for (x = 0; x < reSTP - 40; x++) { //向左盲走
    FastLongLeft1();
    }

    for (x = 0; x < 120; x++) { //向左盲走
    FastLongLeft2();
    }

    delay(300);

    for (x = 0; x < 60; x++) {     //向上
    Forward();
    }*/
  CrossLeft();

  Servo1.write(70);      //打开舵机门释放绿花盆
  delay(50);
  ShakeMoto();

  for (x = 0; x < reSTP + 40; x++) { //向右盲走一段    From  reSTP+50
    if (x == 60) {
      Servo1.write(80);
      MotoGo3();
      Servo2.write(70);
    }
    if (x == 80) {
      digitalWrite(Moto1, LOW);
      digitalWrite(Moto2, LOW);
    }
    FastLongRight();
  }

  ShakeStop();
  Servo1.write(50);

  for (x = 0; x < 40; x++) {      //向下快速盲走
    FastBack();
  }

  Servo1.write(160);
  Servo2.write(160);

  SPD = 100;
}

void RightRelease() {
  int x;
  SPD = 40;    //改变速度为40

  /*for (x = 0; x < reSTP - 40; x++) { //向左慢速盲走
    FastLongLeft1();
    }

    for (x = 0; x < 110; x++) { //向左快速盲走
    FastLongLeft2();
    }

    delay(300);

    for (x = 0; x < 60; x++) {     //向上
    FastBack();
    }

    for (x = 0; x < 1250; x++) { //旋转
    LeftRotate();
    }                                      //加入旋转

    for (x = 0; x < 10; x++) { //贴墙
    Back();
    }*/
  CrossRight();


  Servo2.write(100);      //打开舵机门释放绿花盆
  delay(50);
  ShakeMoto();

  for (x = 0; x < reSTP + 40; x++) { //向右盲走一段  From 50
    if (x == 60) {
      Servo2.write(80);
      MotoGo2();
      Servo1.write(100);
    }
    if (x == 80) {
      digitalWrite(Moto1, LOW);
      digitalWrite(Moto2, LOW);
    }
    FastLongRight2();
  }

  ShakeStop();
  Servo2.write(160);

  for (x = 0; x < 40; x++) {      //向下快速盲走
    Forward();
  }

  Servo1.write(0);
  Servo2.write(0);

  SPD = 100;
}

void ReleaseRedGold() {

  for (int i = 0; i < 15; i++) {
    Forward();
  }

  for (int i = 0; i < 7500; ++i)
  {
    LeftRotate();
  }
  for (int i = 0; i < 15; ++i)
  {
    Forward();
  }

  for (int i = 0; i < 25; i++) {
    FastRight();
  }

  HalfRelease();

  for (int i = 0; i < 8; i++) {
    FastRight();
  }

  for (int i = 0; i < 20; ++i)
  {
    FastLongLeft();
  }
}

void ReleaseRedGold2() {

  for (int i = 0; i < 15; i++) {
    FastBack();
  }

  for (int i = 0; i < 7500; ++i)
  {
    RightRotate();
  }
  for (int i = 0; i < 15; ++i)
  {
    FastBack();
  }

  for (int i = 0; i < 25; i++) {
    FastRight();
  }

  HalfRelease();

  for (int i = 0; i < 8; i++) {
    FastRight();
  }

  for (int i = 0; i < 20; ++i)
  {
    FastLongLeft();
  }
}

void ReleaseGreenGold() {
  int x;

  for (x = 0; x < reSTP - 30; x++) { //向左盲走
    FastLongLeft1();
  }

  for (x = 0; x < 120; x++) { //向左快速盲走
    FastLongLeft2();
  }

  for (x = 0; x < 1000; x++) {
    AdjustRight(1);
  }

  for (int i = 0; i < 15; i++) {
    Forward();
  }


  for (int i = 0; i < 7500; ++i)
  {
    LeftRotate();
  }


  for (int i = 0; i < 25; i++) {
    FastRight();
  }

  HalfRelease();

  for (int i = 0; i < 8; i++) {
    FastRight();
  }

  for (int i = 0; i < 20; ++i)
  {
    FastLongLeft();
  }
}

void ReleaseGreenGold2() {
  int x;

  for (x = 0; x < reSTP - 30; x++) { //向左盲走
    FastLongLeft1();
  }

  for (x = 0; x < 120; x++) { //向左快速盲走
    FastLongLeft2();
  }

  for (x = 0; x < 1000; x++) {
    AdjustRight(1);
  }

  for (int i = 0; i < 15; i++) {
    FastBack();
  }


  for (int i = 0; i < 7500; ++i)
  {
    RightRotate();
  }


  for (int i = 0; i < 25; i++) {
    FastRight();
  }

  HalfRelease();

  for (int i = 0; i < 8; i++) {
    FastRight();
  }

  for (int i = 0; i < 20; ++i)
  {
    FastLongLeft();
  }
}

void Leftloop() {
  //------------左边出发---------------
  if (Pots < 10 && mode == 0) {
    AdjustRight(1);
  }
  else if (Pots < 9 && mode == 1) {
    NewCatch();
    mode = 0;
    attachInterrupt(0, Change, LOW);
  }
  else if (Pots == 9 && mode == 1) {
    SafeCatch();
    mode = 0;
    attachInterrupt(0, Change, LOW);
  }
  else{
    Pots = 0;
    detachInterrupt(0);

    LeftRelease();

    attachInterrupt(0, SafeHalfCatch, LOW);
    MsTimer2::start();

    while (1) {
      AdjustRight(1);
      if (Pots != 0) {
        MsTimer2::stop();
        break;
      }
    }
    detachInterrupt(0);

    if (digitalRead(Nano) == LOW) {
      ReleaseRedGold();
    }
    else {
      ReleaseGreenGold();
    }

    while (1);
  }
}

void NewCatch() {
  int x;
  delayMicroseconds(1000);
  if (digitalRead(OP2) == LOW) {

    digitalWrite(EN1, HIGH);
    digitalWrite(EN2, LOW);
    digitalWrite(EN3, HIGH);
    digitalWrite(EN4, LOW);
    digitalWrite(EN5, HIGH);

    digitalWrite(DIR1, LOW);
    digitalWrite(DIR3, HIGH); //1.3号电机旋转方向

    digitalWrite(DIR5, HIGH);

    for (x = 0; x < 1000; x++) { //300步的小前进，以确定盆能进入到爪内
      digitalWrite(STP1, HIGH);  //一倍速度
      digitalWrite(STP3, HIGH);
      delayMicroseconds(SPD);

      digitalWrite(STP1, LOW);
      digitalWrite(STP3, LOW);
      delayMicroseconds(SPD);
    }

    for (x = 0; x < 200; x++) {
      digitalWrite(STP1, HIGH); //0.5倍速，600步，用低速让花盆能略微抬起，不至于直接进行高速
      digitalWrite(STP3, HIGH);
      digitalWrite(STP5, HIGH);
      digitalWrite(STP6, HIGH);
      delayMicroseconds(SPD);

      digitalWrite(STP1, LOW);
      digitalWrite(STP3, LOW);
      delayMicroseconds(SPD);

      digitalWrite(STP1, HIGH);
      digitalWrite(STP3, HIGH);
      digitalWrite(STP5, LOW);
      digitalWrite(STP6, LOW);
      delayMicroseconds(SPD);

      digitalWrite(STP1, LOW);
      digitalWrite(STP3, LOW);
      delayMicroseconds(SPD);
    }                                  //    */

    for (x = 0; x < 3000; x++) {
      digitalWrite(STP1, HIGH);  //一倍速度，2600步，完成一周旋转，高速
      digitalWrite(STP3, HIGH);
      digitalWrite(STP5, HIGH);
      delayMicroseconds(SPD);



      digitalWrite(STP1, LOW);
      digitalWrite(STP3, LOW);
      digitalWrite(STP5, LOW);
      delayMicroseconds(SPD);
    }
    Pots++;
  }
}

void SafeCatch() {
  int x;
  delayMicroseconds(1000);
  if (digitalRead(OP2) == LOW) {

    digitalWrite(EN1, HIGH);
    digitalWrite(EN2, LOW);
    digitalWrite(EN3, HIGH);
    digitalWrite(EN4, LOW);
    digitalWrite(EN5, HIGH);

    digitalWrite(DIR1, LOW);
    digitalWrite(DIR3, HIGH); //1.3号电机旋转方向

    digitalWrite(DIR5, HIGH);

    for (x = 0; x < 1500; x++) { //300步的小前进，以确定盆能进入到爪内
      digitalWrite(STP1, HIGH);  //一倍速度
      digitalWrite(STP3, HIGH);
      delayMicroseconds(SPD);

      digitalWrite(STP1, LOW);
      digitalWrite(STP3, LOW);
      delayMicroseconds(SPD);
    }

    for (x = 0; x < 400; x++) {
      digitalWrite(STP1, HIGH); //0.5倍速，600步，用低速让花盆能略微抬起，不至于直接进行高速
      digitalWrite(STP3, HIGH);
      digitalWrite(STP5, HIGH);
      digitalWrite(STP6, HIGH);
      delayMicroseconds(SPD);

      digitalWrite(STP1, LOW);
      digitalWrite(STP3, LOW);
      delayMicroseconds(SPD);

      digitalWrite(STP1, HIGH);
      digitalWrite(STP3, HIGH);
      digitalWrite(STP5, LOW);
      digitalWrite(STP6, LOW);
      delayMicroseconds(SPD);

      digitalWrite(STP1, LOW);
      digitalWrite(STP3, LOW);
      delayMicroseconds(SPD);
    }                                  //    */

    for (x = 0; x < 2800; x++) {
      digitalWrite(STP5, HIGH);
      delayMicroseconds(SPD);

      digitalWrite(STP5, LOW);
      delayMicroseconds(SPD);
    }
    Pots++;
  }
}

void SafeHalfCatch() {
  int x;
  delayMicroseconds(1000);
  if (digitalRead(OP2) == LOW) {

    digitalWrite(EN1, HIGH);
    digitalWrite(EN2, LOW);
    digitalWrite(EN3, HIGH);
    digitalWrite(EN4, LOW);
    digitalWrite(EN5, HIGH);

    digitalWrite(DIR1, LOW);
    digitalWrite(DIR3, HIGH); //1.3号电机旋转方向

    digitalWrite(DIR5, HIGH);

    for (x = 0; x < 1500; x++) { //300步的小前进，以确定盆能进入到爪内
      digitalWrite(STP1, HIGH);  //一倍速度
      digitalWrite(STP3, HIGH);
      delayMicroseconds(SPD);

      digitalWrite(STP1, LOW);
      digitalWrite(STP3, LOW);
      delayMicroseconds(SPD);
    }

    for (x = 0; x < 400; x++) {
      digitalWrite(STP1, HIGH); //0.5倍速，600步，用低速让花盆能略微抬起，不至于直接进行高速
      digitalWrite(STP3, HIGH);
      digitalWrite(STP5, HIGH);
      digitalWrite(STP6, HIGH);
      delayMicroseconds(SPD);

      digitalWrite(STP1, LOW);
      digitalWrite(STP3, LOW);
      delayMicroseconds(SPD);

      digitalWrite(STP1, HIGH);
      digitalWrite(STP3, HIGH);
      digitalWrite(STP5, LOW);
      digitalWrite(STP6, LOW);
      delayMicroseconds(SPD);

      digitalWrite(STP1, LOW);
      digitalWrite(STP3, LOW);
      delayMicroseconds(SPD);
    }                                  //    */

    for (x = 0; x < 900; x++) {
      digitalWrite(STP5, HIGH);
      delayMicroseconds(SPD*2);

      digitalWrite(STP5, LOW);
      delayMicroseconds(SPD*2);
    }
    Pots++;
  }
}

void CatchTest() {
  int x;

  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, LOW);
  digitalWrite(EN3, HIGH);
  digitalWrite(EN4, LOW);
  digitalWrite(EN5, HIGH);

  digitalWrite(DIR1, LOW);
  digitalWrite(DIR3, HIGH); //1.3号电机旋转方向

  digitalWrite(DIR5, LOW);

  for (x = 0; x < 500; x++) { //300步的小前进，以确定盆能进入到爪内
    digitalWrite(STP1, HIGH);  //一倍速度
    digitalWrite(STP3, HIGH);
    delayMicroseconds(SPD);

    digitalWrite(STP1, LOW);
    digitalWrite(STP3, LOW);
    delayMicroseconds(SPD);
  }

  for (x = 0; x < 200; x++) {
    digitalWrite(STP1, HIGH); //0.5倍速，600步，用低速让花盆能略微抬起，不至于直接进行高速
    digitalWrite(STP3, HIGH);
    digitalWrite(STP5, HIGH);
    digitalWrite(STP6, HIGH);
    delayMicroseconds(SPD);

    digitalWrite(STP1, LOW);
    digitalWrite(STP3, LOW);
    delayMicroseconds(SPD);

    digitalWrite(STP1, HIGH);
    digitalWrite(STP3, HIGH);
    digitalWrite(STP5, LOW);
    digitalWrite(STP6, LOW);
    delayMicroseconds(SPD);

    digitalWrite(STP1, LOW);
    digitalWrite(STP3, LOW);
    delayMicroseconds(SPD);
  }

  for (x = 0; x < 3000; x++) {
    digitalWrite(STP1, HIGH);  //一倍速度，2600步，完成一周旋转，高速
    digitalWrite(STP3, HIGH);
    digitalWrite(STP5, HIGH);
    delayMicroseconds(SPD);



    digitalWrite(STP1, LOW);
    digitalWrite(STP3, LOW);
    digitalWrite(STP5, LOW);
    delayMicroseconds(SPD);
  }
}

void Rightloop() {
  //------------右边出发---------------
  if (Pots < 10 && mode == 0) {
    AdjustRight(1);
  }
  else if (Pots < 9 && mode == 1) {
    NewCatch();
    mode = 0;
    attachInterrupt(0, Change, LOW);
  }
  else if (Pots == 9 && mode == 1) {
    SafeCatch();
    mode = 0;
    attachInterrupt(0, Change, LOW);
  }
  else {
    Pots = 0;
    detachInterrupt(0);

    RightRelease();

    attachInterrupt(0, SafeHalfCatch, LOW);
    MsTimer2::start();

    while (1) {
      AdjustRight(1);
      if (Pots != 0) {
        MsTimer2::stop();
        break;
      }
    }

    detachInterrupt(0);

    if (digitalRead(Nano) == LOW) {
      ReleaseRedGold2();
    }
    else {
      ReleaseGreenGold2();
    }
    while (1);
  }
}
/*--------------------------主程序-------------------------------*/
void loop() {
  if (digitalRead(RunMode) == HIGH) {
    delay(5);
    if (digitalRead(RunMode) == HIGH) {
      while (1) {
        Leftloop();
      }
    }
  }
  else if (digitalRead(RunMode) == LOW) {
    delay(5);
    if (digitalRead(RunMode) == LOW) {
      while (1) {
        Rightloop();
      }
    }
  }
}
