// What: Controlling a LED Through Bluetooth® with Nano 33 IoT
// Where: https://docs.arduino.cc/tutorials/nano-33-iot/bluetooth + Example Program
// Why: Using app on phone to control LED through Bluetooth

// What: Stream IMU data from Arduino to another device example
// Where: https://github.com/ucla-hci/m119/blob/main/m2b_peripheral/m2b_peripheral.ino
// Why: Link from M2 to example approach for accelerometer

#include <ArduinoBLE.h>
#include <Arduino_LSM6DS3.h>

long previousMillis = 0;
int interval = 0;

// BLE Acc Characteristic - custom 128-bit UUID, read and writable by central

BLEService imuService("1101"); //BLE Accelerometer & Gyroscope service - Serial Port

//x, y, z for accelerometer and gyroscope
BLEFloatCharacteristic accCharacteristicX("2101", BLERead | BLEWrite);
BLEFloatCharacteristic accCharacteristicY("2102", BLERead | BLEWrite);
BLEFloatCharacteristic accCharacteristicZ("2103", BLERead | BLEWrite);
BLEFloatCharacteristic gyrCharacteristicX("2104", BLERead | BLEWrite);
BLEFloatCharacteristic gyrCharacteristicY("2105", BLERead | BLEWrite);
BLEFloatCharacteristic gyrCharacteristicZ("2106", BLERead | BLEWrite);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // set built in LED pin to output mode
  pinMode(LED_BUILTIN, OUTPUT);

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy failed!");

    while (1);
  }
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("Nano 33 IoT");
  BLE.setAdvertisedService(imuService);

  // add the characteristic to the service
  imuService.addCharacteristic(accCharacteristicX);
  imuService.addCharacteristic(accCharacteristicY);
  imuService.addCharacteristic(accCharacteristicZ);
  imuService.addCharacteristic(gyrCharacteristicX);
  imuService.addCharacteristic(gyrCharacteristicY);
  imuService.addCharacteristic(gyrCharacteristicZ);

  // add service
  BLE.addService(imuService);

  // set the initial value for the characteristic:
  accCharacteristicX.writeValue(0);
  accCharacteristicY.writeValue(0);
  accCharacteristicZ.writeValue(0);
  gyrCharacteristicX.writeValue(0);
  gyrCharacteristicY.writeValue(0);
  gyrCharacteristicZ.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("BLE IMU Peripheral");
  Serial.println("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");
  Serial.print("Gyroscope sample rate = ");
  Serial.print(IMU.gyroscopeSampleRate());
  Serial.println(" Hz");
  Serial.println();
}

void loop() {
  float ax, ay, az, gx, gy, gz;
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
      // IMU data:
      if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
        IMU.readAcceleration(ax, ay, az);
        IMU.readGyroscope(gx, gy, gz);
        // BLE Write
        accCharacteristicX.writeValue(ax);
        accCharacteristicY.writeValue(ay);
        accCharacteristicZ.writeValue(az);
        gyrCharacteristicX.writeValue(gx);
        gyrCharacteristicY.writeValue(gy);
        gyrCharacteristicZ.writeValue(gz);
      }
    }
    
    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}