/* e-paper display lib */
#include <GxEPD.h>

// correct display for b/w/r badgy
#include <GxGDEW029Z10/GxGDEW029Z10.h>    // 2.9" b/w/r
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>

/* include any other fonts you want to use https://github.com/adafruit/Adafruit-GFX-Library */
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeSansBoldOblique12pt7b.h>

/* WiFi  libs*/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiManager.h>

// uuid-ish
#include <sha256.h>

// qr code generation
#include <qrcode.h>

// shadowman display
#include "shadowman.c"

// get system chip id which is really the chip MAC address
// we don't want to leak this upstream but we can do something with it...
// like seeding a pRNG to create a UUID
// as a component of the UUID...
// uh, UUID stuff
const uint32_t _chipId = system_get_chip_id();
char* salt = "redhat badgy"; // this could be a random salt but that might be paranoid
uint8_t saltlen = 12;
char uuid[64];

/* change this to your name, you may have to adjust font size and cursor location in showHello() to perfectly center the text */
const char* name = "Mark Shoger";
const char* title = "Manager, ICSA Team";
const char* group = "NAPS - NSP";
const char* loc = "RHCE# 150-169-852";
const char* rhcn = "mshoger@redhat.com";

/* Always include the update server, or else you won't be able to do OTA updates! */
/**/const int port = 8888;
/**/ESP8266WebServer httpServer(port);
/**/ESP8266HTTPUpdateServer httpUpdater;
/*                                                                                */

/* Configure pins for display */
GxIO_Class io(SPI, SS, 0, 2);
GxEPD_Class display(io); // default selection of D4, D2

/* A single byte is used to store the button states for debouncing */
byte buttonState = 0;
byte lastButtonState = 0;   //the previous reading from the input pin
unsigned long lastDebounceTime = 0;  //the last time the output pin was toggled
unsigned long debounceDelay = 50;    //the debounce time

void setup()
{
  // create chip hash
  SHA256 sha256;
  sha256.add(salt, saltlen);
  sha256.add(&_chipId, 4);
  sprintf(uuid, "%s", sha256.getHash());

  display.init();

  pinMode(1, INPUT_PULLUP); //down
  pinMode(3, INPUT_PULLUP); //left
  pinMode(5, INPUT_PULLUP); //center
  pinMode(12, INPUT_PULLUP); //right
  pinMode(10, INPUT_PULLUP); //up

  /* Enter OTA mode if the center button is pressed */
  if (digitalRead(5)  == 0) {
    /* WiFi Manager automatically connects using the saved credentials, if that fails it will go into AP mode */
    WiFiManager wifiManager;
    wifiManager.setAPCallback(configModeCallback);
    wifiManager.autoConnect("Badgy AP");
    /* Once connected to WiFi, startup the OTA update server if the center button is held on boot */
    httpUpdater.setup(&httpServer);
    httpServer.begin();
    showIP();
    while (1) {
      httpServer.handleClient();
    }
  }

  showHello(); //show "Hello my name is" immediately on boot
}

void loop()
{
  byte reading =  (digitalRead(1)  == 0 ? 0 : (1 << 0)) | //down
                  (digitalRead(3)  == 0 ? 0 : (1 << 1)) | //left
                  (digitalRead(5)  == 0 ? 0 : (1 << 2)) | //center
                  (digitalRead(12) == 0 ? 0 : (1 << 3)) | //right
                  (digitalRead(10) == 0 ? 0 : (1 << 4)); //up

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      for (int i = 0; i < 5; i++) {
        if (bitRead(buttonState, i) == 0) {
          switch (i) {
            case 0:
              //do something when the user presses down

              break;
            case 1:
              //do something when the user presses left

              break;
            case 2:
              //do something when the user presses center

              break;
            case 3:
              //do something when the user presses right

              break;
            case 4:
              //do something when the user presses up

              break;
            default:
              break;
          }
        }
      }
    }
  }
  lastButtonState = reading;
}

void configModeCallback (WiFiManager *myWiFiManager) {
  display.setRotation(3); //even = portrait, odd = landscape
  display.fillScreen(GxEPD_WHITE);
  const GFXfont* f = &FreeMonoBold9pt7b ;
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.setCursor(0, 50);
  display.println("Connect to Badgy AP");
  display.println("to setup your WiFi!");
  display.update();
}

void showText(const char *text)
{
  display.setRotation(3); //even = portrait, odd = landscape
  display.fillScreen(GxEPD_WHITE);
  const GFXfont* f = &FreeMonoBold9pt7b ;
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.setCursor(10, 70);
  display.println(text);
  display.update();
}

void showIP() {
  display.setRotation(3); //even = portrait, odd = landscape
  display.fillScreen(GxEPD_WHITE);
  const GFXfont* f = &FreeMonoBold9pt7b ;
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.setCursor(0, 10);

  String url = WiFi.localIP().toString() + ":" + String(port) + "/update";
  byte charArraySize = url.length() + 1;
  char urlCharArray[charArraySize];
  url.toCharArray(urlCharArray, charArraySize);

  display.println("You are now connected!");
  display.println("");
  display.println("Go to:");
  display.println(urlCharArray);
  display.println("to upload a new sketch.");
  display.update();
}

