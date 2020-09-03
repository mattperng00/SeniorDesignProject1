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

#define LCD_Rows 16
#define LCD_Cols 2

// Pin assignments
#define LCD_RS 12
#define LCD_ENABLE 11
#define LCD_D4 5
#define LCD_D5 4
#define LCD_D6 3
#define LCD_D7 2

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
int set_temp = 40; // In Celsius
int currTemp = 0;
int pastTemp = 0;


void setup() {
  Serial.begin(9600);
  Serial.println("--- Start Serial Monitor SEND_RCVE ---");
  Serial.println(" Type in Box above, . ");

  pinMode(6, OUTPUT); // Cooling LED
  pinMode(7, OUTPUT); // Heating LED
  pinMode(8, OUTPUT); //Fan

  // set up the LCD's number of columns and rows:
  lcd.begin(LCD_Rows, LCD_Cols);
  // Print a message to the LCD.
  lcd.print("Temperature");

  digitalWrite(6, HIGH); //Turns LED OFF
  digitalWrite(7, HIGH); //Turns LED OFF
  digitalWrite(8, HIGH);
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  currTemp = -40 + 0.488155 * (analogRead(A0) - 20);
  lcd.setCursor(2, 2);

  //if function so screen doesn't clear on every loop, otherwise screen flashes on every loop
  if(currTemp != pastTemp)
  {
     lcd.clear();
     pastTemp = currTemp;
     lcd.print("Temp: ");
     Serial.println(currTemp);

     lcd.setCursor(6, 0);
     lcd.print(currTemp);
     lcd.setCursor(9,0);
     lcd.print("C");
  }
  else
  {
    Serial.println(currTemp);
  }
  delay(1000);
}
