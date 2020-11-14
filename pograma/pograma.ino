//class Motor { //teste de teste
    int pointer = 0;
    int* pins ;

    String sequence[8] = {"1100", "0100", "0110", "0010", "0011", "0001", "1001", "1000"};

    void pinFunc(int vetor[], int range) {
      for (int i = 0; i < range; i++) {
        pinMode(vetor[i], OUTPUT);
      }
    }


  public : Motor(int pins[]) {
      pinFunc(pins, 4);
      this->pins = pins;
    }

    void setState(String input) {
      Serial.println("setState");
      if (input.charAt(0) == '1') {
        digitalWrite(pins[0], 1);
      } else {
        digitalWrite(pins[0], 0);
      }

      if (input.charAt(1) == '1') {
        digitalWrite(pins[1], 1);
      } else {
        digitalWrite(pins[1], 0);
      }

      if (input.charAt(2) == '1') {
        digitalWrite(pins[2], 1);
      } else {
        digitalWrite(pins[2], 0);
      }

      if (input.charAt(3) == '1') {
        digitalWrite(pins[3], 1);
      } else {
        digitalWrite(pins[3], 0);
      }
    }

  public: void forward() {
      setState(sequence[pointer]);
      pointer = pointer + 1;
      if (pointer == 8)pointer = 0;
    }

  public: void backward() {
      setState(sequence[pointer]);
      pointer = pointer - 1;
      if (pointer == -1)pointer = 7;
    }
};


#include <WiFi.h>
#include "time.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 4

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);




int pinosMotor1[] = {13, 12, 14, 27};
int pinosMotor2[] = {26, 25, 33, 32};


#define VEL_MIN 200

#define VEL_MAX 2

Motor motor1(pinosMotor1);
Motor motor2(pinosMotor2);

void setup() {

  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
}

int tempoH = 0;
int tempoV = 0;
int tempoD = 0;


double angH = 0;
double angV = 0;

double ratio = 360 / (4075.7728395 * 10);


void loop() {

  int leituraV = analogRead(A6) - 1967;
  int leituraH = analogRead(A7) - 1900;

  long currentMilis = millis();
  int intervalV = map(abs(leituraV), 0, 1900, VEL_MIN, VEL_MAX);
  intervalV = intervalV < 0 ? 2 : intervalV;

  int intervalH = map(abs(leituraH), 0, 1900, VEL_MIN, VEL_MAX);
  intervalH = intervalH < 0 ? 2 : intervalH;


  if (abs(leituraV) > 100) {
    if ((currentMilis - tempoV) > intervalV) {
      if (leituraV < 0) {
        //backward(sequence, &pointer, pinosMotor1);
        motor1.backward();
        angV += ratio;
      } else {
        motor1.forward();
        angV -= ratio;
      }
      tempoV = currentMilis;
    }
  }

  if (abs(leituraH) > 100) {
    if ((currentMilis - tempoH) > intervalH) {
      if (leituraH < 0) {
        motor2.forward();
        angH += ratio;
      } else {
        motor2.backward();
        angH -= ratio;
      }
      tempoH = currentMilis;
    }
  }

  if ((currentMilis - tempoD) > 100) {
    tempoD = currentMilis;
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("azimute:");
    display.println(angH);
    display.print("elevacao:");
    display.println(angV);
    display.display();
  }

}
