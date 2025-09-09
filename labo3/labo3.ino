#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>


#define I2C_ADDR      0x3C
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// LM35
const int PIN_LM35 = A0;
const float VREF = 5.0;         
const int NUM_SAMPLES = 10;      
const unsigned long INTERVALO_MS = 800;

unsigned long t0 = 0;

float leerTempC() {
  long suma = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    suma += analogRead(PIN_LM35);  // 0..1023
    delay(3);
  }
  float lectura = suma / (float)NUM_SAMPLES;
  float voltaje = lectura * (VREF / 1023.0); // V
  return voltaje * 100.0;                    // LM35: 10mV/°C
}

void setup() {
  Serial.begin(9600);
  Wire.begin();                

  delay(200);                  // tiempo de power-up de la OLED
  display.begin(I2C_ADDR, true);
  display.clearDisplay();
  display.setContrast(255);    
  display.display();

  // Pantalla de bienvenida
  display.setTextColor(SH110X_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("LM35 + OLED I2C"));
  display.println(F("Inicializando..."));
  display.display();
  delay(1000);
}

void loop() {
  unsigned long now = millis();
  if (now - t0 >= INTERVALO_MS) {
    t0 = now;

    // --- Leer temperatura ---
    float tempC = leerTempC();

    // --- Serial (para ver/loggear) ---
    Serial.print(F("Temperatura: "));
    Serial.print(tempC, 2);
    Serial.println(F(" °C"));

    // --- Dibujar en OLED ---
    display.clearDisplay();

    // Titulo
    display.setTextSize(1);                       
    display.setCursor(0, 0);
    display.println(F("Temperatura (C)"));

    // Valor grande
    display.setTextSize(3);                       
    display.setCursor(5, 22);
    display.print(tempC, 1);                      
    display.setTextSize(2);
    display.print(F(" C"));

    // Pie de pagina opcional
    display.setTextSize(1);
    display.setCursor(0, 54);
    display.print(F("LM35 10mV/°C  Vref="));
    display.print(VREF, 2);
    display.print(F("V"));

    display.display();
  }
}
