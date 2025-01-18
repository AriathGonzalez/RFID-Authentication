# RFID-Authentication

This project demonstrates the security capabilities of RFID technology through a dual-factor authentication system. It uses NFC cards and an Arduino Mega 2560 to create a mock web server that grants access only when primary and secondary cards are scanned within a 5-second window.  

## Components
- RFID-RC522 Module
- Arduino Mega 2560
- NFC Cards (x3)
- RGB LED
- Ethernet Shield 2 (optional)
  
## Physical Setup  

### RC522 Module Configuration  
The RC522 module was wired to the Arduino Mega 2560 based on the MFRC522 Arduino library pin layout:  
- **RST** → Pin 5  
- **SDA (SS)** → Pin 53  
- **MOSI** → Pin 51  
- **MISO** → Pin 50  
- **SCK** → Pin 52  

### RGB LED Integration  
An RGB LED was added for status indications:  
- **Yellow**: Standby mode  
- **Cyan**: Primary card scanned  
- **Green**: Secondary card scanned successfully within 5 seconds  
- **Red**: Authentication failed (secondary card not scanned in time)  

RGB LED pins were connected as follows:  
- **Red Pin** → Pin 8  
- **Green Pin** → Pin 9  
- **Blue Pin** → Pin 10  

### Network Configuration  
The Arduino required a manually assigned IP address due to DHCP issues. The IP was set within the LAN range (e.g., `192.168.137.0` to `192.168.137.255`).  

## Authentication Workflow  

The system follows these steps:  
1. **Standby Mode**: The RGB LED glows yellow, awaiting the primary card.  
2. **Primary Card**: Upon scanning, the LED turns cyan, signaling the system is ready for the secondary card.  
3. **Secondary Card**: If scanned within 5 seconds, the LED turns green, granting access. If not, the LED flashes red for 2 seconds, indicating failure.  

## Installation and Usage  

### Installation  
1. **Hardware Setup**  
   - Connect the RC522 module to the Arduino Mega 2560 following the pin layout above.  
   - Wire the RGB LED to pins 8, 9, and 10 for red, green, and blue, respectively.  
   - If needed, attach an Ethernet Shield 2 for web server functionality.  

2. **Software Setup**  
   - Install the Arduino IDE: [Download Here](https://www.arduino.cc/en/software).  
   - Install the **MFRC522** library in the Arduino IDE:  
     1. Go to **Sketch > Include Library > Manage Libraries**.  
     2. Search for **MFRC522** and click **Install**.  

3. **Load the Code**  
   - Open the `authentication.ino` file in the Arduino IDE.  
   - Verify and upload the code to the Arduino Mega 2560.  

4. **Network Configuration**  
   - Manually assign an IP address in the code to match your LAN range.  

### Usage  
1. Power on the Arduino setup and ensure all connections are secure.  
2. Place the primary RFID card near the reader; the RGB LED will turn cyan.  
3. Within 5 seconds, place the secondary RFID card near the reader.  
   - **Success**: The LED will turn green, and access is granted.  
   - **Failure**: If the time limit is exceeded, the LED will flash red.  
