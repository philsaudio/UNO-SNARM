#include <Servo.h> 
#include <SoftwareSerial.h>

SoftwareSerial mySerial(12, 13); // RX, TX
const int SERVOS = 4;
int PIN[SERVOS], value[SERVOS], currentAngle[SERVOS], MIN[SERVOS], MAX[SERVOS], INITANGLE[SERVOS];
Servo myservo[SERVOS];
int afrom[] = {90, 35, 135}, ato[] = {20, 140, 110}, amiddle[] = {20, 35, 135}, afinal[] = {179,75,95}, aafterfinal[] = {179,35,135};
int DELAYTIME = 200;
int servo_moving[SERVOS] = {0,0,0,0};
boolean stringComplete = false;
int bt_servo = 0;
int bt_move = 0;
int idle = 0;


void setup() {
  Serial.begin(9600);
  pinMode(3, OUTPUT);  
  digitalWrite(3, HIGH); 
  mySerial.begin(9600);
  
  init_Pins();
  
  //auto_mode();
}

void loop() {
  move_bt();
  move_joy();  
}

void init_Pins(){
  PIN[0] = 11;
  MIN[0] = 0;
  MAX[0] = 179;
  INITANGLE[0] = 90;
  PIN[1] = 10;
  MIN[1] = 35;
  MAX[1] = 179;
  INITANGLE[1] = 90;
  PIN[2] = 9;
  MIN[2] = 90;
  MAX[2] = 179;
  INITANGLE[2] = 155;
  PIN[3] = 5;
  MIN[3] = 0;
  MAX[3] = 179;
  INITANGLE[3] = 25;
  
  for (int i = 0; i < SERVOS; i++){
    myservo[i].attach(PIN[i]);
    myservo[i].write(INITANGLE[i]);
    value[i] = 0;
    idle = 0;
  }  
}

void move_bt(){ 
  checkSoftSerial();

  for (int i = 0; i < SERVOS; i++){
    currentAngle[i] = myservo[i].read();
    
    if (servo_moving[i] != 0){
      currentAngle[i] += servo_moving[i];
      currentAngle[i] = currentAngle[i] > MAX[i] ? --currentAngle[i] : currentAngle[i];
      currentAngle[i] = currentAngle[i] < MIN[i] ? ++currentAngle[i] : currentAngle[i];
      myservo[i].write(currentAngle[i]);
      delay(20);
    }
  }
}

void checkSoftSerial() {
  String str = "";
  
  if (mySerial.available()){
    for (int i = 0 ; i < 2; i++){
      str += (char)mySerial.read(); 
    }
    // the servo to move
    int value = str.toInt();
    bt_servo = value / 10;
    
    // the direction to move
    int angle = value % 10;
    if (angle == 2) bt_move = 1;
    else if (angle == 1) bt_move = -1;
    else bt_move = 0;
    
    servo_moving[bt_servo] = bt_move;
  }
}

void move_joy(){
  for (int i = 0; i < SERVOS; i++){
    value[i] = analogRead(i);
    currentAngle[i] = myservo[i].read();
    
       
    if (value[i] > 612) {
      idle = 0;      
      if (currentAngle[i] > MIN[i]) --currentAngle[i]; 
    } else if (value[i] < 412) {
      idle = 0;
      if (currentAngle[i] < MAX[i]) ++currentAngle[i]; 
    } else {
      ++idle;
    }
    
    if (idle == 100){
      myservo[i].detach();
    }
  }      
    
  for (int i = 0 ; i < SERVOS; i++){
    if (!myservo[i].attached()) myservo[i].attach(PIN[i]);
    myservo[i].write(currentAngle[i]);
  }  

  delay(20);
}

void auto_mode(){
  for (int i = 0; i < 2; i++){
    closeclaw(false);
    armfromto(afrom, ato);
    closeclaw(true);
    delay(DELAYTIME); 
    armfromto(ato, amiddle);
    delay(DELAYTIME);   
    armfromto(amiddle, afinal);
    closeclaw(false);
    delay(DELAYTIME);
    armfromto(afinal, aafterfinal);
    delay(DELAYTIME);
    armfromto(aafterfinal, afrom);
    delay(DELAYTIME);
  } 
}

void armfromto(int *arrf, int *arrt){
  int lp[3], seg = 3, sign;
  
  for (int i = 0; i < 3; i++){    
    lp[i] = abs((arrt[i] - arrf[i])/seg);
  }
  
  //delay(DALAYTIME);
  for (int i = 0; i < 3; i++){
    sign = arrt[i] - arrf[i] > 0 ? 1 : -1;
    for (int j = 0; j < lp[i]; j++){
      myservo[i].write(arrf[i]+j*seg*sign);
      delay(20);
    }
    delay(DELAYTIME);
  }
}

void closeclaw(boolean op){
  if (op){
    myservo[3].write(5);
  } else {
    myservo[3].write(30);
  }
}
 
