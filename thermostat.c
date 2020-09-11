/*
    Authors: Ifeanyi Orizu Jr.
    Filename: thermostat.c
    Date: 
    Description: 
*/

// TODO: Fix the #include(s)
#include <stdio.h>
#include <LiquidCrystal.h>
#include thermostat.h

/* ------------------------------ IO and State definitions ------------------------------ */
#define LCD_ROWS 2
#define LCD_COLS 16

// Pin assignments
#define LCD_RS 12
#define LCD_ENABLE 11
#define LCD_D4 5
#define LCD_D5 4
#define LCD_D6 3
#define LCD_D7 2

// TODO: Assign these IO pins
#define FAN_LED 
#define HEAT_LED 
#define FAN 
#define ALARM 
#define TEMP_SENSOR A0

enum temp_diff {EVEN, LOW, HIGH, VERY_HIGH}
enum temp_scale {CEL, FAHR} // Temperature will be handled internally as Celsius, but can be displayed as Celsius or Fahrenheit

/* ------------------------------ State variables and Settings ------------------------------ */
enum temp_diff current_temp_diff = EVEN;
volatile enum temp_scale current_temp_scale = CEL;
volatile int set_temp;

LiquidCrystal lcd(LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

void setup() {
    
    // TODO: Setup temp sensor
    
    // TODO: Setup set_temp input device(s)
    attachInterrupt(digitalPinToInterrupt(), raise_temp_isr, LOW)
    attachInterrupt(digitalPinToInterrupt(), lower_temp_isr, LOW)
    
    // TODO: Setup temp scale input
    //attachInterrupt(digitalPinToInterrupt(), temp_scale_isr, LOW)
    
    // LEDs
    pinMode(FAN_LED, OUTPUT);
    pinMode(HEAT_LED, OUTPUT);
    
    // Fan and alarm
    pinMode(FAN, OUTPUT);
    pinMode(ALARM, OUTPUT);
  
    // LCD
    lcd.begin(LCD_COLS, LCD_ROWS);
}

void loop() {
    enum temp_diff td;
    
    while (;;) {
        temp_diff = evaluate_temp(); // Read temp and compute new temp_diff state
        temp_ctrl(temp_diff); // Configure hardware based on current and next temp_diff states
        update_display(); // Update LCD
        // TODO: Call a delay function that can be interrupted
    }
}

/* ------------------------------ IO functions ------------------------------ */

int read_temp() {
    return -40 + 0.488155 * (analogRead(TEMP_SENSOR) - 20);
}

bool read_switch() {
    
}

void update_display() {
    noInterrupts();
    int temp = (current_temp_scale == CEL) ? set_temp : 9*set_temp/5 + 32;
    char temp_unit = (current_temp_scale == CEL) ? 'C' : 'F';
    interrupts();
    
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.setCursor(7, 0);
    lcd.print(temp);
    lcd.setCursor(10, 0);
    lcd.print(temp_unit);
    
    lcd.setCursor(0, 1);
    switch (current_temp_diff) {
        case EVEN:
            lcd.print("");
            break;
        case LOW:
            lcd.print("FAN ON");
            break;
        case HIGH:
        case VERY_HIGH:
            lcd.print("HEAT ON");
    }
}

void fan_led(int state) {
    digitalWrite(FAN_LED, state ? HIGH : LOW);
}

void heat_led(int state) {
    digitalWrite(HEAT_LED, state ? HIGH : LOW);
}

void fan(int state) {
    // TODO: Turn fan motor on off based on state
}

void alarm(int state) {
    // TODO: Turn alarm on off based on state
}

/* ------------------------------ State functions ------------------------------ */

void temp_ctrl(temp_diff td) {
    if (td == current_temp_diff) {
        return;
    }
    
    switch (current_temp_diff) {
        case EVEN:
            switch (td) {
                case LOW:
                    fan_led(1);
                    fan(1);
                    break;
                case HIGH:
                    heat_led(1);
                    break;
                case VERY_HIGH:
                    heat_led(1);
                    alarm(1);
            }
            break;
        case LOW:
            switch (td) {
                case EVEN:
                    fan_led(0);
                    fan(0);
                    break
                case HIGH:
                    fan_led(0);
                    fan(0);
                    heat_led(1);
                    break;
                case VERY_HIGH:
                    fan_led(0);
                    fan(0);
                    heat_led(1);
                    alarm(1);
            }
            break;
        case HIGH:
            switch (td) {
                case EVEN:
                    heat_led(0);
                    break;
                case LOW:
                    heat_led(0);
                    fan_led(1);
                    fan(1);
                    break;
                case VERY_HIGH:
                    alarm(1);
            }
            break;
        case VERY_HIGH:
            switch (td) {
                case EVEN:
                    heat_led(0);
                    alarm(0);
                    break;
                case LOW:
                    heat_led(0);
                    alarm(0);
                    fan_led(1);
                    fan(1);
                    break;
                case HIGH:
                    alarm(0);
            }
    }
    
    current_temp_diff = td;
}

enum temp_diff evaluate_temp() {
    int nominal_temp = read_temp();
    enum temp_diff td;
    
    noInterrupts();
    if (nominal_temp > set_temp) {
        if (nominal_temp - set_temp >= 10) {
            td = VERY_HIGH;
        }
        else {
            td = HIGH;
        }
    }
    else if (nominal_temp < set_temp) {
        td = LOW;
    }
    else {
        td = EVEN;
    }
    interrupts();
    
    return td;
}

/* ------------------------------ ISRs ------------------------------ */

void raise_temp_isr() {
    set_temp += 1;
}

void lower_temp_isr() {
    set_temp -= 1;
}

void temp_scale_isr() {
    temp_scale = (temp_scale == FAHR) ? CEL : FAHR;
    //update_display();
}



