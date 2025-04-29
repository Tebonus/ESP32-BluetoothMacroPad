#include <BleKeyboard.h>
#include <Arduino.h>

#define onboardled 2 // Led na urzadzeniu
#define przycisk 4 // Przycisk
#define przycisk2 16 // Button2
#define przycisk3 17 // Button3
// Bluetooth
const char* device_name = "Tymek MacroPad"; // Nazwa
bool is_connected = false; // Podloczono urzadzenie
BleKeyboard bleKeyboard(device_name, "Tymek", 100); // (name, manufacturer, battery level %)

// Przycisk
int lastState[] = {HIGH,HIGH,HIGH};
int currentState[2];
const long debounceDelay = 100; // Czas miedzy nacisnieciami
unsigned long lastDebounce[] = {0,0,0};

// Czas
unsigned long lastConnectionCheck = 0;
const unsigned long connectionCheckInterval = 500;



// Functions list
void keyboardPress(uint8_t klawisze[], uint8_t numKeys); // Transfer keys into keypresses
void buttonPressDetect(unsigned long, int, int); // Detect button press
void bluetoothCheckManager(unsigned long); //Manages Bluetooth
void buttonPressScan(unsigned long); // Activates all button detections
int chceckButtonShortcut(uint8_t, uint8_t, int);

void setup() {
  pinMode(przycisk, INPUT_PULLUP);
  pinMode(przycisk2, INPUT_PULLUP);
  pinMode(przycisk3, INPUT_PULLUP);
  pinMode(onboardled, OUTPUT);
  digitalWrite(onboardled, HIGH);
  Serial.begin(921600);
  bleKeyboard.begin(); // Start BLE Keyboard
  delay(500);
  Serial.println("Starting the Bluetooth Keyboard"); 
  Serial.println("v 0.1.1");
  Serial.println("Tymoteusz Brzumowski");
  delay (100);
  digitalWrite(onboardled, LOW); 
}

void loop() {
  unsigned long currentMillis = millis();

  bluetoothCheckManager(currentMillis);
  buttonPressScan(currentMillis);
  //buttonPressDetect(currentMillis, przycisk2);
}

//Functions
  //Bluetooth
void bluetoothCheckManager(unsigned long currentMillis){
  if (currentMillis - lastConnectionCheck >= connectionCheckInterval) {
    
    lastConnectionCheck = currentMillis;
    bool connected = bleKeyboard.isConnected();

      if (connected && !is_connected) {
        Serial.println("Polaczono!");
        digitalWrite(onboardled, HIGH);
        delay(300);  
        digitalWrite(onboardled, LOW);
        is_connected = true;
      }

      if (!connected && is_connected) {
        Serial.println("Rozlaczono!");
        is_connected = false;
      }
  }

}
  //Buttons
void keyboardPress (uint8_t klawisze[], uint8_t numKeys){ 
    for (int i=0; i<numKeys; i++){
      bleKeyboard.press(klawisze[i]);
    }
    delay(50);
    bleKeyboard.releaseAll();
    
}
int chceckButtonShortcut(uint8_t tab[], uint8_t keycount, int button){
  switch (button) {
    case przycisk:
     tab[keycount++] = KEY_F13;
     tab[keycount++] ='s';
     tab[keycount++] = 'a';
    break;
    default:
     tab[keycount++] = KEY_F13;
     tab[keycount++] ='s';
    break;
  }
  return 0;
}

void buttonPressDetect(unsigned long currentMillis, int button, int number){
  currentState[0] = digitalRead(button);

  if ((currentMillis - lastDebounce[0]) > debounceDelay) {
    if (lastState[0] == HIGH && currentState[0] == LOW) {
      //Serial.println("Przycisk nacisniety!");
        lastDebounce[0]=currentMillis;

      if (bleKeyboard.isConnected()) {
        
        uint8_t tab[5];
        uint8_t keycount=0;
        switch (button) {
          case przycisk:
           tab[keycount++] = KEY_F13;
           tab[keycount++] ='s';
           tab[keycount++] = 'a';
          break;
          default:
           tab[keycount++] = KEY_F13;
           tab[keycount++] ='s';
          break;
        }
        //chceckButtonShortcut(tab[5],keycount,button);
       keyboardPress(tab, keycount);
      }
    }
  }

  lastState[0] = currentState[0];
}

void buttonPressScan(unsigned long currentMillis){
  
  int buttonList[] = {przycisk, przycisk2, przycisk3};
  for (int i=0; i<sizeof(buttonList)/sizeof(buttonList[0]);i++){
  buttonPressDetect(currentMillis, buttonList[i], i);
  }
}