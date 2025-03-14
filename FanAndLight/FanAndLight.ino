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

// This firmware combines LD2420 occupancy sensor and 2 relays:
// one for light and other one for fun.


#include <HomeSpan.h>

/**************************************************************************************/
/*                                        Pins                                        */

#define LAMP_RELAY_PIN      GPIO_NUM_6
#define FAN_RELAY_PIN       GPIO_NUM_7

#define STATUS_LED_PIN      GPIO_NUM_8
#define CONTROL_PIN         GPIO_NUM_9

#define SENSOR_EVENT_PIN    GPIO_NUM_10

/**************************************************************************************/


/**************************************************************************************/
/*                                  HomeSpan sensors                                  */

// Device record.
struct DeviceIdentify : Service::AccessoryInformation
{
    DeviceIdentify(const char* const Name, const char* const Serial)
        : Service::AccessoryInformation()
    {
        new Characteristic::Name(Name);
        new Characteristic::Manufacturer("DroneTales");
        new Characteristic::SerialNumber(Serial);
        new Characteristic::FirmwareRevision("1.0.0");
        new Characteristic::Identify();
    }
};

// Occupacy sensor record.
struct OccupancySensor : Service::OccupancySensor
{
    SpanCharacteristic* FOccupacy;

    OccupancySensor() : Service::OccupancySensor()
    {
        FOccupacy = new Characteristic::OccupancyDetected();
        FOccupacy->setVal(digitalRead(SENSOR_EVENT_PIN) == HIGH);
    }
    
    void loop()
    {
        // Check is sensor status changed.
        bool NewVal = (digitalRead(SENSOR_EVENT_PIN) == HIGH);
        if (NewVal != FOccupacy->getVal())
            // If yes, set new value.
            FOccupacy->setVal(NewVal);
    }
};

// Lamp relay record.
struct LampRelay : Service::LightBulb
{
    SpanCharacteristic* FPower;
    
    LampRelay() : Service::LightBulb()
    {
        FPower = new Characteristic::On();
    }
    
    bool update()
    {
        digitalWrite(LAMP_RELAY_PIN, FPower->getNewVal());
        return true;
    }
};

// Fan relay record.
struct FanRelay : Service::Fan
{
    SpanCharacteristic* FActive;
    
    FanRelay() : Service::Fan()
    {
        FActive = new Characteristic::Active();
    }
    
    bool update()
    {
        digitalWrite(FAN_RELAY_PIN, FActive->getNewVal());
        return true;
    }
};

/**************************************************************************************/


/**************************************************************************************/
/*                                 Arduino  rountines                                 */

// Arduino initialization routine.
void setup()
{
    // Initialize USB UART.
    Serial.begin(115200);

    // Initialize pins.
    pinMode(CONTROL_PIN, INPUT);
    pinMode(SENSOR_EVENT_PIN, INPUT);

    pinMode(LAMP_RELAY_PIN, OUTPUT);
    pinMode(FAN_RELAY_PIN, OUTPUT);
    pinMode(STATUS_LED_PIN, OUTPUT);

    // Turn all output pins off.
    digitalWrite(LAMP_RELAY_PIN, LOW);
    digitalWrite(FAN_RELAY_PIN, LOW);
    digitalWrite(STATUS_LED_PIN, HIGH);

    // Initialize HomeSpan.
    homeSpan.setControlPin(CONTROL_PIN);
    homeSpan.setStatusPin(STATUS_LED_PIN);
    homeSpan.setPairingCode("69534191");
    homeSpan.begin(Category::Bridges, "DroneTales Smart Bridge");

    // Build device's serial number.
    char Sn[24];
    snprintf(Sn, 24, "DRONETALES-%llX", ESP.getEfuseMac());

    // Configure bridge.
    new SpanAccessory();
    new DeviceIdentify("DroneTales HomeKit", Sn);
    new Service::HAPProtocolInformation();
    new Characteristic::Version("1.0.0");

    // Add occupancy sensor.
    new SpanAccessory();
    new DeviceIdentify("DroneTales Occupancy Sensor", Sn);
    new OccupancySensor();

    // Add lamp relay.
    new SpanAccessory();
    new DeviceIdentify("DroneTales Lamp Relay", Sn);
    new LampRelay();

    // Add fan relay.
    new SpanAccessory();
    new DeviceIdentify("DroneTales Fan Relay", Sn);
    new FanRelay();
}

// Arduino main loop.
void loop()
{
    homeSpan.poll();
}

/**************************************************************************************/