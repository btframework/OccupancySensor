// ESP32C3FN4 SuperMini Board
// ===============================================================
// Arduino IDE settings:
//   - Board: ESP32C3 Dev BModule
//   - ESP CDC On Boot: Enabled
//   - CPU Frequency: 80MHz (WiFi)
//   - Core Debug Level: None
//   - Erase All Flash Before Sketch Upload: Disabled
//   - Flash frequency: 80Mhz
//   - Flash Mode: QIO
//   - Flash Size: 4MB (32Mb)
//   - JTAG Adapter: Disabled
//   - Partition Scheme: Huge APP (3MB No OTA/1MB SPIFFS)
//   - Upload Speed: 921600
//   - Zigbee Mode: Disabled
//   - Programmer: Esptool
// ===============================================================

#include <HomeSpan.h>


/**************************************************************************************/
/*                                        Pins                                        */

#define STATUS_LED_PIN          GPIO_NUM_8
#define CONTROL_PIN             GPIO_NUM_9

#define PRESENSE_DETECTION_PIN  GPIO_NUM_10

/**************************************************************************************/


/**************************************************************************************/
/*                                   Occupacy sensor                                  */

struct OccupancySensor : Service::OccupancySensor
{
    SpanCharacteristic* FOccupacy;

    OccupancySensor() : Service::OccupancySensor()
    {
        FOccupacy = new Characteristic::OccupancyDetected();
        FOccupacy->setVal(digitalRead(PRESENSE_DETECTION_PIN) == HIGH);
    }
    
    void loop()
    {
        // Check is sensor status changed.
        bool NewVal = (digitalRead(PRESENSE_DETECTION_PIN) == HIGH);
        if (NewVal != FOccupacy->getVal())
            // If yes, set new value.
            FOccupacy->setVal(NewVal);
    }
};

/**************************************************************************************/


// Arduino initialization routine.
void setup()
{
    // Initialize debug serial port.
    Serial.begin(115200);

    // Initialize pins.
    pinMode(STATUS_LED_PIN, OUTPUT);
    pinMode(CONTROL_PIN, INPUT);
    pinMode(PRESENSE_DETECTION_PIN, INPUT);

    digitalWrite(STATUS_LED_PIN, HIGH);

    // Initialize HomeSpan.
    homeSpan.setControlPin(CONTROL_PIN);
    homeSpan.setStatusPin(STATUS_LED_PIN);
    homeSpan.setPairingCode("63005612");
    homeSpan.begin(Category::Sensors, "DroneTales Occupancy Sensor");

    // Build device's serial number.
    char Sn[24];
    snprintf(Sn, 24, "DRONETALES-%llX", ESP.getEfuseMac());

    // Configure doorbell.
    new SpanAccessory();
    new Service::AccessoryInformation();
    new Characteristic::Identify();
    new Characteristic::Manufacturer("DroneTales");
    new Characteristic::SerialNumber(Sn);
    new Characteristic::Model("DroneTales Occupancy Sensor");
    new Characteristic::FirmwareRevision("1.0.0.0");
    new Characteristic::Name("OccupancySensor");
    new OccupancySensor();
}

// Arduino main loop.
void loop()
{
    homeSpan.poll();
}
