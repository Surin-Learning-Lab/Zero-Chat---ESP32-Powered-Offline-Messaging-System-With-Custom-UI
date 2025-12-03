# ZERO_CHAT — ESP32 Offline Chat Device with TFT Status Display

ZERO_CHAT is an offline, self-contained communication terminal built using the ESP32.
It creates its own Wi-Fi network and captive portal, allowing nearby users to connect and chat without internet, a router, or cloud services.

A compact ST7735 TFT display shows real-time system status:
- IP address
- Battery voltage + percentage
- Connected user names

This project demonstrates a full embedded workflow: networking, firmware, UI rendering, power monitoring, PCB design, and enclosure fabrication.

## Features

### Offline messaging
- ESP32 runs in Access Point mode
- Captive portal via DNS redirection
- Web-based chat UI automatically opens
- WebSockets for instant messaging (low latency)

### TFT status display
- 160×80 ST7735 TFT
- Shows:
  - Device IP
  - Battery %
  - Color-coded voltage state
  - Live user list
- Updates every 500 ms with minimal flicker

### Battery-powered operation
- Li-ion monitoring using ADC and calibration factor
- Automatic color states:
  - Green: > 50%
  - Yellow: 30–50%
  - Red: < 30%

## Project Images


### Login Prompt  
<img width="50%" height="50%" alt="login_with_name" src="https://github.com/user-attachments/assets/3b1b5891-8678-4060-adcd-fb5515c747da" />

### Chat UI  
<img width="50%" height="50%" alt="UI_chat" src="https://github.com/user-attachments/assets/0b97623a-696e-4c86-ab52-f3a0765b082a" />
