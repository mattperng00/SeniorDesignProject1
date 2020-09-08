


#include <stdio.h>
#include thermostat.h

/* ------------------------------ IO and State definitions ------------------------------ */

#define LCD_ROWS 16
#define LCD_COLS 2

// Pin assignments
#define LCD_RS 12
#define LCD_ENABLE 11
#define LCD_D4 5
#define LCD_D5 4
#define LCD_D6 3
#define LCD_D7 2

#define FAN_LED 
#define HEAT_LED 
#define FAN 
#define ALARM 

// TODO: Define IO ports and other things
enum temp_diff {EVEN, LOW, HIGH, VERY_HIGH}
enum temp_scale {CEL, FAHR} // Temperature will be handled internally as Celsius, but can be displayed as Celsius or Fahrenheit
enum device_state {OFF, ON}

/* ------------------------------ State variables and Settings ------------------------------ */
enum temp_diff current_temp_diff = EVEN;
enum temp_scale current_temp_scale = CEL;
volatile int set_temp;

LiquidCrystal lcd(LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

void setup() {
    
    // TODO: Setup temp sensor
    
    
    // Setup temp scale input
    attachInterrupt(digitalPinToInterrupt(), temp_scale_isr, LOW)
    
    // set_temp input device(s)
    
    // LEDs
    pinMode(FAN_LED, OUTPUT);
    pinMode(HEAT_LED, OUTPUT);
    
    // Fan and alarm
    pinMode(FAN, OUTPUT);
    pinMode(ALARM, OUTPUT);
  
    // LCD
    lcd.begin(LCD_ROWS, LCD_COLS);
}

/* ------------------------------ Loop ------------------------------ */
void loop() {
    setup();
    
    while (;;) {
        temp_ctrl(evaluate_temp());
        // Call update hardware funcs
        update_display();
        // Call a delay func that can be interrupted
    }
}

/* ------------------------------ IO functions ------------------------------ */

int read_temp() {
    // int temp = ;
    // TODO: Read in temp as integer
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

void fan_led(device_state ds) {
    // TODO: Turn blue led on off based on ds
    digitalWrite(FAN_LED, (ds == ON) ? HIGH : LOW);
}

void heat_led(device_state ds) {
    // TODO: Turn red led on off based on ds
    digitalWrite(HEAT_LED, (ds == ON) ? HIGH : LOW);
}

void fan(device_state ds) {
    // TODO: Turn fan motor on off based on ds
}

void alarm(device_state ds) {
    // TODO: Turn alarm on off based on ds
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
                    // TODO: Activate blue LED, display "Fan On", turn fan on
                case HIGH:
                    // TODO: Activate red LED, display "Heat On"
                case VERY_HIGH:
                    // TODO: Activate red LED, display "Heat On", turn alarm on
            }
        case LOW:
            switch (td) {
                case EVEN:
                    // TODO: Deactivate blue LED, remove "Fan On"
                case HIGH:
                    // TODO: Deactivate blue LED, remove "Fan On", activate red LED, display "Heat On"
                case VERY_HIGH:
                    // TODO: Deactivate blue LED, remove "Fan On", activate red LED, display "Heat On", turn alarm on
            }
        case HIGH:
            switch (td) {
                case EVEN:
                    // TODO: Deactivate red LED, remove "Heat On"
                case LOW:
                    // TODO: Deactivate red LED, remove "Heat On", activate blue LED, display "Fan On"
                case VERY_HIGH:
                    // TODO: Turn alarm on
            }
        case VERY_HIGH:
            switch (td) {
                case EVEN:
                    // TODO: Deactivate red LED, remove "Heat On", turn alarm off
                case LOW:
                    // TODO: Deactivate red LED, remove "Heat On", turn alarm off, activate blue LED, display "Fan On"
                case HIGH:
                    // TODO: Turn alarm off
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
    //evaluate_temp();
}

void lower_temp_isr() {
    set_temp -= 1;
    //evaluate_temp();
}

void temp_scale_isr() {
    temp_scale = (temp_scale == FAHR) ? CEL : FAHR;
    //update_display();
}


