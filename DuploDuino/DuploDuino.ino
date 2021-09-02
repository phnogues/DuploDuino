#include <ArduinoBLE.h>

BLECharacteristic trainCharacteristic;

// Train addresses
const char *deviceServiceUuid = "00001623-1212-efde-1623-785feabcd123";
const char *trainCharacteristicUuid = "00001624-1212-efde-1623-785feabcd123";
const byte prepval[10] = {0x0a, 0x00, 0x41, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01};

// Buttons
const int blueButtonPin = 2;
const int yellowButtonPin = 3;
const int whiteButtonPin = 5;
const int redButtonPin = 4;

const int potPin = A1;

int blueButtonState = 0;
int yellowButtonState = 0;
int redButtonState = 0;
int whiteButtonState = 0;

int potvalueMapped = 0;

// Leds
const int blueButtonledPin = 9;
boolean trainStopped = true;

void setup()
{
  // buttons and leds
  pinMode(blueButtonPin, INPUT_PULLUP);
  pinMode(yellowButtonPin, INPUT_PULLUP);
  pinMode(whiteButtonPin, INPUT_PULLUP);
  pinMode(redButtonPin, INPUT_PULLUP);

  // pinMode(blueButtonledPin, OUTPUT);

  Serial.begin(9600);
  //while (!Serial)    ;

  // begin initialization
  if (!BLE.begin())
  {
    Serial.println("starting BLE failed!");

    while (1)
      ;
  }

  Serial.println("DuploDuino started");

  // start scanning for peripherals
  BLE.scanForUuid(deviceServiceUuid);
}

void loop()
{
  BLEDevice peripheral = BLE.available();

  if (peripheral)
  {
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

    controlTrain(peripheral);

    // peripheral disconnected, start scanning again
    BLE.scanForUuid(deviceServiceUuid);
  }
}

void controlTrain(BLEDevice peripheral)
{
  // connect to the peripheral
  Serial.println("Connecting ...");

  if (peripheral.connect())
  {
    Serial.println("Connected");
  }
  else
  {
    Serial.println("Failed to connect!");
    return;
  }

  // discover peripheral attributes
  Serial.println("Discovering attributes ...");
  if (peripheral.discoverAttributes())
  {
    Serial.println("Attributes discovered");
  }
  else
  {
    Serial.println("Attribute discovery failed!");
    peripheral.disconnect();
    return;
  }

  Serial.println("retreive characteristic!");
  trainCharacteristic = peripheral.characteristic(trainCharacteristicUuid);

  if (!trainCharacteristic)
  {
    Serial.println("Peripheral does not have LED characteristic!");
    // peripheral.disconnect();
    // return;
  }
  else if (!trainCharacteristic.canWrite())
  {
    Serial.println("Peripheral does not have a writable LED characteristic!");
    // peripheral.disconnect();
    // return;
  }

  while (peripheral.connected())
  {
    blueButtonState = digitalRead(blueButtonPin);
    yellowButtonState = digitalRead(yellowButtonPin);
    whiteButtonState = digitalRead(whiteButtonPin);
    redButtonState = digitalRead(redButtonPin);

    //potValue = analogRead(potPin);
    potvalueMapped = map(analogRead(potPin), 0, 1023, 0, 255);
    //Serial.println(potvalueMapped);

    moveTrain(potvalueMapped);

    if (blueButtonState == LOW)
    {
      washTrain();
    }

    if (yellowButtonState == LOW)
    {
      playHorn();
    }

    if (whiteButtonState == LOW)
    {
      lightOn();
    }

    if (redButtonState == LOW)
    {
      stopTrain();
    }

    delay(1);
  }

  Serial.println("Peripheral disconnected");
}

bool lightIsOn = false;

void lightOn()
{
  Serial.println("Light");

  byte valOn[8] = {0x08, 0x00, 0x81, 0x11, 0x11, 0x51, 0x00, 0x0a};
  byte valOff[8] = {0x08, 0x00, 0x81, 0x11, 0x11, 0x51, 0x00, 0x00};

  trainCharacteristic.writeValue(prepval, 10);

  if (lightIsOn) {
    trainCharacteristic.writeValue(valOff, 8);
  } else {
    trainCharacteristic.writeValue(valOn, 8);
  }

  lightIsOn = !lightIsOn;
}

void playHorn()
{
  Serial.println("Play horn");

  byte val[8] = {0x08, 0x00, 0x81, 0x01, 0x11, 0x51, 0x01, 0x09};

  trainCharacteristic.writeValue(prepval, 10);
  trainCharacteristic.writeValue(val, 8);
}

void washTrain()
{
  Serial.println("Wash train");

  byte val[8] = {0x08, 0x00, 0x81, 0x01, 0x11, 0x51, 0x01, 0x07};

  trainCharacteristic.writeValue(prepval, 10);
  trainCharacteristic.writeValue(val, 8);

  // blue led on
  //digitalWrite(blueButtonledPin, HIGH);
}

void stopTrain()
{
  Serial.println("Stop train");

  byte val[8] = {0x08, 0x00, 0x81, 0x00, 0x01, 0x51, 0x00, 0x00};

  trainCharacteristic.writeValue(prepval, 10);
  trainCharacteristic.writeValue(val, 8);

  trainStopped = true;
}

int lastSpeed = 0;
void moveTrain(int speed)
{
  if ((speed + 10) <= lastSpeed || (speed - 10) >= lastSpeed) {
    //Serial.println("train started");
    //Serial.println(potvalueMapped);
    lastSpeed = speed;
    trainStopped = false;
  }

  if (trainStopped) {
    return;
  }

  byte val0[8] = {0x08, 0x00, 0x81, 0x00, 0x01, 0x51, 0x00, 0x64};

  // reverse
  byte val1[8] = {0x08, 0x00, 0x81, 0x00, 0x01, 0x51, 0x00, 0x1e}; // Too slow
  byte val2[8] = {0x08, 0x00, 0x81, 0x00, 0x01, 0x51, 0x00, 0x32};
  byte val3[8] = {0x08, 0x00, 0x81, 0x00, 0x01, 0x51, 0x00, 0x64};

  // go forward
  byte val4[8] = {0x08, 0x00, 0x81, 0x00, 0x01, 0x51, 0x00, 0xe2};
  byte val5[8] = {0x08, 0x00, 0x81, 0x00, 0x01, 0x51, 0x00, 0xce};
  byte val6[8] = {0x08, 0x00, 0x81, 0x00, 0x01, 0x51, 0x00, 0x9c};

  trainCharacteristic.writeValue(prepval, 10);

  // reverse
  if (speed < 160 && speed > 140) {
    trainCharacteristic.writeValue(val3, 8);
  }

  if (speed < 140 && speed > 130) {
    trainCharacteristic.writeValue(val2, 8);
  }

  // stop it
  if (speed < 130 && speed > 110) {
    stopTrain();
  }

  // forward
  if (speed < 110 && speed > 95) {
    trainCharacteristic.writeValue(val5, 8);
  }

  if (speed < 95 && speed > 85) {
    trainCharacteristic.writeValue(val6, 8);
  }
}
