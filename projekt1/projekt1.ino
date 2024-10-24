/*
* Name: clock and temp project
* Author: Erik Marenius Dehlin
* Date: 2024-10-24
* Description: This project uses a ds3231 to measure time and displays the time to an 1306 oled display,
* Further, it measures temprature with a analog temprature module and displays a mapped value to a 9g-servo-motor
*/

// Include Libraries
#include <RTClib.h>
#include <Wire.h>
#include "U8glib.h"
#include <Servo.h>

// Init constants
const int piezoPin = 8;

// Init global variables
int ang = 0;

// construct objects
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);
Servo myservo;
RTC_DS3231 rtc;


void setup() {
  // init communication
  Serial.begin(9600);
  // Init Hardware
  Wire.begin();
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  myservo.attach(7);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  u8g.setFont(u8g_font_helvB12);
  pinMode(A0, INPUT);

}


void loop() {

  oledWrite(getTime());   //Printar tiden på skärmen
  servoWrite(getTemp());  //Servern roterar beroende på temperaturen
  piezoPlay(getTemp());   //Piezo spelar beroende på temperaturen
  delay(1000); 
}


/*
*This function reads time from an ds3231 module and package the time as a String
*Parameters: Void
*Returns: time in hh:mm:ss as String
*/
String getTime(){
  DateTime now = rtc.now(); 
  return String(now.hour()) +":"+ String(now.minute()) +":"+ String(now.second()); // Returnar tiden som en string
}

/*
* This function reads an analog pin connected to an analog temprature sensor and calculates the corresponding temp
*Parameters: Void
*Returns: temprature as float
*/
float getTemp(){
  float temp = 0;
  float R1 = 10000;  // value of R1 on board
  float logR2, R2, T;
  float c1 = 0.001129148, c2 = 0.000234125, c3 = 0.0000000876741;  //steinhart-hart coeficients for thermistor

  int Vo = analogRead(A0);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);  //calculate resistance on thermistor
  logR2 = log(R2);
  temp = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));  // temperature in Kelvin
  temp = temp - 273.15;                                           //convert Kelvin to Celcius

  return temp;
}

/*
* This function takes a string and draws it to an oled display
*Parameters: - text: String to write to display
*Returns: void
*/
void oledWrite(String text){
  u8g.firstPage();

  do {
    u8g.drawStr(0, 22, text.c_str());
  } while (u8g.nextPage());
}

/*
* takes a temperature value and maps it to corresponding degree on a servo
*Parameters: - value: temeprature
*Returns: void
*/
void servoWrite(float value){
  float ang = map(value, 20, 30, 0, 180); //Mappar temperaturen till en grad på servomotorn
  myservo.write(ang);                     
  Serial.println(String(ang));
}

/*
* takes a temprature value and maps it to corresponding tone on a piezo
*Parameters: - value: temperature
*Returns: void
*/

void piezoPlay(float value){
  float tone1 = map(value,20,30,20,1000); // Mappar temperaturen till en ton på Piezo:n

  tone(8,tone1);
  delay(50);
  noTone(8);

}

