#include <amt1001_ino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MQ135.h>
#include "SharpGP2Y10.h"

/*--------------------------------------------*/
const int LED_K = 40;
const int LED_B = 41;
const int LED_N1 = 42;
const int LED_N2 = 43;
const int NL = 44;

const int Buzzer = 3;        //Digital
const int Led_Sharp = 31;     //Digital
const int MQ_2_Dout = 32;     //Digital
const int A_IA = 4;           //PWM
const int A_IB = 5;           //PWM
const int tempSensor_H = A2;  //Analog
const int tempSensor_T = A3;  //Analog
const int MQ_135_Aout = A1;   //Analog
const int vo_Sharp = A15;      //Analog

MQ135 mq135_sensor = MQ135(MQ_135_Aout);
SharpGP2Y10 dustSensor(vo_Sharp, Led_Sharp);
LiquidCrystal_I2C LCD(0x27, 16, 2);

/*--------------------------------------------*/
float getTemp() {
  //Lay nhiet do
  uint16_t step = analogRead(tempSensor_T);
  uint16_t temperature = amt1001_gettemperature(step);

  //Tra va ket qua
  return temperature;
}
float getHumidity() {
  //Lay do am
  uint16_t step = analogRead(tempSensor_H);
  double volt = (double)step * (5.0 / 1023.0);
  uint16_t humidity = amt1001_gethumidity(volt);

  //Tra va ket qua
  return humidity;
}
uint8_t getAirQua() {
  float ppm = mq135_sensor.getPPM(); 
  if (ppm < 150) {
    return 1;
  } else {
    return 0;
  }
}
/*-----------Điều khiển điều hòa---------------*/
void airCon(int temp) {
  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.print("Nhiet do DH:");
  LCD.setCursor(5, 1);
  LCD.print(temp);
  LCD.setCursor(7, 1);
  LCD.print("*C");
}
/*---------------------------------------------*/
void query() {
  float temp = 0, hum = 0, sharp = 0;
  int airQuality = "", gas = "";

  temp = getTemp();
  hum = getHumidity();
  sharp = dustSensor.getDustDensity();
  if (getAirQua() == 1) {
    airQuality = "BAD";
  } else {
    airQuality = "GOOD";
  }
  if (digitalRead(MQ_2_Dout) == 1) {
    gas = "BAD";    
  } else {
    gas = "GOOD";
  }
  delay(500);
  Serial.println("QUERY_OK");
  delay(50);
  Serial.println((String)temp + " " + (String)hum + " " + gas + " " + (String)sharp + " " + airQuality);
  // 36,7 50 GOOD 23 GOOD
}
/*---------------------------------------------*/
void execute(int cmd) {
  if (cmd == 123) {
      //Truy van thong tin nha
      query();
  } else if (cmd> 16 && cmd<32) {
    airCon(cmd);
  } else if (cmd == -234) {
    LCD.clear();
    LCD.setCursor(1, 0);
    LCD.print("Tat dieu hoa!");
  } else {
    /*--------------Bat 1 thiet bi---------------*/
    if (cmd == 1) {
      //Bat den phong khach
      digitalWrite(LED_K, 1);
    } else if (cmd == 2) {
      //Bat den phon ngu 1
      digitalWrite(LED_N1, 1);
    } else if (cmd == 3) {
      //Bat den phong ngu 2
      digitalWrite(LED_N2, 1);
    } else if (cmd == 4) {
      //Bat nong lanh
      digitalWrite(NL, 1);
    } else if (cmd == 5) {
      //Mo rem cua
      OC_Curtains('O');
    } else if (cmd == 6) {
      //Bat den nha bep
      digitalWrite(LED_B, 1);
    }
    /*---------------Tat 1 thiet bi---------------*/
    else if (cmd == -1) {
      //Tat den phong khach
      digitalWrite(LED_K, 0);
    } else if (cmd == -2) {
      //Tat den phong ngu 1
      digitalWrite(LED_N1, 0);
    } else if (cmd == -3) {
      //Tat den phong ngu 2
      digitalWrite(LED_N2, 0);
    } else if (cmd == -4) {
      //Dong rem cua
      OC_Curtains('C');
    } else if (cmd == -6) {
      //Tat den nha bep
      digitalWrite(LED_B, 0);
    }
  }
}
/*---------------------------------------------*/
void OC_Curtains(char M) {
  if (M == 'C') {
    digitalWrite(A_IA, 1);
    digitalWrite(A_IB, 0);
    delay(700);
    digitalWrite(A_IA, 0);
    digitalWrite(A_IB, 0);
  } else if (M == 'O') {
    digitalWrite(A_IA, 0);
    digitalWrite(A_IB, 1);
    delay(750);
    digitalWrite(A_IA, 0);
    digitalWrite(A_IB, 0);
  } else {
    digitalWrite(A_IA, 0);
    digitalWrite(A_IB, 0);
  }
}
/*---------------------------------------------*/
void setup() {

  pinMode(Buzzer, OUTPUT);
  digitalWrite(Buzzer, 1);

  Serial.begin(9600);  //Khoi tao serial
  LCD.init();          //Khoi tao LCD
  /*-----------Khai bao chan UOTPUT------------*/
  for (int i = 40; i <= 44; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, 0);
  }
  for (int j = 4; j <= 5; j++) {
    pinMode(j, OUTPUT);
    digitalWrite(j, 0);
  }
  pinMode(Led_Sharp, OUTPUT);
  /*-----------Khai bao chan INPUT------------*/
  pinMode(MQ_2_Dout, INPUT);
  pinMode(MQ_135_Aout, INPUT);
  pinMode(tempSensor_H, INPUT);
  pinMode(tempSensor_T, INPUT);
  /*------------------------------------------*/
  LCD.backlight();
  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.print("Hello SmartHome!");
  delay(2000);
  LCD.clear();
  delay(200);
}
/*---------------------------------------------*/
void loop() {
 if (Serial.available()){
   String dataIn = Serial.readString();
   dataIn.trim();
   int data = dataIn.toInt();
   execute(data);
 }
 if (getAirQua() == 0 && getTemp() >= 50) {
   digitalWrite(Buzzer, 0);
 } else {
   digitalWrite(Buzzer, 1);
 }
}