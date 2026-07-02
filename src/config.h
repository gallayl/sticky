#define HTTP_PORT 80

#define WEBSOCKETS_URL "/ws"

#define AP_SSID "sticky"
#define AP_PASSPHRASE "sticky1234"

/**
 * Enables the LittleFS File System feature
 */
#define ENABLE_LITTLEFS true

/**
 * Enable to read input from a serial console
 */
#define ENABLE_SERIAL_READ true

/**
 * Enable the OTA firmware upgrade feature
 */
#define ENABLE_OTA true

/**
 * ESP-8266 specific features
 */
#ifdef ESP8266

/**
 * Enable the PIR Sensor module. Doesn't work with ESP32 due to conflicting D6 sensor type
 */
#define ENABLE_PIR_SENSOR true

/**
 * Enable generic I2C scan functionality
 */
#define ENABLE_I2C true

#ifdef ENABLE_I2C

#define ENABLE_AHT25 true

#endif

#endif
