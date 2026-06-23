#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define IR1 2 // Entry side sensor
#define IR2 3 // Exit side sensor
#define TOTAL_SLOTS 5

Servo myservo;
LiquidCrystal_I2C lcd(0x27, 16, 2);

int entryCount = 0;
int exitCount = 0;

void setup() {
  Serial.begin(9600);
  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);
  myservo.attach(4);
  myservo.write(90); // Gate closed

  lcd.begin(16, 2);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Parking System");
  delay(2000);
  lcd.clear();
  updateLCD();
}

void loop() {
  int ir1 = digitalRead(IR1);
  int ir2 = digitalRead(IR2);

  // Car Entering: IR1 first, then IR2
  if (ir1 == LOW) {
    delay(100);
    if (digitalRead(IR2) == LOW) {
      if (entryCount - exitCount < TOTAL_SLOTS) {
        openGate();
        entryCount++;
        Serial.println("Car Entered");
        lcdMessage("Car Entering...");
      } else {
        Serial.println("Parking Full");
        lcdMessage("Parking Full!");
      }
      closeGate();
      updateLCD();
      waitForSensorsToClear();
    }
  }

  // Car Exiting: IR2 first, then IR1
  if (ir2 == LOW) {
    delay(100);
    if (digitalRead(IR1) == LOW) {
      openGate();
      exitCount++;
      Serial.println("Car Exited");
      lcdMessage("Car Exiting...");
      closeGate();
      updateLCD();
      waitForSensorsToClear();
    }
  }

  delay(100);
}

void openGate() {
  myservo.write(0);  // Open
  delay(1000);
}

void closeGate() {
  myservo.write(90); // Close
  delay(1000);
}

void waitForSensorsToClear() {
  // Wait until both sensors are HIGH again
  while (digitalRead(IR1) == LOW || digitalRead(IR2) == LOW) {
    delay(100);
  }
}

void updateLCD() {
  int available = TOTAL_SLOTS - (entryCount - exitCount);
  if (available < 0) available = 0;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("In:");
  lcd.print(entryCount);
  lcd.print(" Out:");
  lcd.print(exitCount);
  lcd.setCursor(0, 1);
  lcd.print("Available:");
  lcd.print(available);
}

void lcdMessage(String msg) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(msg);
  delay(1500);
}