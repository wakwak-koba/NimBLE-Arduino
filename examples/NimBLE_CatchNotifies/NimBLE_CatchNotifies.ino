/*
 * https://twitter.com/wakwak_koba/
 */

#include <NimBLEDevice.h>

static void notifyCallback(BLERemoteCharacteristic* pRemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify)  {
  Serial.printf("notifyCallback: %s %s handle: %d value:", pRemoteCharacteristic->getRemoteService()->getClient()->getPeerAddress().toString().c_str(), pRemoteCharacteristic->getUUID().toString().c_str(), pRemoteCharacteristic->getHandle());
  for (int i=0; i<length; i++)
    Serial.printf(" %02x", pData[i]);
  Serial.println();
}

void setup() {
  Serial.begin(115200);

  NimBLEDevice::init("");
  auto* pBLEScan = NimBLEDevice::getScan();
  pBLEScan->setActiveScan(true);

  Serial.println("wait 10 secs..");
  auto pScanResults = pBLEScan->start(10);

  for (int i = 0; i < pScanResults.getCount(); i++) {
    auto advertisedDevice = pScanResults.getDevice(i);
    if (advertisedDevice.haveServiceUUID())  {
      Serial.print("Found Device ");
      Serial.print(advertisedDevice.toString().c_str());
      auto pClient = NimBLEDevice::createClient();
      if(pClient && pClient->connect(&advertisedDevice)) {
        auto pServices = pClient->getServices(true);
        for (auto pService : *pServices)  {
          auto pCharacteristics = pService->getCharacteristics(true);
          for (auto pCharacteristic : *pCharacteristics)
            if(pCharacteristic->canNotify())
              if(pCharacteristic->registerForNotify(notifyCallback)) {
                Serial.printf("registerForNotify: %s %s handle:%d", pClient->getPeerAddress().toString().c_str(), pCharacteristic->getUUID().toString().c_str(), pCharacteristic->getHandle());
                Serial.println();
              }
        }
      }
      Serial.println();
    }
  }
}

void loop() {
  delay(1);
}