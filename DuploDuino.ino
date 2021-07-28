#include <ArduinoBLE.h>

const char* deviceServiceUuid = "00001623-1212-efde-1623-785feabcd123";
const char* trainCharacteristicUuid ="00001624-1212-efde-1623-785feabcd123";
   
BLECharacteristic trainCharacteristic;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }

  Serial.println("BLE Central - Peripheral Explorer");

  // start scanning for peripherals
    BLE.scanForUuid(deviceServiceUuid);

}

void loop() {
  BLEDevice peripheral = BLE.available();

if (peripheral) {
    // discovered a peripheral, print out address, local name, and advertised service
    Serial.print("Found ");
    Serial.print(peripheral.address());
    Serial.print(" '");
    Serial.print(peripheral.localName());
    Serial.print("' ");
    Serial.print(peripheral.advertisedServiceUuid());
    Serial.println();



    // stop scanning
    BLE.stopScan();

   controlLed(peripheral);

    // peripheral disconnected, start scanning again
    BLE.scanForUuid(deviceServiceUuid);
  }
}

  void controlLed(BLEDevice peripheral) 
  {
  // connect to the peripheral
  Serial.println("Connecting ...");

  if (peripheral.connect()) {
    Serial.println("Connected");
  } else {
    Serial.println("Failed to connect!");
    return;
  }  

   // discover peripheral attributes
  Serial.println("Discovering attributes ...");
  if (peripheral.discoverAttributes()) {
    Serial.println("Attributes discovered");
  } else {
    Serial.println("Attribute discovery failed!");
    peripheral.disconnect();
    return;
  }

    Serial.println("retreive characteristic!"); 
   trainCharacteristic = peripheral.characteristic(trainCharacteristicUuid);

  if (!trainCharacteristic) {
    Serial.println("Peripheral does not have LED characteristic!");
   // peripheral.disconnect();
   // return;
  } else if (!trainCharacteristic.canWrite()) {
    Serial.println("Peripheral does not have a writable LED characteristic!");
   // peripheral.disconnect();
   // return;
  }

  
  // while (peripheral.connected()) {
    
    moveTrain(); 
 // }

  Serial.println("Peripheral disconnected");
}

void lightOn(){
      byte led1[10] = {0x0a, 0x00, 0x41, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01};
      byte led2[8] = {0x08, 0x00, 0x81, 0x11, 0x11, 0x51, 0x00, 0x0a};
      
      trainCharacteristic.writeValue(led1,10);
      trainCharacteristic.writeValue(led2,8);
}

void playHorn(){
       byte horn1[10] = {0x0a, 0x00, 0x41, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01};  // Ports 0, 1, 2, 3, 4
       byte horn2[8] = {0x08, 0x00, 0x81, 0x01, 0x11, 0x51, 0x01, 0x09};
  
      trainCharacteristic.writeValue(horn1,10);
      trainCharacteristic.writeValue(horn2,8);
}

void washTrain(){
       byte wash1[10] = {0x0a, 0x00, 0x41, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01};  // Ports 0, 1, 2, 3, 4
       byte wash2[8] = {0x08, 0x00, 0x81, 0x01, 0x11, 0x51, 0x01, 0x07};
  
      trainCharacteristic.writeValue(wash1,10);
      trainCharacteristic.writeValue(wash2,8);
}

void moveTrain(){
   byte wash1[10] = {0x0a, 0x00, 0x41, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01};  // Ports 0, 1, 2, 3, 4
   byte wash2[8] = {0x08, 0x00, 0x81, 0x00, 0x01, 0x51, 0x00, 0x64};
  
  trainCharacteristic.writeValue(wash1,10);
  trainCharacteristic.writeValue(wash2,8);

}
