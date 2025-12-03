// test code

#include <WiFi.h>
#include <WebSocketsServer.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <Wire.h>

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>  
#include "HTML.h"
#include <DNSServer.h>
#include <Arduino.h>

unsigned long screenTimer = 0;
unsigned long screensaverAnimTimer = 0;
unsigned long lastNameSwitch = 0;
int nameIndex = 0;

const char* apPassword = "zerozero"; 

DNSServer dnsServer;

#define TFT_CS 5
#define TFT_DC 16
#define TFT_RST 17

Adafruit_ST7735 display = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 80

#define BATTERY_ADC_PIN 36

const char *ssid = "test_ZERO_CHAT";

AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

const char *userColors[] = {
  "#4927F5", "#00FFFF", "#5F9EA0", "#00008B",
  "#9932CC", "#00BFFF", "#FF1493", "#FF00FF"
};
#define MAX_USERS 8
String userNames[MAX_USERS];

int getBatteryPercent(float v) {
  if (v >= 4.20) return 100;
  if (v >= 4.12) return 95;
  if (v >= 4.08) return 90;
  if (v >= 4.02) return 85;
  if (v >= 3.98) return 80;
  if (v >= 3.94) return 75;
  if (v >= 3.91) return 70;
  if (v >= 3.87) return 65;
  if (v >= 3.85) return 60;
  if (v >= 3.84) return 55;
  if (v >= 3.82) return 50;
  if (v >= 3.80) return 45;
  if (v >= 3.79) return 40;
  if (v >= 3.77) return 35;
  if (v >= 3.75) return 30;
  if (v >= 3.73) return 25;
  if (v >= 3.71) return 20;
  if (v >= 3.69) return 15;
  if (v >= 3.61) return 10;
  if (v >= 3.50) return 5;
  return 0;
}


float readBatteryVoltage() {
  int raw = analogRead(BATTERY_ADC_PIN);
  float vpin = (raw / 4095.0) * 3.3;
  float vbat = vpin * 2.0;

  // compensate ADC error
  vbat *= 1.14;  // adjust until the display matches your DMM

  return vbat;
}


void broadcastJSONMessage(const String &name, const String &message, const String &color) {
  String text = name.length() > 0 ? name + ": " + message : message;
  String json = "{\"text\":\"" + text + "\",\"color\":\"" + color + "\"}";
  webSocket.broadcastTXT(json);
}

String getUserColor(uint8_t id) {
  if (id < MAX_USERS) return String(userColors[id]);
  return "#000";
}

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t len) {
  switch (type) {
    case WStype_CONNECTED:
      webSocket.sendTXT(num, "{\"text\":\"Welcome to ZERO_CHAT!\",\"color\":\"#888\"}");
      break;

    case WStype_TEXT:
      {
        String msg = String((char *)payload);

        // JOIN disabled (still allowed but no LCD change)
        if (msg.startsWith("JOIN|")) {
          userNames[num] = msg.substring(5);
          // updateLCDNames();  // DISABLED
          return;
        }

        String color = getUserColor(num);
        broadcastJSONMessage("", msg, color);
      }
      break;

    case WStype_DISCONNECTED:
      userNames[num] = "";
      // updateLCDNames();  // DISABLED
      break;
  }
}

// =======================
// BATTERY + TITLE DISPLAY
// =======================
void showBatteryScreen() {
  float voltage = readBatteryVoltage();
  int percent = getBatteryPercent(voltage);

  uint16_t bg;
  uint16_t fg;

  if (percent < 30) {
    bg = ST77XX_RED;
    fg = ST77XX_BLACK;
  } else if (percent < 50) {
    bg = ST77XX_YELLOW;
    fg = ST77XX_BLACK;
  } else {
    bg = ST77XX_BLACK;
    fg = ST77XX_GREEN;
  }

  display.fillScreen(bg);

  display.setTextColor(fg);
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("ZERO_CHAT");

  display.setTextSize(1);
  display.setCursor(0, 25);
  display.print("IP: ");
  display.println(WiFi.softAPIP());

  display.setCursor(0, 45);
  display.print("Battery: ");
  display.print(percent);
  display.println("%");
}

