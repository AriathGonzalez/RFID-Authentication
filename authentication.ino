#include <SPI.h>
#include <MFRC522.h>
#include <Ethernet.h>
#include <SD.h>

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

// Variables needed for Web Server
byte mac[] = {0xA8, 0x61, 0x0A, 0xAE, 0x98, 0x24};  // Replace w/ MAC of ethernet shield
byte ip[] = { 192, 168, 137, 174 }; // ip in lan; run ipconfig, then under Ethernet adapter Ethernet, select any available host
EthernetServer server(80);
File webPage;
const String welcomePageHTM = "index.htm";
const String successPageHTM = "success.htm";
const String errorPageHTM = "error.htm";
String currentPage = welcomePageHTM;

// Instance of the MFRC522 RFID reader
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  // Initializing serial communication
  Serial.begin(9600);
  
  setupSD();
  setupRFID();
  setupLED();
  setupServer();
  
  Serial.println("Access Control");
  Serial.println("Scan Your Card>>");
}

void setupSD(){
  Serial.println("Initializing SD card...");
  if (!SD.begin(4)) {
    Serial.println("ERROR - SD card initialization failed!");
    return;
  }
  Serial.println("SUCCESS - SD card initialized.");
  
  if (!SD.exists(welcomePageHTM)){
    Serial.print("ERROR - Can't find ");
    Serial.print(welcomePageHTM);
    Serial.println(" file!");
    return;
  }
  if (!SD.exists(successPageHTM)){
    Serial.print("ERROR - Can't find ");
    Serial.print(successPageHTM);
    Serial.println(" file!");
    return;
  }
  if (!SD.exists(errorPageHTM)){
    Serial.print("ERROR - Can't find ");
    Serial.print(errorPageHTM);
    Serial.println(" file!");
    return;
  }  
  Serial.println("SUCCESS - All files found");
}

void setupRFID() {
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("SUCCESS - RFID set up complete!");
}

void setupLED() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  Serial.println("SUCCESS - LED set up complete!");
}

void setupServer() {
  Ethernet.begin(mac, ip);  // (mac, ip);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  
  server.begin();
  Serial.print("Server is at ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  serveClient();
}

void serveClient() {
  EthernetClient client = server.available();
  if (client) {
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n' && currentLineIsBlank) {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();

          webPage = SD.open(currentPage);
          if (webPage) {
            while (webPage.available()) {
              client.write(webPage.read());
            }
            webPage.close();
          }
          break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    //delay(1);
    client.stop();
  }
  detectRFID();
}

void detectRFID() {
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
          currentPage = successPageHTM;
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
        currentPage = errorPageHTM;
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
