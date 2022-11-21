/*
Denne koden lastes opp på teensy 3.6 som en node i CAN nettverk i delprosjekt 3
Brukt eksempelkode fra:
 - https://github.com/skpang/Teensy-3.6-Dual-CAN-Bus-Breakout-Board/blob/master/teensy36_oled128x64_demo.ino
   name=Teensy-3.6-Dual-CAN-Bus-Breakout-Board
   version=1.0
   date= July 2017
   author=Skpang

 - https://github.com/wollewald/MPU9250_WE
   name=MPU6500_SPI_all_data
   version=1.2.6
   date= march 2022
   author=Wolfgang Ewald <wolfgang.ewald@wolles-elektronikkiste.de>
*/
#include <FlexCAN.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <MPU6500_WE.h>
#include <Wire.h>
#define MPU6500_ADDR 0x68

#define OLED_DC     6
#define OLED_CS     10
#define OLED_RESET  5
#define OLED_CLK 13
#define oledMosi 11
Adafruit_SSD1306 display(128, 64, oledMosi, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

static CAN_message_t msg;
IntervalTimer TX_timer;
int canCount=0; //Teller mottatt CAN meldinger 
uint lastID=0;  //Brukes til display av IDen til siste mottatt CAN melding CAN message
float IMUSI=0;
float gx=0;
float gy=0;
float gz=0;
float SIx=0;
float SIy=0;
float SIz=0;

MPU6500_WE myMPU6500 = MPU6500_WE(MPU6500_ADDR);


void setup() {
  
  Serial.begin(250000);
  Can0.begin(250000); //Hastighet satt her.
  
  //Setup for IMU
  Wire.begin();
  if(!myMPU6500.init()){
    Serial.println("MPU6500 svarer ikke");
  }
  else{
    Serial.println("MPU6500 er tilkoblet");
  }
  Serial.println("Posisjoner MPU6500 flatt og ikke flytt på den - kalibrerer...");
  delay(1000);
  myMPU6500.autoOffsets();
  Serial.println("Ferdig!");
  myMPU6500.enableGyrDLPF();
  myMPU6500.setGyrDLPF(MPU6500_DLPF_6);
  myMPU6500.setSampleRateDivider(5);
  myMPU6500.setGyrRange(MPU6500_GYRO_RANGE_250);
  myMPU6500.setAccRange(MPU6500_ACC_RANGE_2G);
  myMPU6500.enableAccDLPF(true);  
  myMPU6500.setAccDLPF(MPU6500_DLPF_6);  
  delay(200);

  //Setup for OLED
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay(); // Clear buffer
  delay(1000);

   
}

void IMUMsg(void) //Sender IMU-data til CAN bus hvert sekund. I henhold til interrupt timer.
{
  
  msg.len = 8;
  msg.id = 34;  
  msg.buf[0] = gx*100;  //Akselerasjon i x-retning. Enhet [G]
  msg.buf[1] = gy*100;  //Akselerasjon i y-retning. Enhet [G]
  msg.buf[2] = gz*100;  //Akselerasjon i z-retning. Enhet [G]
  msg.buf[3] = 0;
  msg.buf[4] = 0;
  msg.buf[5] = 0;
  msg.buf[6] = 0;
  msg.buf[7] = 0;
  Can0.write(msg);

  
}


void loop() {
  
  xyzFloat gValue = myMPU6500.getGValues();
  float resultantG = myMPU6500.getResultantG(gValue);

 
  IMUSI=gValue.z*9.81;
  gx=gValue.x;
  gy=gValue.y;
  gz=gValue.z;

  display.setTextSize(0);
  display.setTextColor(WHITE);
  display.setCursor(0,5);
  display.println(" MAS-245 'Gruppe 8' ");
  display.println(" ");
  display.setTextSize(0);
  display.setCursor(0,17);
  display.println(" CAN-Statistikk");
  display.setCursor(0,24);
  display.println(" - - - - - - - - - -");
  display.setCursor(0,30);
  display.println(" IMU x:  ");
  display.setCursor(61,30);
  display.println(SIx);
  display.println(" IMU y: ");
  display.setCursor(61,39);
  display.println(SIy);
  display.setCursor(0,51);
  display.print(" IMU z: ");
  display.setCursor(61,51);
  display.print(SIz);  
  display.setCursor(90,51);
  display.println("m/s^2");

  display.drawRoundRect(0, 0, 128, 64, 3, WHITE);
  display.drawLine(2, 15, 126, 15,WHITE);
 
  display.display();
  delay(100);
  display.clearDisplay();

  static CAN_message_t inMsg;
  //Leser CAN meldingene med ID 0x22 produsert av den andre teensyen i nettverket.
 while (Can0.available())
    {
    Can0.read(inMsg);
    canCount++;
    lastID= inMsg.id; 
    if(lastID==34) //0x22=34 decimal
    {

  //Konverterer fra G til SI-enhet for akselerasjon    
 SIx=inMsg.buf[0]*0.0981; 
 SIy=inMsg.buf[1]*0.0981;
 SIz=inMsg.buf[2]*0.0981;
      inMsg.len = 8;
  inMsg.id = 35;  
  inMsg.buf[0] = inMsg.buf[0]*0.981;  //Sender akselerasjon i xyz [m/s^2] *10
  inMsg.buf[1] = inMsg.buf[1]*0.981;
  inMsg.buf[2] = inMsg.buf[2]*0.981;

  Can0.write(inMsg);

      Serial.println("Melding med ID=0x22 er sendt. Starter å sende IMU verdier til CAN-bus.");
    }

   
    }


}
