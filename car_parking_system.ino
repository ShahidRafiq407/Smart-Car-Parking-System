#include <Servo.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Pins define karein (As per your current code)
const int irSensor1 = 3; // Entry Sensor Pin 3 par
const int irSensor2 = 4; // Exit Sensor Pin 4 par
const int servoPin  = 9; // Servo Motor Pin 9 par

Servo myServo;
// LCD address aamtaur par 0x27 ya 0x3F hota hai (16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2); 

const int totalSlots = 4;
int occupiedSlots = 0; 

void setup() {
  Serial.begin(9600);
  
  pinMode(irSensor1, INPUT);
  pinMode(irSensor2, INPUT);
  
  myServo.attach(servoPin);
  myServo.write(0); // Shuruat me gate band (0 degree)
  
  // LCD Initialize karein
  lcd.init();
  lcd.backlight();
  
  // Welcome Screen
  lcd.setCursor(0, 0);
  lcd.print(" SMART PARKING ");
  lcd.setCursor(0, 1);
  lcd.print("  SYSTEM READY  ");
  Serial.println("--- SMART PARKING SYSTEM READY ---");
  delay(2000); 
  
  updateDisplay(); // Pehli baar screen status dekhane ke liye
}

void loop() {
  int s1 = digitalRead(irSensor1);
  int s2 = digitalRead(irSensor2);

  // ================= 1. ENTRY SCENARIO (Pin 3 First) =================
  if (s1 == LOW && s2 == HIGH) { 
    if (occupiedSlots < totalSlots) {
      
      // LCD & Serial par Gate Open status
      Serial.println("\n[ENTRY] First Detection at D3 -> Gate Open!");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("  GATE OPENING  ");
      lcd.setCursor(0, 1);
      lcd.print(" PLEASE ENTER.. ");
      
      myServo.write(90); // Gate khol do
      
      // 4 second ka timeout
      unsigned long startTime = millis();
      bool crossedD4 = false;
      
      while (millis() - startTime < 4000) {
        if (digitalRead(irSensor2) == LOW) { 
          crossedD4 = true;
          break;
        }
      }
      
      if (crossedD4) {
        occupiedSlots++; 
        Serial.println("[SUCCESS] Car Entered successfully!");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("  CAR ENTERED   ");
        lcd.setCursor(0, 1);
        lcd.print("  SUCCESSFULLY  ");
      } else {
        Serial.println("[TIMEOUT/CANCEL] Gate open hua par gadi andar nahi gayi.");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("TIMEOUT / CANCEL");
      }
      
      delay(1000); // Gate 1 second khula rahega
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("  GATE CLOSING  ");
      myServo.write(0);  // Gate Band
      delay(1000);       
      
      updateDisplay(); // Wapas slots status dikhao
    } 
    else {
      Serial.println("\n[ALERT] Parking FULL! Entry Denied.");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("  PARKING FULL  ");
      lcd.setCursor(0, 1);
      lcd.print(" ENTRY DENIED!  ");
      delay(2000);
      updateDisplay();
    }
  }

  // ================= 2. EXIT SCENARIO (Pin 4 First) =================
  if (s2 == LOW && s1 == HIGH) { 
    if (occupiedSlots > 0) {
      
      Serial.println("\n[EXIT] First Detection at D4 -> Gate Open!");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("  GATE OPENING  ");
      lcd.setCursor(0, 1);
      lcd.print("  PLEASE EXIT.. ");
      
      myServo.write(90); // Gate khol do
      
      // 4 second ka timeout
      unsigned long startTime = millis();
      bool crossedD3 = false;
      
      while (millis() - startTime < 4000) {
        if (digitalRead(irSensor1) == LOW) { 
          crossedD3 = true;
          break;
        }
      }
      
      if (crossedD3) {
        occupiedSlots--; 
        Serial.println("[SUCCESS] Car Left successfully!");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("    CAR LEFT    ");
        lcd.setCursor(0, 1);
        lcd.print("  SUCCESSFULLY  ");
      } else {
        Serial.println("[TIMEOUT/CANCEL] Gate open hua par gadi baahir nahi gayi.");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("TIMEOUT / CANCEL");
      }
      
      delay(3000); // Gate 1 second khula rahega
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("  GATE CLOSING  ");
      myServo.write(0);  // Gate Band
      delay(1000);       
      
      updateDisplay(); // Wapas slots status dikhao
    } 
    else {
      Serial.println("\n[ALERT] Parking empty! No car to exit.");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" PARKING EMPTY! ");
      lcd.setCursor(0, 1);
      lcd.print(" NO CAR INSIDE  ");
      delay(2000);
      updateDisplay();
    }
  }
}

// LCD aur Serial par hamesha live status update karne ka function
void updateDisplay() {
  int remainingSlots = totalSlots - occupiedSlots;
  
  // Serial Monitor Output
  Serial.print("Total Slots: "); Serial.print(totalSlots);
  Serial.print(" | Occupied: "); Serial.print(occupiedSlots);
  Serial.print(" | Remaining Slots: "); Serial.println(remainingSlots);
  Serial.println("-------------------------------------------");

  // I2C LCD Output
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Available: ");
  lcd.print(remainingSlots); // Khali slots dikhayega
  
  lcd.setCursor(0, 1);
  lcd.print("Occupied : ");
  lcd.print(occupiedSlots);  // Bhari hui slots dikhayega
}