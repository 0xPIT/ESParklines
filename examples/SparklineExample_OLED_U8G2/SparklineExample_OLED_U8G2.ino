#include <Arduino.h>
#include <PolledTimeout.h>

#include <U8g2lib.h>
U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(U8G2_R0, U8X8_PIN_NONE, SCL, SDA);

#include <SparkLine.h>
SparkLine<uint16_t> MySparkLine1(25, [&](const uint16_t x0, const uint16_t y0, const uint16_t x1, const uint16_t y1) { 
  display.drawLine(x0, y0, x1, y1);
});
SparkLine<uint16_t> MySparkLine2(50, [&](const uint16_t x0, const uint16_t y0, const uint16_t x1, const uint16_t y1) { 
  display.drawLine(x0, y0, x1, y1);
});
SparkLine<uint16_t> MySparkLine3(50, [&](const uint16_t x0, const uint16_t y0, const uint16_t x1, const uint16_t y1) { 
  display.drawLine(x0, y0, x1, y1);
});

void setupDisplay() {  
  display.begin();
  display.clear(); 
  display.setDrawColor(1);
  display.setFont(u8g2_font_profont11_mf);
  display.drawStr(0, 8, "Sparklines <3 @0xPIT");
  display.sendBuffer();
}

void setup() {
  randomSeed(analogRead(0));
  setupDisplay();
}

void clearArea(int x, int y, int w, int h) {
    display.setDrawColor(0); 
    display.drawBox(x, y, w, h); 
    display.setDrawColor(1);
}

static esp8266::polledTimeout::periodicMs periodicSecond(1000);

void loop() {
  if (periodicSecond) {
    const uint8_t border = 4;

    clearArea(0, 8, display.getDisplayWidth(), display.getDisplayHeight() - 8);

    MySparkLine1.add(random(0, 100));
    MySparkLine1.draw(border, 25, display.getDisplayWidth() - border, 12);

    uint16_t halfWidth = display.getDisplayWidth() / 2 
                       - 2 * display.getMaxCharWidth() + 2 * 1
                       - 2 * border;

    uint xpos = 0;
    display.drawStr(xpos, 38, "1");
    xpos += display.getMaxCharWidth() + 1;
    MySparkLine1.draw(xpos, 40, halfWidth, 12);

    xpos += display.getDisplayWidth() / 2;

    display.drawStr(xpos, 38, "2");
    xpos += display.getMaxCharWidth() + 1;

    MySparkLine3.add(random(0, 100));
    MySparkLine3.draw(xpos, 40, halfWidth, 12);
    
    MySparkLine2.add(random(0, 50));
    MySparkLine2.draw(border, 64, display.getDisplayWidth() - border, 20);

    display.sendBuffer();
  }
}
