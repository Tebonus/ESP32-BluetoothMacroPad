#include <BleKeyboard.h>
#include <Arduino.h>

#define onboardled 2 // Led na urzadzeniu
#define przycisk 4 // Przycisk

//Bluetooth
const char* device_name = "Tymek"; // Nazwa
bool is_connected = false; // Podloczono urzadzenie

//Przycisk
int lastState = HIGH;
int currentState;
const long debounceDelay = 100; //Czas miedzy nacisnieciami
unsigned long lastDebounce = 0;

//Czas
unsigned long lastConnectionCheck = 0;
const unsigned long connectionCheckInterval = 500;

BleKeyboard bleKeyboard(device_name, "Tymek", 100); // (name, manufacturer, battery level %)

void buttonPress(uint8_t klawisze[], uint8_t numKeys); //Funkcja nacisniecia skrotu 

void setup() {
  pinMode(przycisk, INPUT_PULLUP);
  pinMode(onboardled, OUTPUT);
  Serial.begin(921600);
  bleKeyboard.begin(); // Start BLE Keyboard
  delay(500);
  Serial.println("Starting the Bluetooth Keyboard"); 
  Serial.println("Tymoteusz Brzumowski"); 
}

void loop() {
  unsigned long currentMillis = millis();

  // Sprawdzenie polaczenia
  if (currentMillis - lastConnectionCheck >= connectionCheckInterval) {
    
    lastConnectionCheck = currentMillis;
    bool connected = bleKeyboard.isConnected();

      if (connected && !is_connected) {
        Serial.println("Polaczono!");
        digitalWrite(onboardled, HIGH);
        delay(250);  
        digitalWrite(onboardled, LOW);
        is_connected = true;
      }

      if (!connected && is_connected) {
        Serial.println("Rozlaczono!");
        is_connected = false;
      }
  }

  // Button press detection 
  currentState = digitalRead(przycisk);

  if ((currentMillis - lastDebounce) > debounceDelay) {
    if (lastState == HIGH && currentState == LOW) {
      //Serial.println("Przycisk nacisniety!");
        lastDebounce=currentMillis;
      if (bleKeyboard.isConnected()) {
        uint8_t tab[] ={KEY_F13, 's'};
       buttonPress(tab, sizeof(tab)/sizeof(tab[0]));
      }
    }
  }

  lastState = currentState;
}
void buttonPress (uint8_t klawisze[], uint8_t numKeys){
    for (int i=0; i<numKeys; i++){
      bleKeyboard.press(klawisze[i]);
    }
    delay(50);
    bleKeyboard.releaseAll();
    
}