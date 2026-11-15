
#include <WiFi.h>
#include <HTTPClient.h>

// Wi-Fi credentials
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Pin configuration
#define SENSOR_PIN 34
#define LED_PIN 18

// Shared sensor value
volatile uint16_t sensorValue = 0;

// Wi-Fi task
void wifiTask(void *parameter) {

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.println("Connecting to WiFi...");

  while (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(pdMS_TO_TICKS(500));
  }

  Serial.println("WiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  TickType_t lastWakeTime = xTaskGetTickCount();

  while (true) {

    if (WiFi.status() == WL_CONNECTED) {

      uint16_t value = sensorValue;

      String url =
        "http://httpbin.org/get?sensor=" + String(value);

      HTTPClient http;

      http.setTimeout(2000);

      digitalWrite(LED_PIN, HIGH);

      if (http.begin(url)) {

        int httpCode = http.GET();

        Serial.print("HTTP Response: ");
        Serial.println(httpCode);

        if (httpCode > 0) {
          Serial.println("Data transmitted successfully!");
        }

        http.end();
      }

      digitalWrite(LED_PIN, LOW);
    }

    // Schedule the next transmission
    vTaskDelayUntil(
      &lastWakeTime,
      pdMS_TO_TICKS(3000)
    );
  }
}

void setup() {

  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Configure ADC
  analogReadResolution(12);
  analogSetPinAttenuation(SENSOR_PIN, ADC_11db);

  Serial.println("ESP32 IoT Telemetry Node Started!");

  // Start networking on a separate FreeRTOS task
  xTaskCreatePinnedToCore(
    wifiTask,
    "WiFi_Task",
    8192,
    NULL,
    1,
    NULL,
    0
  );
}

void loop() {

  // Continuously read analog sensor
  sensorValue = analogRead(SENSOR_PIN);

  // Print raw 12-bit ADC value
  Serial.print("Sensor Value: ");
  Serial.println(sensorValue);

  delay(100);
}
