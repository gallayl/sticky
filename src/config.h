#define HTTP_PORT 80

#define WEBSOCKETS_URL "/ws"

#define STA_SSID "sticky"
#define STA_PASSPHARSE "sticky1234"

#define STA_IP "192.168.0.1"
#define STA_GATEWAY "192.168.0.1"
#define STA_NETMASK "255.255.255.0"

#define JSON_BUFFER_SIZE 2048

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
