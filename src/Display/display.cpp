#include <Display/display.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET     -1 
#define SCREEN_ADDRESS 0x3C

static Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const unsigned long update_interval = 1000;

void init_dispaly()
{
    display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
}

void update_display(float *MFC_set, float *MFC_is)
{
    static unsigned long last_update = millis();
    if(millis() - last_update < update_interval) return;

    display.clearDisplay();
    char string_buf[6];
    for(byte i=0; i<4; i++)
    {
        display.setTextSize(2);
        display.setCursor((i%2)*64, (i/2)*32);
        dtostrf(MFC_is[i], 5, 1, string_buf);
        display.print(string_buf);
        display.setTextSize(1);
        display.setCursor((i%2)*64 + 30, (i/2)*32 + 18);
        dtostrf(MFC_set[i], 5, 1, string_buf);
        display.println(string_buf);
    }
    display.display();
    last_update = millis();
}