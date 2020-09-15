/*
  LiquidCrystal Library - Hello World

  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

// include the library code:
#include <LiquidCrystal.h>

// Pin assignments
#define LCD_RS 12
#define LCD_ENABLE 11
#define LCD_D4 5
#define LCD_D5 4
#define LCD_D6 3
#define LCD_D7 2

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
int set_temp = 41; // In Celsius
int currTemp = 0;
int pastTemp = 0;
int buttonState = 0;
int lastState = 0;

void setup() {
  
  pinMode(0, INPUT); // Temp Switch
  pinMode(13, INPUT); //Increment
  Serial.begin(9600);
  Serial.println("--- Start Serial Monitor SEND_RCVE ---");
  Serial.println(" Type in Box above, . ");

  pinMode(6, OUTPUT); // Cooling LED
  pinMode(7, OUTPUT); // Heating LED
  pinMode(8, OUTPUT); //Fan
  pinMode(9, OUTPUT); //Alarm
  pinMode(10, INPUT); //Alarm Switch
  
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.setCursor(0,0);
  lcd.print("Temp: ");
  
  digitalWrite(6, HIGH); //Turns LED OFF
  digitalWrite(7, HIGH); //Turns LED OFF
  digitalWrite(8, HIGH); // Turns Fan OFF
  digitalWrite(9, LOW); // Turns Alarm OFF
  
}
  
void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  currTemp = -40 + 0.488155 * (analogRead(A0) - 20);
    
  int tarTemp = set_temp;
  int Lasttemp = 0;
  buttonState = digitalRead(13);
 
    if(buttonState == HIGH)
    {
      Serial.println("on");
      set_temp += 1;
      Lasttemp = tarTemp;
      tarTemp = set_temp;
      
      Serial.println(tarTemp);
    }
    else
    {
      //Serial.println("off");
    }
    delay(5);
  
 
  
  //if function so screen doesn't clear on every loop, otherwise screen flashes on every loop
  if(currTemp != pastTemp || tarTemp != Lasttemp)
  {
    lcd.clear();
    pastTemp = currTemp;
    
    if(digitalRead(0) == HIGH)
    {
     lcd.setCursor(0,0);
     lcd.print("TTemp: ");
     lcd.setCursor(7,0);
     lcd.print(tarTemp);
     lcd.setCursor(10,0);
     lcd.print("C");
     Serial.println(currTemp); Serial.println("C");
    }
    else if(digitalRead(0) == LOW)
    {
     tarTemp = (tarTemp * 9/5) +32;
     currTemp = (currTemp * 9/5) +32;
     lcd.setCursor(0,0);
     lcd.print("TTemp: ");
     lcd.setCursor(7,0);
     lcd.print(tarTemp);
     lcd.setCursor(10,0);
     lcd.print("F");
     Serial.println(currTemp); Serial.println("F");
    }
  }
  
  if(currTemp > tarTemp)
  {
    int Alrm = currTemp - tarTemp; 
    if(Alrm > 10)
    {
      digitalWrite(6, LOW); //Blue LED on
      digitalWrite(7, HIGH); // RED LED off
      digitalWrite(8, LOW); //  turn on fan
      lcd.setCursor(0,1);
      lcd.print("Fan On");
      if(digitalRead(10) == HIGH)
      {
        tone(9, 1200, 50);
      	delay(100);
      	tone(9, 1000, 50);
      	delay(100);
      	tone(9, 1200, 50);
      	delay(100);
      	noTone(9);
      }
      else
      {
       noTone(9);
      }
    }
    else
    {
     digitalWrite(6, LOW); //Blue LED on
     digitalWrite(7, HIGH); // RED LED off
     digitalWrite(8, LOW); //  turn on fan
     lcd.setCursor(0,1);
     lcd.print("Fan On");
    }

  }
  else if(currTemp < tarTemp)
  {
     digitalWrite(6, HIGH); // Blue LED off
     digitalWrite(7, LOW); //  Red LED on
     digitalWrite(8, HIGH); //  turn off fan
     lcd.setCursor(0,1);
     lcd.print("Heat On");
  }
  else
  {
     digitalWrite(6, HIGH); // Blue LED off
     digitalWrite(7, HIGH); // Red LED OFF
     digitalWrite(8, HIGH); // turn off fan
    
  }
  delay(1000);
}
