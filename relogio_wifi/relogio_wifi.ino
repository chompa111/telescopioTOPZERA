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

#define dc 0.0657098244
#define tc 1.00273791
#define gc 6.648605
#define g2000 6.5988098
#define lc 0.0497958000000001
#define nc -0.0159140999999998
#define fudge -0.013922               // fudge factor (unnecessary?
#define LONGITUDE -2.247926           // Longitude for Manchester UK            
#define LATITUDE  53.53               // Latitude for Manchester UK
#define siderealday 23.9344699        // length of sidereal day (23:56:04)
double GST,LST,utc;                   // greenwich & local apparent sidereal time
int dh,dm,ds;                         // local sidereal time
int sh,sm,ss;                         // solar time

// equation of time
double tv,tv1,tv2,tv3;                // total time variation (mins)
int tvm,tvs,nd;               

#define sp Serial.print   
#define spln Serial.println

const char* ssid     = "FREUD";//rede wifi
const char* password = "pensa428";//senha wifi

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -10800;
const int   daylightOffset_sec = 0;
long hora,minuto,segundo,dia,mes,ano;


void setup() {
  delay(1000);
  Serial.begin(115200);
  delay(100);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  // init done
  display.clearDisplay();
  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Conectando ao WiFi...");
  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  display.print("Rede = ");
  Serial.println(ssid);
  display.print(ssid);
  WiFi.begin(ssid, password);
  display.display();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    display.print(">");
    display.display();
  }
  Serial.println("");
  display.println("");
  Serial.println("WiFi connected.");
  display.println("CONECTADO");
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void loop() {
  delay(1000);
  printLocalTime();
}

void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  display.clearDisplay();
  display.setCursor(0,0);
  Serial.println("Time variables");
  char converteano[5];
  strftime(converteano, 5, "%Y", &timeinfo);
  ano=atol(converteano);
  char converte[3];
  strftime(converte, 3, "%d", &timeinfo);
  dia=atol(converte);
  strftime(converte, 3, "%m", &timeinfo);
  mes=atol(converte);
  strftime(converte, 3, "%H", &timeinfo);
  hora=atol(converte);
  strftime(converte, 3, "%M", &timeinfo);
  minuto=atol(converte);
  strftime(converte, 3, "%S", &timeinfo);
  segundo=atol(converte);
  display.print(dia);
  display.print("/");
  display.print(mes);
  display.print("/");
  display.print(ano);
  display.print(" ");
  display.print(hora);
  display.print(":");
  display.print(minuto);
  display.print(":");
  display.println(segundo);
  display.display();
  
}
