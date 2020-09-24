/*
    Authors: Ifeanyi Orizu Jr.
    Filename: thermostat.c
*/

#include <LiquidCrystal.h>

/* ------------------------------ Parameters and Types ------------------------------ */

// Parameters
#define LCD_COLS 16
#define LCD_ROWS 2
#define BAUD_RATE 9600
#define TEMP_READ_FREQ 10
#define ALARM_FREQ 600
#define ALARM_DURATION 1000

// Pin assignments
#define LCD_RS 12
#define LCD_EN 11
#define LCD_D4 5
#define LCD_D5 4
#define LCD_D6 3
#define LCD_D7 2
#define TEMP_SENSOR A0
#define FAN_LED 6
#define HEAT_LED 7
#define FAN 9
#define ALARM 8
#define TEMP_INC 
#define TEMP_DEC 
#define TEMP_SCALE 

// Function prototypes
enum temp_status evaluate_temp(float nominal_temp);
void temp_ctrl(temp_status ts);
float read_temp();
void serial_print(float nominal_temp);
void update_display();
void fan_led_driver(int state);
void heat_led_driver(int state);
void fan_driver(int state);
void alarm_driver(int state);

// Enumerated types
enum temp_scale {CEL, FAHR}; // Temperature will be handled internally as Celsius, but can be displayed as Celsius or Fahrenheit
enum temp_status {NO_ACTION, RAISE_TEMP, LOWER_TEMP, LOWER_TEMP_ALARM};

/* ------------------------------ State variables and Settings ------------------------------ */
volatile enum temp_status current_temp_status = NO_ACTION;
volatile enum temp_scale current_temp_scale = CEL;
volatile float set_temp;
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

void setup() {
    // Serial print out
    Serial.begin(BAUD_RATE);
    
    // LEDs
    pinMode(FAN_LED, OUTPUT);
    pinMode(HEAT_LED, OUTPUT);
    
    // Fan and Alarm
    pinMode(FAN, OUTPUT);
    pinMode(ALARM, OUTPUT);
    
    // LCD
    lcd.begin(LCD_COLS, LCD_ROWS);
    
    // Interrupts
    attachInterrupt(digitalPinToInterrupt(TEMP_SCALE), temp_scale_isr, ); // LOW, CHANGE, RISING, FALLING https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
    attachInterrupt(digitalPinToInterrupt(TEMP_INC), increment_temp_isr, );
    attachInterrupt(digitalPinToInterrupt(TEMP_DEC), decrement_temp_isr, );
}


/* ------------------------------ State functions ------------------------------ */

void loop() {
    float temp;
    enum temp_status ts;
    
    while (1) {
        temp = read_temp();
        serial_print(temp);
        ts = evaluate_temp(temp);
        temp_ctrl(ts);
        update_display();
        delay(1000/TEMP_READ_FREQ); // TODO: I want this evaluated at compile time
    }
}

enum temp_status evaluate_temp(float nominal_temp) {
    float temp_difference;
    enum temp_status ts;
    
    noInterrupts();
    temp_difference = (current_temp_scale == CEL) ? (nominal_temp - set_temp) : 9*(nominal_temp - set_temp)/5;
    interrupts();
    
    if (temp_difference > 0) {
        if (temp_difference >= 10) { // TODO: If it's in FAHR mode shouldn't the alarm threshold be 10 (F), not 10 (C)?
            ts = LOWER_TEMP_ALARM;
        }
        else {
            ts = LOWER_TEMP;
        }
    }
    else if (temp_difference < 0) {
        ts = RAISE_TEMP;
    }
    else {
        ts = NO_ACTION;
    }
    
    return ts;
}

void temp_ctrl(enum temp_status ts) {
    switch (ts) {
        case NO_ACTION:
            heat_led_driver(0);
            fan_led_driver(0);
            fan_driver(0);
            alarm_driver(0);
            break;
        case RAISE_TEMP:
            heat_led_driver(1);
            fan_led_driver(0);
            fan_driver(0);
            alarm_driver(0);
            break;
        case LOWER_TEMP:
            heat_led_driver(0);
            fan_led_driver(1);
            fan_driver(1);
            alarm_driver(0);
            break;
        case LOWER_TEMP_ALARM:
            heat_led_driver(0);
            fan_led_driver(1);
            fan_driver(1);
            alarm_driver(1);
    }
    current_temp_status = ts;
}


/* ------------------------------ IO functions ------------------------------ */

float read_temp() { // TODO: Wrong formula according to Nate U (Looked jank to me)
    return 0.488155*(analogRead(TEMP_SENSOR) - 20) - 40;
}

void serial_print(float nominal_temp) { // TODO: Is this a crtical section?
    switch (current_temp_scale) {
        case CEL:
            Serial.print((int)nominal_temp);
            Serial.println(" C");
            break;
        case FAHR:
            nominal_temp = 9*nominal_temp/5 + 32;
            Serial.print((int)nominal_temp);
            Serial.println(" F");
    }
}

void update_display() {
    noInterrupts();
    float temp = (current_temp_scale == CEL) ? set_temp : 9*set_temp/5 + 32;
    char temp_unit = (current_temp_scale == CEL) ? 'C' : 'F';
    interrupts();
    
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.setCursor(7, 0);
    lcd.print((int)temp);
    lcd.setCursor(10, 0);
    lcd.print(temp_unit);
    
    lcd.setCursor(0, 1);
    switch (current_temp_status) {
        case NO_ACTION:
            lcd.print("       ");
            break;
        case RAISE_TEMP:
            lcd.print("FAN ON ");
            break;
        case LOWER_TEMP:
        case LOWER_TEMP_ALARM:
            lcd.print("HEAT ON");
    }
}

void fan_led_driver(int state) {
    if (state) {
        digitalWrite(FAN_LED, LOW);
    }
    else {
        digitalWrite(FAN_LED, HIGH);
    }
}

void heat_led_driver(int state) {
    if (state) {
        digitalWrite(HEAT_LED, LOW);
    }
    else {
        digitalWrite(HEAT_LED, HIGH);
    }
}

void fan_driver(int state) {
    if (state) {
        digitalWrite(FAN, LOW);
    }
    else {
        digitalWrite(FAN, HIGH);
    }
}

void alarm_driver(int state) {
    if (state) {
        tone(ALARM, ALARM_FREQ, ALARM_DURATION);
    }
    else {
        noTone(ALARM);
    }
}

/* ------------------------------ ISRs ------------------------------ */

void temp_scale_isr() {
    current_temp_scale = (current_temp_scale == CEL) ? FAHR : CEL;
}

void increment_temp_isr() {
    if (current_temp_scale == FAHR) { // TODO: I can do better than this
        float new_temp = 9*set_temp/5 + 32;
        new_temp++;
        set_temp = 5*(new_temp - 32)/9;
    }
    else {
        set_temp++;
    }
}

void decrement_temp_isr() {
    if (current_temp_scale == FAHR) {
        float new_temp = 9*set_temp/5 + 32;
        new_temp--;
        set_temp = 5*(new_temp - 32)/9;
    }
    else {
        set_temp--;
    }
}