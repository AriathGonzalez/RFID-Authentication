#include <SPI.h>
#include <MFRC522.h>

// Pin definitions
const int RED_PIN = 8;
const int GREEN_PIN = 9;
const int BLUE_PIN = 10;

// RFID reader pins
#define RST_PIN 5
#define SS_PIN 53

// Master and Slave tag IDs
const String MasterTag = "96501430";  // Replace with your Master Tag ID
const String SlaveTag = "9B24524C";    // Replace with your Secondary Tag ID
String tagID = "";

// Timeout for granting access (in milliseconds)
const unsigned long accessTimeout = 5000;

// Instance of the MFRC522 RFID reader
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  // Initializing serial communication and RFID reader
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  // Setting LED pins as output
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  // Printing initialization message
  Serial.println("Access Control");
  Serial.println("Scan Your Card>>");
}

void loop() {
  // Setting LED to standby color (Yellow)
  setColor(255, 255, 0);

  // Waiting until new tag is available
  while (getID()) {
    // Checking if Master Tag is detected
    if (tagID == MasterTag) {
      Serial.println("Master Card Detected");

      // Setting LED to indicating color (Cyan)
      setColor(0, 255, 255);

      // Starting the timer
      unsigned long startTime = millis();

      // Waiting for Slave Tag within the timeout period
      while (millis() - startTime < accessTimeout) {
        if (getID() && tagID == SlaveTag) {
          // Slave Tag detected within the timeout period
          Serial.println("Slave Card Detected");
          Serial.println("Access Granted!");

          // Setting LED to granted color (Green)
          setColor(0, 255, 0);

          // Additional code for granting access (e.g., opening doors)
          delay(2000); // Keeping the LED on for a while

          // Resetting LED to standby color (Yellow)
          setColor(255, 255, 0);
          break; // Exiting the loop once access is granted
        }
      }

      // Access denied if Slave Tag not detected within the timeout period
      if (tagID != SlaveTag) {
        Serial.println("Access Denied! Slave Card Not Detected within the timeout period");

        // Setting LED to denied color (Red)
        setColor(255, 0, 0);

        delay(2000); // Keeping the LED on for a while

        // Resetting LED to standby color (Yellow)
        setColor(255, 255, 0);
      }

      break; // Exiting the loop once Master Card is detected and processed
    }
  }
}

// Function to read new tag if available
boolean getID() {
  // Getting ready for reading PICCs
  if (!mfrc522.PICC_IsNewCardPresent()) {
    // If a new PICC is not present, continue
    return false;
  }

  // Since a PICC is present, get the serial
  if (!mfrc522.PICC_ReadCardSerial()) {
    return false;
  }

  // Concatenating the tag ID bytes into a single string variable
  tagID = "";
  for (uint8_t i = 0; i < 4; i++) {
    tagID.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  tagID.toUpperCase();

  // Stopping reading
  mfrc522.PICC_HaltA();
  return true;
}

// Function to set LED color
void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(RED_PIN, redValue);
  analogWrite(GREEN_PIN, greenValue);
  analogWrite(BLUE_PIN, blueValue);
}
