#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD address genelde 0x27 veya 0x3F olur
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin tanımları
const int buttonPin = 2;
const int buzzerPin = 8;
const int potPin    = A0;

// Zaman ayarları
const unsigned long dotThreshold = 300;
const unsigned long letterPause  = 1000;
const unsigned long debounceTime = 30;

String morseInput = "";
String decodedText = "";

bool lastButtonState = HIGH;
bool currentButtonState = HIGH;
bool pressing = false;

unsigned long pressStartTime = 0;
unsigned long lastInputTime = 0;
unsigned long lastDebounceTime = 0;

// Morse tablosu
String morseTable[][2] = {
  {".-", "A"},   {"-...", "B"}, {"-.-.", "C"}, {"-..", "D"},  {".", "E"},
  {"..-.", "F"}, {"--.", "G"},  {"....", "H"}, {"..", "I"},   {".---", "J"},
  {"-.-", "K"},  {".-..", "L"}, {"--", "M"},   {"-.", "N"},   {"---", "O"},
  {".--.", "P"}, {"--.-", "Q"}, {".-.", "R"},  {"...", "S"},  {"-", "T"},
  {"..-", "U"},  {"...-", "V"}, {".--", "W"},  {"-..-", "X"}, {"-.--", "Y"},
  {"--..", "Z"},
  {"-----", "0"}, {".----", "1"}, {"..---", "2"}, {"...--", "3"}, {"....-", "4"},
  {".....", "5"}, {"-....", "6"}, {"--...", "7"}, {"---..", "8"}, {"----.", "9"}
};

String decodeMorse(String code) {
  for (int i = 0; i < 36; i++) {
    if (morseTable[i][0] == code) {
      return morseTable[i][1];
    }
  }
  return "?";
}

void beepSymbol(char symbol) {
  int potValue = analogRead(potPin);
  int freq = map(potValue, 0, 1023, 400, 2000);

  if (symbol == '.') {
    tone(buzzerPin, freq, 150);
    delay(180);
  } else if (symbol == '-') {
    tone(buzzerPin, freq, 450);
    delay(500);
  }
  noTone(buzzerPin);
}

void showStatus() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Morse:");
  lcd.setCursor(7, 0);
  lcd.print(morseInput);

  lcd.setCursor(0, 1);
  lcd.print("Text:");
  lcd.setCursor(5, 1);
  lcd.print(decodedText);
}

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Morse Code");
  lcd.setCursor(0, 1);
  lcd.print("Interpreter");
  delay(2000);

  lcd.clear();
  showStatus();
}

void loop() {
  bool reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceTime) {
    if (reading != currentButtonState) {
      currentButtonState = reading;

      if (currentButtonState == LOW) {
        pressing = true;
        pressStartTime = millis();
      }

      if (currentButtonState == HIGH && pressing) {
        pressing = false;
        unsigned long pressDuration = millis() - pressStartTime;

        if (pressDuration < dotThreshold) {
          morseInput += ".";
          beepSymbol('.');
        } else {
          morseInput += "-";
          beepSymbol('-');
        }

        lastInputTime = millis();
        showStatus();
      }
    }
  }

  lastButtonState = reading;

  if (morseInput.length() > 0 && (millis() - lastInputTime > letterPause)) {
    String letter = decodeMorse(morseInput);
    decodedText += letter;

    morseInput = "";
    showStatus();

    delay(300);
  }

  if (decodedText.length() > 11) {
    delay(500);
    decodedText = "";
    lcd.clear();
    showStatus();
  }
}
