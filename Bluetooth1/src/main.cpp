#include <BleKeyboard.h>
#include <Arduino.h>

#define onboardled 2 // On board led (who could have guessed)
#define przycisk 4 // Button1
#define przycisk2 16 // Button2
#define przycisk3 17 // Button3

// Bluetooth
const char* device_name = "Tymek MacroPad"; // Name
bool is_connected = false;
BleKeyboard bleKeyboard(device_name, "Tymek", 100); // (name, manufacturer, battery level %)

// Przycisk
int lastState[] = {HIGH,HIGH,HIGH};
int currentState[2];
const long debounceDelay = 150; // time between button presses
unsigned long lastDebounce[] = {0,0,0};
unsigned long holdTime[2];
unsigned long holdTimeRequired = 1000; // time to repeat press

// Czas
unsigned long lastConnectionCheck = 0;
const unsigned long connectionCheckInterval = 500;
unsigned long timerStart1[] = {0,0,0};



// Functions list
void keyboardPress(uint8_t klawisze[], uint8_t numKeys); // Transfer keys into keypresses
void buttonPressDetect(unsigned long, int, int); // Detect button press
void bluetoothCheckManager(unsigned long); //Manages Bluetooth
void buttonPressScan(unsigned long); // Activates all button detections
void sendKey(int); // manages the selection of keys based on button presseed, and then executes these keys

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
  Serial.println("v 0.5");
  Serial.println("Tymoteusz Brzumowski");
  delay (100);
  digitalWrite(onboardled, LOW); 
}

void loop() {
  unsigned long currentMillis = millis();

  bluetoothCheckManager(currentMillis);
  buttonPressScan(currentMillis);
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

void sendKey(int button){
  uint8_t tab[5];
  uint8_t keycount=0;
  switch (button) {
    case przycisk:
     tab[keycount++] =KEY_F13;
     tab[keycount++] ='s';
    break;
    case przycisk2:
     tab[keycount++] ='u';
    break;
    case przycisk3:
     tab[keycount++] ='s';
    break;
  }
  keyboardPress(tab, keycount);
}

void buttonPressDetect(unsigned long currentMillis, int button, int number){
  currentState[number] = digitalRead(button);
  holdTime[number]=currentMillis-timerStart1[number];
  if ((currentMillis - lastDebounce[number]) > debounceDelay) {
    
    if (lastState[number] == HIGH && currentState[number] == LOW ) {
      timerStart1[number]=currentMillis;
      //Serial.println("Przycisk nacisniety!");
        lastDebounce[number]=currentMillis;
        sendKey(button);
    }
    else if (currentState[number] == LOW && holdTime[number] > holdTimeRequired){
      lastDebounce[number]=currentMillis;
      sendKey(button);
    }
    else {
      holdTime[number]= 0;
    }
  }

  lastState[number] = currentState[number];
}

void buttonPressScan(unsigned long currentMillis){
  
  int buttonList[] = {przycisk, przycisk2, przycisk3};
  for (int i=0; i<sizeof(buttonList)/sizeof(buttonList[0]);i++){
  buttonPressDetect(currentMillis, buttonList[i], i);
  }
}