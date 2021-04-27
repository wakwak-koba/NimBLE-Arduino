/*
 * catch notifies and indicates from BLEServer
 *
 * https://twitter.com/wakwak_koba/
 */

#include <NimBLEDevice.h>

void printCharacteristic(BLERemoteCharacteristic* pRemoteCharacteristic) {
  Serial.printf("%s handle:%02d service:%s characteristic:%s ", pRemoteCharacteristic->getRemoteService()->getClient()->getPeerAddress().toString().c_str(), pRemoteCharacteristic->getHandle(), pRemoteCharacteristic->getRemoteService()->getUUID().toString().c_str(), pRemoteCharacteristic->getUUID().toString().c_str());
}

void subscribeCallback(BLERemoteCharacteristic* pRemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify)  {
  printCharacteristic(pRemoteCharacteristic);
  Serial.print(isNotify ? "notify" : "indicate");
  Serial.printf("Callback:(%d)", length);
  for (int i=0; i<length; i++)
    Serial.printf(" %02x", pData[i]);
  Serial.println();
}

void setup() {
  Serial.begin(115200);

  NimBLEDevice::init("");
  auto pBLEScan = NimBLEDevice::getScan();
  pBLEScan->setActiveScan(true);

  Serial.println("wait 10 secs..");
  auto pScanResults = pBLEScan->start(10);

  for (int i = 0; i < pScanResults.getCount(); i++) {
    auto advertisedDevice = pScanResults.getDevice(i);
    if (advertisedDevice.haveServiceUUID())  {
      Serial.print("Found Device ");
      Serial.print(advertisedDevice.toString().c_str());
      Serial.println();
      auto pClient = NimBLEDevice::createClient();
      if(pClient && pClient->connect(&advertisedDevice)) {
        auto pServices = pClient->getServices(true);
        for (auto pService : *pServices)  {
          auto pCharacteristics = pService->getCharacteristics(true);
          for (auto pCharacteristic : *pCharacteristics) {
            if(pCharacteristic->canRead()) {
              std::string rd = pCharacteristic->readValue();
              if(rd.size() > 0) {
                printCharacteristic(pCharacteristic);
                Serial.printf("readValue:(%d)", rd.size());
                for(int i = 0; i < rd.size(); i++)
                  Serial.printf(" %02x", rd[i]);
                Serial.println();
              }
            }
            if(pCharacteristic->canNotify()) {
              printCharacteristic(pCharacteristic);
              Serial.printf("registerForNotify:%s", pCharacteristic->subscribe(true, subscribeCallback) ? "true" : "false");
              Serial.println();
            }
            if(pCharacteristic->canIndicate()) {
              printCharacteristic(pCharacteristic);
              Serial.printf("registerForIndicate:%s", pCharacteristic->subscribe(false, subscribeCallback) ? "true" : "false");
              Serial.println();
            }
          }
        }
      }
    }
  }
  
  if(NimBLEDevice::getClientListSize() == 0)
    ESP.restart();
}

void loop() {
  delay(1);
}