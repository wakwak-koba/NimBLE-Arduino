/*
 * https://twitter.com/wakwak_koba/
 */

#include <NimBLEDevice.h>

static void notifyCallback(BLERemoteCharacteristic* pRemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify)  {
  Serial.printf("notifyCallback: %s %s handle: %02x value:", pRemoteCharacteristic->getRemoteService()->getClient()->getPeerAddress().toString().c_str(), pRemoteCharacteristic->getUUID().toString().c_str(), pRemoteCharacteristic->getHandle());
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
        auto* pRemoteServiceMap = pClient->getServices();
        for (auto itr : *pRemoteServiceMap)  {
          auto *pCharacteristicMap = itr.second->getCharacteristicsByHandle();
          for (auto itr : *pCharacteristicMap)
            if(itr.second->canNotify())
              if(itr.second->registerForNotify(notifyCallback)) {
                Serial.printf("registerForNotify: %s %s handle:%02x", pClient->getPeerAddress().toString().c_str(), itr.second->getUUID().toString().c_str(), itr.second->getHandle());
                Serial.println();
              }
        }
      }
    }
  }
}

void loop() {
  delay(1);
}
