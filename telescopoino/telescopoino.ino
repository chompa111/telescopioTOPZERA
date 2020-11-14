
class Motor {
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


/*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                 Created 6 April 2014                                    *
*                                       by EDC                                            *
*                           http://epsilonphoto.weebly.com/                               *
*                                                                                         *
*      This program is free software, you can redistribute it and/or modify.              *
*      Is in the public domain, it only has a didactic purpose and you can modify it      *
*      to your liking. I do not provide any guarantee for any damage it may cause         *
*      to property or persons.                                                            *
*                                                                                         *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*/


#include <stdlib.h>
#include <math.h>
#include <WiFi.h>
#include "time.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 4

int pinosMotorH[] = {13, 12, 14, 27};
int pinosMotorV[] = {26, 25, 33, 32};


#define VEL_MIN 200

#define VEL_MAX 2

Motor motorH(pinosMotorH);
Motor motorV(pinosMotorV);

const int buttonAlign = 19;         // pin in cui l'interruttore abilita l'allineamento con una stella
int buttonState = 0;                // variabile in cui arduino andrà a leggere lo stato logico di buttonAlign
char input[20];                     // array di char in cui verranno memorizzati i dati in ingresso
char txAR[10];                      // array di char in cui verranno memorizzati i dati in uscita
char txDEC[11];                     // array di char in cui verranno memorizzati i dati in uscita
long ARtel, DECtel;                 // coordinate del telescopio espresse in secondi
long ARtarget, DECtarget;           // coordinate del target espresse in secondi
long A_diff, h_diff;                // differenza tra le coordinate del target e del telescopio
long arHH, arMM, arSS;              // coordinata AR del telescopio da trasmattere a stellarium
char SIGNtel;                       // segno della DEC: 43 = +   45 = -
long decDEG, decMM, decSS;          // coordinata DEC del telescopio da trasmettere a stellarium
boolean enablegoto;
long latitude, TSL, A_teltx, A_tel, h_tel, H_tel, H_target, h_target, A_target;
const float pigreco = 3.141593;
float phi, delta_tel, H_telRAD, h_telRAD, A_telRAD, cos_phi, sin_phi;
float delta_target, H_targetRAD, h_targetRAD, A_targetRAD;
float numerator1, denominator1, numerator2, denominator2;


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void setup() {

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  pinMode(buttonAlign, INPUT);
  pinMode(5, OUTPUT);
  Serial.begin(9600);
  A_target = A_tel = 0;            // coordinata iniziale di Azimut A (in secondi d'arco) per visualizzare il reticolo del telescopio
  h_target = h_tel = 0;            // coordinata iniziale di altezza h (in secondi d'arco) per visualizzare il reticolo del telescopio
  latitude = -83632;                    // latitudine del luogo di osservazione espressa in secondi d'arco
  phi = (float(latitude)/3600.0)*pigreco/180.0;    // latitudine in radianti
  cos_phi = cos(phi);
  sin_phi = sin(phi);
  enablegoto = false;
  TSL = 0;                              // in questo Sketch non c'è l'orologio siderale, pertanto TSL è fissato a zero e deve essere espresso in secondi
}


void loop() {

  if(Serial.available()>0){
    communication();
  }

  A_diff = A_target - A_tel;
  h_diff = h_target - h_tel;
  if(enablegoto == true){
   goto_object();
  }
  display.display();
  //display.setCursor(0,0);

}


//--------------------------------------------------------------------------------------------------------------------------------------------------------

void communication(){

  int i=0;
  input[i++] = Serial.read();
  delay(5);
  while((input[i++] = Serial.read()) != '#'){
    delay(5);
  }
  input[i]='\0';


  if(input[1]==':' && input[2]=='G' && input[3]=='R' && input[4]=='#'){             // con il comando #:GR# stellarium chiede l'invio della coordinata di AR
    transmitAR();
  }


  if(input[1]==':' && input[2]=='G' && input[3]=='D' && input[4]=='#'){             // con il comando #:GD# stellarium chiede l'invio della coordinata di AR
    transmitDEC();
  }


  if(input[1]==':' && input[2]=='Q' && input[3]=='#'){                              // con il comando #:Q# stellarium chiede l'arresto dei motori
  // inviare l'arresto ai motori
  }


  if(input[0]==':' && input[1]=='S' && input[2]=='r'){                              // con il comando :Sr stellarium invia la coordinata di AR
    getAR();
  }


  if(input[0]==':' && input[1]=='S' && input[2]=='d'){                              // con il comando :Sd stellarium invia la coordinata di DEC
    getDEC();
  }


  if(input[0]==':' && input[1]=='M' && input[2]=='S' && input[3]=='#'){             // con il comando :MS# stellarium chiede se la rotazione è possibile
    Serial.print("0");
    buttonState = digitalRead(buttonAlign);                                         // legge se l'interruttore per l'allineamento iniziale è su ON o OFF
    if (buttonState == HIGH) {
      digitalWrite(5,HIGH);                                                   // se HIGH aggiorna le coordinate, altrimenti proseguià con il GOTO
      ARtel = ARtarget;
      DECtel = DECtarget;
    }
    else{
      digitalWrite(5,LOW);
    }
    enablegoto = true;                                                              // abilita il goto
  }
}


//--------------------------------------------------------------------------------------------------------------------------------------------------------

void transmitAR(){                                                              // trasmissione dati nella forma   HH:MM:SS#

     convert_AZ_EQ();
     arHH = ARtel/3600;                                                         // ricava le ore della coordinata AR del telescopio
     arMM = (ARtel-arHH*3600)/60;                                               // ricava i minuti della coordinata AR del telescopio
     arSS = (ARtel-arHH*3600)-arMM*60;                                          // ricava i secondi della coordinata AR del telescopio
     sprintf(txAR, "%02d:%02d:%02d#", int(arHH), int(arMM), int(arSS));
     Serial.print(txAR);

    }

//--------------------------------------------------------------------------------------------------------------------------------------------------------

void transmitDEC(){                                                             // trasmissione dati nella forma  sDDßMM:SS#  (dove s è il segno + o -)

     convert_AZ_EQ();
     (DECtel < 0) ? SIGNtel = 45: SIGNtel = 43;                                 // controllo del segno della coordinata DEC del telescopio
     decDEG = abs(DECtel)/3600;                                                 // ricava i gradi della coordinata DEC del telescopio
     decMM = (abs(DECtel) - decDEG*3600)/60;                                    // ricava i minuti della coordinata DEC del telescopio
     decSS = (abs(DECtel) - decDEG*3600) - decMM*60;                            // ricava i secondi della coordinata DEC del telescopio
     sprintf(txDEC, "%c%02d%c%02d:%02d#", SIGNtel, int(decDEG), 223, int(decMM), int(decSS));
     Serial.print(txDEC);

    }

//--------------------------------------------------------------------------------------------------------------------------------------------------------

void getAR(){                                                                   // riceve la coordinata AR del target nel formato :Sr HH:MM:SS#
     // lcd.setCursor(0,0);
     // lcd.print(input);
     //display.setCursor(0,0);
     display.println(input);

     Serial.print("1");
     ARtarget = (atol(input+3))*3600 + (atol(input+6))*60 + atol(input+9);     // converte in secondi la coordinata AR del target
     enablegoto = false;                                                        // mantiene disabilitato il goto fino all'arrivo della coordinata DEC
    }


//--------------------------------------------------------------------------------------------------------------------------------------------------------

void getDEC(){                                                                  // riceve la coordinata AR del target nel formato :Sd +DDßMM:SS#
    // lcd.setCursor(0,1);
    // lcd.print(input);
    //display.setCursor(0,1);
    display.println(input);

    Serial.print("1");
    DECtarget = (atol(input+4))*3600 + (atol(input+7))*60 + atol(input+10);     // converte in secondi la coordinata DEC del target
    if (input[3] == '-'){                                                       // per una corretta conversione ha prelevato solo la parte numerale
      DECtarget *=(-1);                                                         // (cioè senza il segno) pertanto se la coordinata DEC è negativa, dopo
     }                                                                          // la conversione in secondi viene moltiplica per -1
     enablegoto = false;
     convert_EQ_AZ();
   }


//--------------------------------------------------------------------------------------------------------------------------------------------------------

void convert_EQ_AZ(){

  H_target = TSL - ARtarget;
  if (H_target < 0){
    H_target = H_target + 86400;                                                                 // l'angolo orario è sempre positivo
  }
  H_targetRAD = (float(H_target)*15.0/3600.0)*pigreco/180.0;                                     // angolo orario target in radianti
  delta_target = (float(DECtarget)/3600.0)*pigreco/180.0;                                        // declinazione target espressa in radianti

  h_targetRAD = asin(sin_phi*sin(delta_target) + cos_phi*cos(delta_target)*cos(H_targetRAD));
  h_targetRAD = (h_targetRAD*180.0/pigreco)*3600.0;                                              // altezza h espressa in secondi d'arco
  h_target = long(h_targetRAD);

  numerator1 = sin(H_targetRAD);
  denominator1 = (cos(H_targetRAD)*sin_phi - tan(delta_target)*cos_phi);
  A_targetRAD = atan(numerator1/denominator1);
  A_targetRAD = (A_targetRAD*3600.0)*180.0/pigreco;                                              // A_targetRAD espresso in secondi d'arco
  A_target = long(A_targetRAD);

  if ((numerator1 >= 0) && (denominator1 >=0)){                                                  // aggiungo l'offset. Ho incluso anche la correzione per avere
    A_target = A_target + 648000;                                                                // l'origine dell'azimut a Nord aggiungendo altri 180° (espressi
  }                                                                                              // in secondi d'arco)
  if(denominator1 < 0){
    A_target = A_target + 1296000;
  }
  if ((numerator1 < 0) && (denominator1 >=0)){
    A_target = A_target + 1944000;
  }

  if (A_target >= 1296000){                                                                      // se l'azimut supera i 360° sottraggo 360°
    A_target = A_target-1296000;
  }

}


//--------------------------------------------------------------------------------------------------------------------------------------------------------

void convert_AZ_EQ(){

  A_teltx = A_tel;
  A_teltx = A_teltx - 648000;                                                                   // ripristino l'origine dell'azimut a Sud togliendo 180°
  if(A_teltx < 0){                                                                              // e se risultasse negativo aggiungo 360°
    A_teltx = A_teltx + 1296000;
  }

  A_telRAD = (float(A_teltx)/3600.0)*pigreco/180.0;                                             // Azimut A del telescopio espresso in radianti
  h_telRAD = (float(h_tel)/3600.0)*pigreco/180.0;                                               // altezza h del telescopio espressa in radianti

  delta_tel = asin(sin_phi*sin(h_telRAD) - cos_phi*cos(h_telRAD)*cos(A_telRAD));                // declinazione del telescopio in radianti
  delta_tel = (delta_tel*180.0/pigreco)*3600.0;                                                 // declinazione del telescopio in secondi d'arco
  DECtel = long(delta_tel);

  numerator2 = sin(A_telRAD);
  denominator2 = (cos(A_telRAD)*sin_phi + tan(h_telRAD)*cos_phi);
  H_telRAD = atan(numerator2/denominator2);
  H_telRAD = (H_telRAD*180.0/(pigreco))*(3600.0/15.0);                                          // H_telRAD espresso in secondi
  H_tel = long(H_telRAD);

 if(denominator2 < 0){                                                                          // aggiungo l'offset
    H_tel = H_tel + 43200;
  }
  if ((numerator2 < 0) && (denominator2 >=0)){
    H_tel = H_tel + 86400;
  }

  if (H_tel >= 86400){
    H_tel = H_tel - 86400;
  }

  ARtel = TSL - H_tel;
  if (ARtel < 0){                                                                               // l'Ascensione Retta è sempre positiva
    ARtel = ARtel + 86400;
  }

}


//--------------------------------------------------------------------------------------------------------------------------------------------------------

void goto_object(){

  if ((A_diff >0 && A_diff <= 648000) || (A_diff <=(-648000))){         //confronta le coordinate di AR e sceglie la via più breve per raggiungere il target
    increment_A_tel();
  }

  if ((A_diff > 648000) ||  (A_diff <0 && A_diff > (-648000))){
    decrement_A_tel();
  }



  if (h_target > h_tel){                                                //confronta le coordinate di DEC e sceglie la via più breve per raggiungere il target
    go_up();
  }

  if (h_target < h_tel){
    go_down();
  }

}


//--------------------------------------------------------------------------------------------------------------------------------------------------------

void increment_A_tel(){
  A_tel+=32;
  // control.SetDirection(StepperControl::Forward);
  // control.Step(1);
  motorV.backward();
  delay(2);
  // in questo Sketch l'incremento è semplicemente aritmetico, in realtà deve provenire
  if(A_tel >= 1296000){                   // da una lettura dei dati dell'encoder, oppure per le montature sprovviste, dal
    A_tel = A_tel - 1296000;              // numero dei passi del motore (meno preciso perché non tiene conto del backlash
  }                                       // dell'eventuale motoriduttore o degli step persi.
  // inviare comando al motore AR

}

//--------------------------------------------------------------------------------------------------------------------------------------------------------

void decrement_A_tel(){
  A_tel-=32;
  // control.SetDirection(StepperControl::Backward);
  // control.Step(1);  // in questo Sketch il decremento è semplicemente aritmetico, in realtà deve provenire
  motorV.forward();
  delay(2);
  if(A_tel < 0){                          // da una lettura dei dati dell'encoder, oppure per le montature sprovviste, dal
    A_tel = A_tel + 1296000;              // numero dei passi del motore (meno preciso perché non tiene conto del backlash
  }                                       // dell'eventuale motoriduttore o degli step persi.
  // inviare comando al motore AR

}

//--------------------------------------------------------------------------------------------------------------------------------------------------------

void go_up(){                             // in questo Sketch l'incremento è semplicemente aritmetico, in realtà deve provenire
  h_tel+=32;                                // da una lettura dei dati dell'encoder, oppure per le montature sprovviste, dal
  // inviare comando al motore DEC        // numero dei passi del motore (meno preciso perché non tiene conto del backlash
                                          // dell'eventuale motoriduttore o degli step persi.

    motorH.forward();
    delay(2);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------

void go_down(){                           // in questo Sketch il decremento è semplicemente aritmetico, in realtà deve provenire
  h_tel-=32;                                // da una lettura dei dati dell'encoder, oppure per le montature sprovviste, dal
  // inviare comando al motore DEC        // numero dei passi del motore (meno preciso perché non tiene conto del backlash
                                          // dell'eventuale motoriduttore o degli step persi.
      motorV.backward();
      delay(2);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