/*
// =======================
// ORIGINAL SCREENSAVER (DISABLED)
// =======================
void runScreensaver() { }
*/

// =======================
// ORIGINAL NAME ROTATION (DISABLED)
// =======================
void updateLCDNames() {
  float voltage = readBatteryVoltage();
  int percent = getBatteryPercent(voltage);

  // Background becomes RED when battery < 40%
  if (percent < 40) {
    display.fillScreen(ST77XX_RED);
  } else {
    display.fillScreen(ST77XX_BLACK);
  }

  // ===== Title =====
  display.setTextColor(ST77XX_YELLOW);
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("ZERO_CHAT");

  // ===== IP + Battery =====
  display.setTextSize(1);
  display.setCursor(0, 22);
  display.print("IP: ");
  display.println(WiFi.softAPIP());

  display.setCursor(0, 34);
  display.print("Battery: ");
  display.print(percent);
  display.println("%");

  // ===== User List in Grid =====
  display.setCursor(0, 48);
  display.setTextColor(ST77XX_YELLOW);
  display.println("Users:");

  int x = 0;
  int y = 60;
  int maxWidth = SCREEN_WIDTH - 5;  // 160px - margin

  display.setCursor(x, y);

  for (int i = 0; i < MAX_USERS; i++) {
    if (userNames[i].length() > 0) {
      String name = userNames[i];

      // width in pixels (6px per char with size 1 font)
      int textWidth = name.length() * 6 + 6;  // +6 for ", "

      // wrap to next line if needed
      if (x + textWidth > maxWidth) {
        x = 0;
        y += 10;
      }

      display.setCursor(x, y);
      display.print(name);

      // Add comma if not last entry
      display.print(", ");

      // Move cursor by width
      x += textWidth;
    }
  }
}



void setup() {
  Serial.begin(115200);
  analogReadResolution(12);

  display.initR(INITR_MINI160x80);
  display.setRotation(1);
  display.fillScreen(ST77XX_BLACK);
  display.setTextColor(ST77XX_YELLOW);

  float voltage = readBatteryVoltage();
  int percent = getBatteryPercent(voltage);

  display.setCursor(0, 0);
  display.print("Starting ZERO_CHAT\nBattery: ");
  display.print(percent);
  display.println("%");
  delay(2000);

  WiFi.persistent(false);
  WiFi.disconnect(true, true);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, apPassword);

  IPAddress apIP = WiFi.softAPIP();
  dnsServer.start(53, "*", apIP);

  display.fillScreen(ST77XX_BLUE);
  display.setCursor(0, 0);
  display.println("ZERO_CHAT Ready");
  display.print("IP: ");
  display.println(apIP);
  display.print("Battery: ");
  display.print(percent);
  display.println("%");
  delay(3000);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", index_html);
    response->addHeader("Content-Type", "text/html; charset=utf-8");
    request->send(response);
  });

  server.on("/generate_204", HTTP_GET, [](AsyncWebServerRequest *req) { req->redirect("/"); });
  server.on("/fwlink", HTTP_GET, [](AsyncWebServerRequest *req) { req->redirect("/"); });
  server.on("/hotspot-detect.html", HTTP_GET, [](AsyncWebServerRequest *req) { req->redirect("/"); });
  server.on("/ncsi.txt", HTTP_GET, [](AsyncWebServerRequest *req) { req->redirect("/"); });
  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *req) { req->send(204); });

  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
  server.begin();
}

// =======================
// LOOP (Simplified)
// =======================
void loop() {
  webSocket.loop();
  dnsServer.processNextRequest();

  updateLCDNames();      // always show name list + battery + IP
  delay(500);            // prevent flicker


// =======================
// TEST RAW V DATA
// =======================

// int raw = analogRead(BATTERY_ADC_PIN);

//  display.fillScreen(ST77XX_BLUE);  // clear area
// display.setCursor(0, 0);
// display.print("RAW:");
// display.print(raw);
// // display.display();
// delay(5000);
}
