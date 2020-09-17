/*
    Authors: Ifeanyi Orizu Jr.
    Filename: thermostat.c
*/

#include <LiquidCrystal.h>

// Pin assignments
#define LCD_RS 12;
#define LCD_EN 11;
#define LCD_D4 5;
#define LCD_D5 4;
#define LCD_D6 3;
#define LCD_D7 2;
#define FAN_LED 6;
#define HEAT_LED 7;
#define FAN 9;
#define ALARM 8;
#define TEMP_SENSOR A0;

void temp_ctrl(temp_diff td);
enum temp_diff evaluate_temp(int nominal_temp);
int read_temp();
void update_display();
void fan_led(int state);
void heat_led(int state);
void fan(int state);
void alarm(int state);

enum temp_scale {CEL, FAHR}; // Temperature will be handled internally as Celsius, but can be displayed as Celsius or Fahrenheit
enum temp_diff {NO_ACTION, RAISE_TEMP, LOWER_TEMP, LOWER_TEMP_FAST};

/* ------------------------------ State variables and Settings ------------------------------ */
volatile enum temp_diff current_temp_diff = NO_ACTION;
volatile enum temp_scale current_temp_scale = CEL;
volatile int set_temp;

LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

void setup() {
    // Serial print out
    Serial.begin(9600);
    
    // LEDs
    pinMode(FAN_LED, OUTPUT);
    pinMode(HEAT_LED, OUTPUT);
    
    // Fan and alarm
    pinMode(FAN, OUTPUT);
    pinMode(ALARM, OUTPUT);
  
    // LCD
    lcd.begin(16, 2);
}

void loop() {
    int temp;
    enum temp_diff td;
    
    while (1) {
        temp = read_temp();
        Serial.println(temp);
        td = evaluate_temp(temp);
        temp_ctrl(td);
        update_display();
        delay(50);
    }
}

/* ------------------------------ State functions ------------------------------ */

void temp_ctrl(temp_diff td) {
    
    switch (current_temp_diff) {
        case NO_ACTION:
            switch (td) {
                case RAISE_TEMP:
                    heat_led(1);
                    break;
                case LOWER_TEMP:
                    fan_led(1);
                    fan(1);
                    break;
                case LOWER_TEMP_FAST:
                    fan_led(1);
                    fan(1);
                    alarm(1);
            }
            break;
        case RAISE_TEMP:
            switch (td) {
                case NO_ACTION:
                    heat_led(0);
                    break;
                case LOWER_TEMP:
                    heat_led(0);
                    fan_led(1);
                    fan(1);
                    break;
                case LOWER_TEMP_FAST:
                    heat_led(0);
                    fan_led(1);
                    fan(1);                    
                    alarm(1);
            }
            break;
        case LOWER_TEMP:
            switch (td) {
                case NO_ACTION:
                    fan_led(0);
                    fan(0);
                    break;
                case RAISE_TEMP:
                    fan_led(0);
                    fan(0);
                    heat_led(1);
                    break;
                case LOWER_TEMP_FAST:
                    alarm(1);
            }
            break;
        case LOWER_TEMP_FAST:
            switch (td) {
                case NO_ACTION:
                    fan_led(0);
                    fan(0);
                    alarm(0);
                    break;
                case RAISE_TEMP:
                    fan_led(0);
                    fan(0);
                    alarm(0);
                    heat_led(1);
                    break;
                case LOWER_TEMP:
                    alarm(0);
            }
    }
    
    current_temp_diff = td;
}

enum temp_diff evaluate_temp(int nominal_temp) {
    enum temp_diff td;
    
    noInterrupts();
    if (nominal_temp > set_temp) {
        if (nominal_temp - set_temp >= 10) {
            td = LOWER_TEMP_FAST;
        }
        else {
            td = LOWER_TEMP;
        }
    }
    else if (nominal_temp < set_temp) {
        td = RAISE_TEMP;
    }
    else {
        td = NO_ACTION;
    }
    interrupts();
    
    return td;
}

/* ------------------------------ IO functions ------------------------------ */

int read_temp() {
    return 0.488155*(analogRead(TEMP_SENSOR) - 20) - 40;
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
        case NO_ACTION:
            lcd.print("       ");
            break;
        case RAISE_TEMP:
            lcd.print("FAN ON ");
            break;
        case LOWER_TEMP:
        case LOWER_TEMP_FAST:
            lcd.print("HEAT ON");
    }
}

void fan_led(int state) {
    if (state) {
        digitalWrite(FAN_LED, HIGH);
    }
    else {
        digitalWrite(FAN_LED, LOW);
    }
}

void heat_led(int state) {
    if (state) {
        digitalWrite(HEAT_LED, HIGH);
    }
    else {
        digitalWrite(HEAT_LED, LOW);
    }
}

void fan(int state) {
    if (state) {
        digitalWrite(FAN, HIGH);
    }
    else {
        digitalWrite(FAN, LOW);
    }
}

void alarm(int state) {
    if (state) {
        tone(ALARM, 300, 1000);
    }
    else {
        noTone(ALARM);
    }
}
