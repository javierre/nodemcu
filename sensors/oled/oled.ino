/* Hello World OLED Test */

#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
SSD1306  display(0x3c, D1, D2); // Initialise the OLED display using Wire library
void setup() 
{
  Serial.begin(115200);
  display.init(); // Initialising the UI will init the display too.
  display.flipScreenVertically();
  
  display.clear();
  drawHelloWorld(); 
  display.display();
}
void loop() 
{
  
}
void drawHelloWorld() 
{
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "Hello world");
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 10, "Hello world");
    display.setFont(ArialMT_Plain_24);
    display.drawString(0, 26, "Hello world");
}
