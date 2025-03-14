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

// RX pin. Connect to LD2420 TX pin.
// OT1 for new version. OT2 for old version.
#define RX_PIN              GPIO_NUM_20
// TX pin. Connect to LD2420 RX pin.
#define TX_PIN              GPIO_NUM_21

// The communication baud rate. Change to 256000 for old version.
#define BAUD_RATE           115200

void setup()
{
    Serial.begin(BAUD_RATE);
    Serial1.begin(BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);
}

void loop()
{
    // Pass all data received from radar to PC
    while (Serial1.available())
        Serial.write(Serial1.read());
    // Pass all data received from PC to radar.
    while (Serial.available())
        Serial1.write(Serial.read());
}
