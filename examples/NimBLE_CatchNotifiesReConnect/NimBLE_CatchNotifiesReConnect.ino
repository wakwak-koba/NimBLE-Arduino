/*
 * https://twitter.com/wakwak_koba/
 */

#include <NimBLEDevice.h>

void notifyCallback(BLERemoteCharacteristic* pRemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify)  {
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
      auto pClient = NimBLEDevice::createClient(advertisedDevice.getAddress());
      if(pClient)
      {
//      pClient->setConnectTimeout(1);    during trial and error
        Serial.print("Found Device ");
        Serial.println(advertisedDevice.toString().c_str());
      }        
    }
  }
  
  if(NimBLEDevice::getClientListSize() == 0)
    ESP.restart();  
}

void loop() {
  auto pClients = NimBLEDevice::getClientList();
  for(auto pClient : *pClients) {
    if(!pClient->isConnected() && pClient->connect()) {
      Serial.printf("connect: %s", pClient->getPeerAddress().toString().c_str());
      Serial.println();
      auto pServices = pClient->getServices(true);
      for (auto pService : *pServices)  {
        auto pCharacteristics = pService->getCharacteristics(true);
        for (auto pCharacteristic : *pCharacteristics)
          if(pCharacteristic->canNotify() && pCharacteristic->registerForNotify(notifyCallback)) {
            Serial.printf("registerForNotify: %s %s handle:%d", pClient->getPeerAddress().toString().c_str(), pCharacteristic->getUUID().toString().c_str(), pCharacteristic->getHandle());
            Serial.println();
          }
      }
    }
  }

  delay(1);
}