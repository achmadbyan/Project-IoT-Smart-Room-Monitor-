// Proyek IoT: Smart Room Monitor (ESP32)
// Input : DHT22 (suhu & kelembapan), LDR (cahaya)
// Output: LCD I2C 16x2, Buzzer, LED Merah (panas), LED Biru (dingin), LED Kuning (lampu)

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

#define DHTPIN       15
#define DHTTYPE      DHT22
#define LDR_PIN      34
#define BUZZER_PIN   18
#define LED_PANAS    2
#define LED_LAMPU    4
#define LED_DINGIN   19

// Batas logika otomatisasi (sesuaikan dengan hasil Serial Monitor)
const float BATAS_PANAS  = 30.0;  // di atas ini => PANAS
const float BATAS_DINGIN = 24.0;  // di bawah ini => DINGIN
const int   BATAS_GELAP  = 2000;  // nilai LDR di bawah ini => GELAP

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PANAS, OUTPUT);
  pinMode(LED_LAMPU, OUTPUT);
  pinMode(LED_DINGIN, OUTPUT);
  dht.begin();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Smart Room");
  lcd.setCursor(0, 1);
  lcd.print("Monitor ESP32");
  delay(1500);
  lcd.clear();
}

void loop() {
  float suhu = dht.readTemperature();
  float hum  = dht.readHumidity();
  int   ldr  = analogRead(LDR_PIN);

  if (isnan(suhu) || isnan(hum)) {
    lcd.clear();
    lcd.print("Sensor DHT error");
    delay(1000);
    return;
  }

  // Status suhu
  bool panas  = suhu > BATAS_PANAS;
  bool dingin = suhu < BATAS_DINGIN;
  const char* statusSuhu = panas ? "PANAS " : (dingin ? "Dingin" : "Normal");

  digitalWrite(LED_PANAS, panas);
  digitalWrite(LED_DINGIN, dingin);
  if (panas) tone(BUZZER_PIN, 1000);
  else       noTone(BUZZER_PIN);

  // Status cahaya
  bool gelap = ldr < BATAS_GELAP;
  digitalWrite(LED_LAMPU, gelap);

  // Tampilan LCD
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(suhu, 1);
  lcd.print("C H:");
  lcd.print(hum, 0);
  lcd.print("%  ");
  lcd.setCursor(0, 1);
  lcd.print(gelap ? "Gelap " : "Terang");
  lcd.print(" ");
  lcd.print(statusSuhu);
  lcd.print("   ");

  // Serial Monitor (untuk kalibrasi)
  Serial.printf("Suhu=%.1f Hum=%.0f LDR=%d | %s | %s\n",
                suhu, hum, ldr, gelap ? "GELAP" : "TERANG", statusSuhu);

  delay(500);
}
