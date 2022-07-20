#include <Wire.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define pin_reset 0
#define pin_ss 2
#define buzzer 15
#define relay 16

LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 mfrc522(pin_ss, pin_reset);
//inisialisasi konfigurasi wifi
const char *ssid =  "Kede Ndon";     //Nama SSID Wifi
const char *pass =  "nasution123";             //Password Wifi
WiFiClient client;
String link = "https://yasir.justhasnah.me/api/device/status";
String link_cek_id_card = "https://yasir.justhasnah.me/api/access/id-card/cek";

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  pinMode(buzzer, OUTPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);
  //  Serial.println("Put your card to the reader...");
  lcd.begin();
  lcd.backlight();
  printLcd("Sistem keamanan", "laboratorium");
  delay(3000);
  printLcd("M. Yasir", "Habibi Bako");
  delay(3000);
  printLcd("Inisialisasi", "Koneksi wifi ...");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  printLcd("Koneksi", "berhasil");
  delay(2000);
  printLcd("Cek status", "perangkat");
  delay(2000);
  printLcd("Selesai", "Ready ...");
  delay(2000);
}

void loop() {
  printLcd("Singkronisasi", "perangkat");
  delay(1000);
  cekCommand();
  delay(2000);
}

void cekCommand()
{
  HTTPClient http;
  String postData;
  postData = "status=on";
  http.begin(client, link.c_str());
  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    String payload = http.getString();
    printLcd("Status device : ", payload);
    delay(2000);
    if (String(payload) == "ADD_ACCESS") {
      //      Ke program utama
      printLcd("Penambahan", "Hak akses ..");
      delay(2000);
      printLcd("Mohon persiapkan", "ID CARD");
      delay(2500);
      printLcd("Dekatkan", "ID CARD");
      delay(2000);
    } else if (String(payload) == "READ_CARD") {
      printLcd("Pembacacaan", "ID CARD");
      delay(2000);
      printLcd("Dekatkan", "ID CARD");
      delay(2000);
      printLcd("Membaca", "ID CARD");
      delay(1000);
      bacaIdCard();
      delay(1000);
    } else {
      printLcd("Dekatkan", "ID CARD");
      cekIdCard();
    }
  } else {
    printLcd("Error koneksi", "ke server");
  }
  http.end();  //Close connection
}

void bacaIdCard()
{
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  printLcd("Id card", "Terdeteksi ...");
  delay(2000);

  String content = "";
  byte letter;

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  content.toUpperCase();
  printLcd("ID CARD : ", content.substring(1));
  delay(6000);
}

void cekIdCard()
{
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  printLcd("Id card", "Terdeteksi ...");
  delay(2000);

  String content = "";
  byte letter;

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  content.toUpperCase();
  printLcd("Checking data ", "on server ...");
  delay(2000);
  HTTPClient http;
  String postData;
  postData = "idcard=" + content.substring(1);
  http.begin(client, link_cek_id_card.c_str());
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpResponseCode = http.POST(postData);
  if (httpResponseCode > 0) {
    String payload = http.getString();
    if (payload == "NO_ID") {
      printLcd("ID card tidak", "teregistrasi");
      delay(2000);
      buzzCepat();
      printLcd("Harap coba ", "kembali");
      delay(2000);
    } else {
      printLcd("Data", "ditemukan..");
      delay(2000);
      printLcd("Nama : ", payload);
      delay(2000);
      printLcd("Autentifikasi", "selanjutnya");
      delay(1500);
      printLcd("Tempelkan", "jari anda");
      delay(3000);
      printLcd("Sidik jari", "cocok");
      delay(2000);
      printLcd("Akses diterima", "silahkan masuk ..");
      buzzKencang();
      digitalWrite(relay, LOW);
      delay(2000);
      printLcd("Bersiap untuk", "mengunci pintu ..");
      delay(10000);
      digitalWrite(relay, HIGH);
    }
  } else {
    printLcd("Error koneksi", "ke server");
  }
  //  printLcd("ID Card data :", content.substring(1));
  delay(2000);
}


void buzzCepat()
{
  digitalWrite(buzzer, HIGH);
  delay(200);
  digitalWrite(buzzer, LOW);
  delay(200);
  digitalWrite(buzzer, HIGH);
  delay(200);
  digitalWrite(buzzer, LOW);
  delay(200);
}

void buzzKencang()
{
  digitalWrite(buzzer, HIGH);
  delay(500);
  digitalWrite(buzzer, LOW);
  delay(500);
  digitalWrite(buzzer, HIGH);
  delay(500);
  digitalWrite(buzzer, LOW);
  delay(500);
}

void printLcd(String teks, String teks2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(teks);
  lcd.setCursor(0, 1);
  lcd.print(teks2);
}
