void Relogio() {
  //atualiza data*******************************************************
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return;
  }
  //display.clearDisplay();
  //display.setCursor(0, 0);
  Serial.println("Time variables");
  char converteano[5];
  strftime(converteano, 5, "%Y", &timeinfo);
  ano = atol(converteano);
  char converte[3];
  strftime(converte, 3, "%d", &timeinfo);
  dia = atol(converte);
  strftime(converte, 3, "%m", &timeinfo);
  mes = atol(converte);
  strftime(converte, 3, "%H", &timeinfo);
  hora = atol(converte);
  strftime(converte, 3, "%M", &timeinfo);
  minuto = atol(converte);
  strftime(converte, 3, "%S", &timeinfo);
  segundo = atol(converte);
  // CALCULATE LST AND UPDATE ARDUINO RTC
  // Uses algorithms from: Duffett-Smith, P. Zwart, J. (2011),
  // "Practical astronomy with your calculator or spreadsheet",
  // 4th ed, Cambridge University Press.
  float UT = hora + (minuto + segundo / 60.0) / 60.0;      // UTC in decimal
  //Serial.print("UT=");
  //Serial.println(UT, 6);
  double JD0 = 367 * ano - int(7 * (ano + int((mes + 9) / 12)) / 4) + int(275 * mes / 9) + dia + 1721013.5; // Julian date at (UTC 0)
  //double JD = JD0 + UT/24.0;               // Julian date (uncomment if required)
  //Serial.print("JD0=");
  //Serial.println(JD0, 6);
  double s = JD0 - 2451545; // note 0.5 difference between methods
  //Serial.print("s=");
  //Serial.println(s, 6);
  double t = s / 36525.0;
  //Serial.print("t=");
  //Serial.println(t, 6);
  double t0 = 6.697374558 + (2400.051336 * t) + (0.000025862 * t * t);
  t0 = t0 - (24 * int(t0 / 24));
  //Serial.print("t0=");
  //Serial.println(t0, 6);
  double a = UT * 1.002737909;
  //Serial.print("a=");
  //Serial.println(a, 6);
  double lst = t0 + a + Longitude / 15;
  //Serial.print("lst=");
  //Serial.println(lst, 6);
  lst = lst - (24 * int(lst / 24));
  //Serial.print("lst=");
  //Serial.println(lst, 6);
  long totalSec = lst * 3600;
  //Serial.print("totalSec");
  //Serial.println(totalSec);
  TSL = totalSec;
  lstSec = totalSec % 60;
  lstMin = (totalSec / 60) % 60;
  lstHour = totalSec / 3600;
}
