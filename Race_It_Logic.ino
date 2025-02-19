#include <Wire.h>
#include <TM1637.h>
#include <LiquidCrystal_I2C.h>

// Define the LCD address
LiquidCrystal_I2C lcd(0x27, 16, 2); // 16x2 LCD

// Create an instance of the TM1637 class
TM1637 tm1637(CLK, DIO);


// Initialize Sensor Pins Here


// Variables for Logic



void setup() {

  // **********************  LCD  ***********************************
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);

  // **************  Seven Segment Display **************************
  tm1637.init();
  tm1637.setBrightness(7);
  // tm1637.display(position, value), where position is from 0-3

  // ****************  Break Beam Sensors  **************************
  pinMode(sensorPin, INPUT_PULLUP);

  // ******************  Passive Buzzer  ****************************
  pinMode(buzzerPin, OUTPUT);

  // ****************  Ultrasonic Sensor  ***************************
  pinMode(sensorTrigPin, OUTPUT);
  pinMode(sensorEchoPin, INPUT);

  // ********************  Buttons  *********************************
  pinMode(resetButton, INPUT_PULLDOWN)
  pinMode(joystickButton, INPUT_PULLDOWN);

  // *******************  Debugging  ********************************
  Serial.begin(9600);
}

void loop() {

  // ****************************************************************
  // **************** NEEDED VARIABLES ******************************
  // ****************************************************************

  // Variables for cursor position on LCD via Joystick (0 = 30-sec, 1 = 60-sec)
  int cursorPos = 0;  // 0 for "30-sec", 1 for "60-sec"
  bool modeSelected = false;  // To track if a mode is selected
  int selectedMode = 0;  // 0 for "30-sec", 1 for "60-sec"
  
  // ****************************************************************
  // **************** GETTING MODE INPUT ****************************
  // ****************************************************************

  // Read joystick X-axis value
  int joystickXValue = analogRead(jotstickXDirection);

  // Map joystick value to move the cursor left (0) or right (1)
  if (joystickXValue < 400) {
    cursorPos = 0;  // Move to "30-sec"
  } else if (xValue > 600) {
    cursorPos = 1;  // Move to "60-sec"
  }

  // Update the LCD with the cursor position
  lcd.setCursor(0, 1);
  if (cursorPos == 0) {
    lcd.print("->30-sec  60-sec");
  } else {
    lcd.print("30-sec ->60-sec ");
  }

  // Check if the button is pressed to select the mode
  int buttonState = digitalRead(joystickButton);

  if (buttonState == LOW) {  // Button is pressed
    modeSelected = true;  // Mark the mode as selected
    selectedMode = cursorPos;  // Store the selected mode (0 for 30-sec, 1 for 60-sec)
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Selected Mode:");
    lcd.setCursor(0, 1);
    if (selectedMode == 0) {
      lcd.print("30-sec");
    } else {
      lcd.print("60-sec");
    }
    delay(1000);  // Debounce delay to avoid multiple reads
  }

  // ****************************************************************
  // **************** GETTING MODE INPUT ****************************
  // ****************************************************************





  // If a mode is selected, stop updating cursor and hold the selection
  if (modeSelected) {
    // You can perform any further actions based on the selected mode
    // For now, we'll just stop moving the cursor
    while (true) {
      // This will hold the selected mode on the LCD until the next reset
    }
  }

  delay(100);  // Delay to reduce flickering

}
