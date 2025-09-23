// === OLED: Carita triste + "nos vemos hermanita pequeña" (ñ dibujada) + lágrima animada ===
// Arduino UNO + Adafruit_GFX + SH1106/SSD1306 (128x64)

#include <Wire.h>
#include <Adafruit_GFX.h>
// Usa UNA de las dos según tu módulo:
#include <Adafruit_SH110X.h>      // SH1106 / SH1106G
// #include <Adafruit_SSD1306.h>  // SSD1306 (descomenta si tu módulo es SSD1306)

#define I2C_ADDR      0x3C
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

// Display para SH1106 (por defecto):
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// Si usas SSD1306, comenta la línea de arriba y descomenta esta:
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Aliases de color por si cambias a SSD1306
#ifndef SH110X_WHITE
  #define SH110X_WHITE WHITE
  #define SH110X_BLACK BLACK
#endif

// Centro de la carita
const int CX = SCREEN_WIDTH / 2;
const int CY = 38;
const int R  = 18;

// Lágrima
int tearY = CY - 4;        // arranca a la altura del ojo
const int tearX = CX + 6;  // ojo derecho
const int tearYMin = CY - 4;
const int tearYMax = CY + R - 1;

// ---- Helpers ----
void drawCenteredText(int y, const char* msg, uint8_t size) {
  int len = 0;
  while (msg[len] != '\0') len++;
  int charW = 6 * size;                 // ancho de la fuente básica
  int textW = len * charW;
  int x = (SCREEN_WIDTH - textW) / 2;
  if (x < 0) x = 0;
  display.setTextSize(size);
  display.setCursor(x, y);
  display.print(msg);
}

// Dibuja una tilde (~) sobre la ÚLTIMA 'n' (o 'N') del mensaje, para simular "ñ"
void drawEnieTildeOverLastN(int xStart, int yTop, const char* msg, uint8_t size) {
  int len = 0, idx = -1;
  while (msg[len] != '\0') {
    if (msg[len] == 'n' || msg[len] == 'N') idx = len;
    len++;
  }
  if (idx < 0) return;

  int charW = 6 * size;
  // Ajuste fino de la posición de la tilde sobre la caja del carácter
  int nx = xStart + idx * charW + 2 * size; // desplaza ligeramente a la derecha
  int ny = yTop   - 2 * size;               // un poco arriba del carácter

  // Trazamos una pequeña "~" con tres segmentos; engrosa con 'size'
  for (int i = 0; i < size; i++) {
    display.drawLine(nx - 2*size, ny + i,     nx - 1*size, ny - 1*size + i, SH110X_WHITE);
    display.drawLine(nx - 1*size, ny - 1*size + i, nx + 0*size, ny + 0*size + i, SH110X_WHITE);
    display.drawLine(nx + 0*size, ny + 0*size + i, nx + 1*size, ny - 1*size + i, SH110X_WHITE);
  }
}

void drawSadFace() {
  // Cabeza
  display.drawCircle(CX, CY, R, SH110X_WHITE);

  // Ojos
  display.fillCircle(CX - 6, CY - 4, 1, SH110X_WHITE);
  display.fillCircle(CX + 6, CY - 4, 1, SH110X_WHITE);

  // Cejas levemente caídas
  display.drawLine(CX - 10, CY - 9, CX - 2, CY - 7, SH110X_WHITE);
  display.drawLine(CX + 2,  CY - 7, CX + 10, CY - 9, SH110X_WHITE);

  // Boca triste (parábola aproximada con píxeles)
  for (int i = -6; i <= 6; i++) {
    int y = CY + 6 + (i*i)/12;  // curva suave
    display.drawPixel(CX + i, y, SH110X_WHITE);
  }
}

void drawTear() {
  // Gota: círculo + un pixel de “cola”
  display.fillCircle(tearX, tearY, 1, SH110X_WHITE);
  display.drawPixel(tearX, tearY + 1, SH110X_WHITE);
}

void setup() {
  Wire.begin();
  delay(200);
  display.begin(I2C_ADDR, true);
  display.clearDisplay();
  display.setContrast(255);
  display.display();
}

void loop() {
  display.clearDisplay();

  // Línea 1
  drawCenteredText(0, "nos vemos", 2);

  // Línea 2: imprimimos "hermanita pequena" y le dibujamos tilde a la 'n' final => "pequena" -> "pequeña"
  const uint8_t size2 = 1;
  const char* msg2 = "hermanita pequena";
  int len2 = 0; while (msg2[len2] != '\0') len2++;
  int xStart = (SCREEN_WIDTH - (len2 * 6 * size2)) / 2; if (xStart < 0) xStart = 0;

  display.setTextSize(size2);
  display.setCursor(xStart, 16);   // yTop=16
  display.print(msg2);
  drawEnieTildeOverLastN(xStart, 16, msg2, size2);

  // Carita y lágrima
  drawSadFace();
  drawTear();

  display.display();

  // Animación de la lágrima
  tearY += 1;                 // velocidad de caída
  if (tearY > tearYMax) {
    tearY = tearYMin;         // reinicia arriba del ojo
  }

  delay(50);                  // velocidad general
}