unsigned char reverse(unsigned char b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

void writeBitmap(const unsigned char *bitmap, uint16_t x, uint16_t y, uint16_t w, uint16_t h, int color, boolean invert) {
  uint16_t cx = 0;
  uint16_t cy = 0;

  unsigned char mask;

  for(int ib = 0; ib < ((h/8+1) * w); ib++) {
    mask = reverse(bitmap[ib]);
    if (invert) {
      mask = ~mask;
    }
    if (mask == 0) {
      cx += 8;
      if(cx >= w) {
        cy++;
        cx = 0;
      }
    } else {
      for (unsigned char bit_index = 0; bit_index < 8; bit_index++) {
        if (mask & 1 > 0) {
          display.drawPixel(cx + x, cy + y, color);
        }
        // move one right
        cx++;
        if(cx >= w) {
          cy++;
          cx = 0;
          break;
        }
        // shift mask
        mask >>= 1;
      }
    }
    if(h <= cy) {
      break;
    }
  }
}

void writeTwo(const unsigned char *bitmap1, const unsigned char *bitmap2, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color1, uint16_t color2, boolean invert) {
  uint16_t cx = 0;
  uint16_t cy = 0;

  unsigned char mask1;
  unsigned char mask2;

  for(int ib = 0; ib < ((h/8+1) * w); ib++) {
    mask1 = reverse(bitmap1[ib]);
    mask2 = reverse(bitmap2[ib]);
    if (invert) {
      mask1 = ~mask1;
      mask2 = ~mask2;
    }
    if (mask1 == 0 && mask2 == 0) {
      cx += 8;
      if(cx >= w) {
        cy++;
        cx = 0;
      }
    } else {
      for (unsigned char bit_index = 0; bit_index < 8; bit_index++) {
        if (mask2 & 1 > 0) {
          display.drawPixel(cx + x, cy + y, color2);
        } else if (mask1 & 1 > 0) {
          display.drawPixel(cx + x, cy + y, color1);
        }
        // move one right
        cx++;
        if(cx >= w) {
          cy++;
          cx = 0;
          break;
        }
        // shift masks
        mask1 >>= 1;
        mask2 >>= 1;
      }
    }
    if(h <= cy) {
      break;
    }
  }
  Serial.print("Left writeTwo: cx=");
  Serial.print(cx);
  Serial.print(", cy=");
  Serial.print(cy);
}


// show shadowman at x/y in
void showShadowmanIcon(uint16_t x, uint16_t y) {
  writeTwo(bwshadowman, oldredhat, x, y, 50, 38, GxEPD_BLACK, GxEPD_RED, true);
}

void showRedHatIcon(uint16_t x, uint16_t y) {
  writeBitmap(newredhat, x, y, 45, 38, GxEPD_RED, false);
}

void showQR(uint16_t ix, uint16_t iy, uint16_t box_size, uint16_t qr_version, uint16_t err_correction, const char *data, bool forcewrite) {
  uint16_t box_x = ix;
  uint16_t box_y = iy;
  uint16_t init_x = box_x;

  // The structure to manage the QR code
  QRCode qrcode;

  // Allocate a chunk of memory to store the QR code
  uint8_t qrcodeBytes[qrcode_getBufferSize(qr_version)];

  qrcode_initText(&qrcode, qrcodeBytes, qr_version, err_correction, data);

  // display bitmap
  for (uint8_t y = 0; y < qrcode.size; y++) {
    // Each horizontal module
    for (uint8_t x = 0; x < qrcode.size; x++) {
      if (qrcode_getModule(&qrcode, x, y)) {
        display.fillRect(box_x, box_y, box_size, box_size, GxEPD_BLACK);
      } else if (forcewrite) {
        display.fillRect(box_x, box_y, box_size, box_size, GxEPD_WHITE);
      }
      box_x = box_x + box_size;
    }
    box_y = box_y + box_size;
    box_x = init_x;
  }
}

void graphics() {
  // qr
  showQR(227, 63, 2, 4, ECC_LOW, "https://www.linkedin.com/in/mshoger", false);
  // show shadowman
  showShadowmanIcon(3, 5);
  showRedHatIcon(249, 4);
}

void showHello()
{
  display.setRotation(3); //even = portrait, odd = landscape
  display.fillScreen(GxEPD_WHITE);

  const GFXfont* f = &FreeSansBoldOblique12pt7b;
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.setCursor(72, 32);
  display.println(name);

  f = &FreeMonoBold9pt7b;
  display.setFont(f);
  display.setCursor(3, 60);
  display.println(title);
  display.setCursor(3, 80);
  display.println(group);
  display.setCursor(3, 100);
  display.println(loc);
  display.setCursor(3, 120);
  display.println(rhcn);

  graphics();

  // update/draw
  display.update();
}
