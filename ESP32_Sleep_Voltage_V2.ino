#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>


BLECharacteristic *pCharacteristic;
int buttonState = 0;
bool deviceConnected = false;
int txValue = 0;
int PINSTATUS;
const float mvc = 3.3;
float counts = 0;
float voltage_float = 0;

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
                                                                    //#define READ_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8" // THIS UUID WILL NEED CHANGED //
#define BUTTON_PIN 25 // RTC GIOP25 pin connected to button
#define cellpin 36 // Pin 36 on M5 stack


class MyServerCallbacks: public BLEServerCallbacks{
  void onConnect (BLEServer* pServer) {
    deviceConnected = true;
  };
  void onDisconnect (BLEServer* pServer){
    deviceConnected = false;
  }
};


void setup() {
  Serial.begin(9600);
  Serial.println("Starting BLE prg!");

BLEDevice::init("SMART_MEDICINE_CAP");
BLEServer *pServer = BLEDevice::createServer();
pServer->setCallbacks(new MyServerCallbacks());
BLEService *pService = pServer->createService(SERVICE_UUID);
pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ);
//pCharacteristic2 = pService->createCharacteristic(CHARACTERISTIC_UUID2, BLECharacteristic::PROPERTY_NOTIFY); This was going to be another characteristic to write back to
pCharacteristic->addDescriptor(new BLE2902());
//pCharacteristic2->addDescriptor(new BLE2902());
pService->start();
pServer->getAdvertising()->start();
Serial.println("waiting for connection");

pinMode(BUTTON_PIN, INPUT_PULLUP);

int buttonState = 0;
}

void loop() {
  

////////// This section is for reading the voltage of the battery //////////
    counts = analogRead(cellpin);
    Serial.println(String(counts));
    float voltage_float = (counts+100) * mvc /4096 * 2;
    Serial.println(String(voltage_float)+"V");

    if (voltage_float >= 3) {
      pCharacteristic->setValue("Battery Full");
    } else if (voltage_float < 3 & voltage_float > 2.7) {
      pCharacteristic->setValue("Battery is Okay");
    } else {
    pCharacteristic->setValue("Battery low");
    }
     pCharacteristic->notify();
     delay(1000);
     buttonState = digitalRead(BUTTON_PIN);

if (buttonState == HIGH) {
    Serial.println("Board is now asleep");
    // Enter Sleep mode
    delay(100);
    esp_deep_sleep_start();
  }
if (buttonState == LOW) {
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_25, 0);
  delay(100);
  Serial.println("Board is now awake");
}
}