#include <Adafruit_NeoPixel.h>

#define PIN       13  // Pin donde tenemos conectado la tira NeoPixel
#define NUMPIXELS 4 // Número de píxeles o LEDs

// Declaramos el objeto. El último parámetro dependerá de tu tipo de NeoPixel
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_RGB + NEO_KHZ800);


// Podemos definir las funciones con los colores que queramos
// https://www.w3schools.com/colors/colors_picker.asp
// el brillo va con pixels.setBrightness(64);
void naranja (){
  pixels.setPixelColor(0, pixels.Color(255, 153, 0));
  pixels.setPixelColor(1, pixels.Color(255, 153, 0));
  pixels.setPixelColor(2, pixels.Color(255, 153, 0));
  pixels.setPixelColor(3, pixels.Color(255, 153, 0));
  pixels.show();
}

void verde (){
  pixels.setPixelColor(0, pixels.Color(0, 153, 0));
  pixels.setPixelColor(1, pixels.Color(0, 153, 0));
  pixels.setPixelColor(2, pixels.Color(0, 153, 0));
  pixels.setPixelColor(3, pixels.Color(0, 153, 0));
  pixels.show();
}

void setup() {
  // Iniciamos neopixel
  pixels.begin();
  pixels.show();
  pinMode(12, OUTPUT);
  digitalWrite(12, HIGH);
}

void loop() {
  naranja();
  pixels.setBrightness(64);
  delay(1000);
  pixels.setBrightness(200);
  delay(1000);

}

