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

### Case / Enclosure  
<img width="50%" height="50%" alt="Case" src="https://github.com/user-attachments/assets/cddca08e-17c8-4f4f-ae5d-974c98a60010" />

### TFT Status Screen  
<img width="50%" height="50%" alt="Case" src="https://github.com/user-attachments/assets/f28a3d5c-b578-4463-b068-2b543b0c30ae" />

## Hardware Summary
- ESP32-WROOM-32U (external antenna)
- ST7735 160×80 TFT (SPI)
- Li-ion battery
- HW-373 TP4056 charger
- Custom hand-solderable PCB
- 3D-printed enclosure

## TFT Pin Connections

| TFT Pin  | ESP32 Pin |
|----------|-----------|
| CS       | 5         |
| DC       | 16        |
| RST      | 17        |
| SCLK     | 18        |
| MOSI     | 23        |
| Backlight | 3.3V     |

**Battery ADC pin:** 36

### Battery Measurement Logic
```
vpin = (raw / 4095.0) * 3.3;
vbat = vpin * 2.0;
vbat *= 1.14; // calibrated
```

A lookup table converts voltage to percentage.

## Networking

The ESP32 provides:
- A Wi-Fi Access Point
- DNS spoofing for captive portal
- WebSockets for chat
- Built-in HTML/JS UI stored in index_html

Clients connecting to the AP are redirected to the chat page automatically.

## Web Interface
- Real-time messaging
- User color coding
- Mobile + desktop compatible
- Auto-scroll, WebSockets backend

## Firmware Structure

### setup()
- Initialize TFT
- Start AP
- Start DNS and WebSockets
- Show boot splash

### loop()
- Process DNS
- Process WebSockets
- Update TFT:
  - IP
  - Battery
  - User list

Disabled features:
- Screensaver
- Animation effects (removed for stability)

## Repository Structure
```
/ZERO_CHAT
 ├── src/
 │    ├── main.cpp
 │    ├── HTML.h
 │    └── board_config.h
 ├── pcb/
 │    ├── ZERO_CHAT_v1.fzz
 │    └── ZERO_CHAT_v1_gerbers.zip
 ├── enclosure/
 │    ├── front.stl
 │    └── back.stl
 ├── images/
 │    ├── LogInPM.png
 │    ├── zero_chat_webui.png
 │    ├── zero_chat_case.jpg
 │    └── zero_chat_tft.jpg
 ├── README.md
 └── LICENSE
```

## How to Build

1. Install libraries:
   - ESPAsyncWebServer
   - AsyncTCP
   - Adafruit_GFX
   - Adafruit_ST7735
   - DNSServer

2. Flash using PlatformIO or Arduino IDE.

3. Device creates AP automatically:
   - SSID: test_ZERO_CHAT
   - Password: zerozero

Open a browser → captive portal loads chat UI.

## Future Improvements
- BLE fallback
- Message encryption
- SPIFFS message logging
- Better mobile layout
- Multi-device mesh network

## License
MIT License

## Author
Kevin Kearns — Embedded / IoT Developer
Self-taught MCU developer
Web Dev Bootcamp Graduate
Creator of the Intro to Arduino Course

