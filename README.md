[esp32_iot_telemetry_node_readme.md](https://github.com/user-attachments/files/34867059/esp32_iot_telemetry_node_readme.md)
# ESP32 Dual-Core IoT Telemetry Node

An asynchronous IoT sensor telemetry firmware built for the **ESP32** using the Arduino framework and **FreeRTOS**. This project decouples high-frequency analog sensor polling from blocking HTTP network requests by leveraging the ESP32's dual-core architecture.

---

## 🚀 Features

* **Dual-Core FreeRTOS Architecture:** 
  * **Core 1 (Application Core):** Dedicated to continuous, non-blocking 12-bit ADC sensor readings every `100ms`.
  * **Core 0 (Protocol Core):** Runs a pinned FreeRTOS task (`WiFi_Task`) that manages Wi-Fi connectivity and transmits telemetry data via HTTP GET requests every `3000ms` using deterministic `vTaskDelayUntil` scheduling.
* **Thread-Safe Data Sharing:** Uses a `volatile uint16_t` global variable to safely pass raw ADC values between cores without compiler optimization issues.
* **Visual Status Feedback:** Uses an onboard or external LED (`GPIO 18`) to visually indicate active HTTP transmission windows.
* **Wokwi Simulator Ready:** Pre-configured with `Wokwi-GUEST` credentials for immediate testing in virtual environments, while easily adaptable to physical hardware.

---

## 🛠️ Hardware & Pin Configuration

| Component | ESP32 Pin | Mode / Configuration | Description |
| :--- | :--- | :--- | :--- |
| **Analog Sensor** (e.g., Potentiometer, TMP36, LDR) | `GPIO 34` | `INPUT` (12-bit ADC, `ADC_11db`) | Reads analog voltage ($0\text{V} - 3.3\text{V}$ mapped to $0 - 4095$). |
| **Status LED** | `GPIO 18` | `OUTPUT` | Turns `HIGH` during active HTTP GET requests, `LOW` otherwise. |

> **Note:** `GPIO 34` is an input-only pin on the ESP32 (`ADC1_CH6`), making it ideal for analog sensing as it does not interfere with Wi-Fi operations (unlike `ADC2` pins).

---

## 📋 Prerequisites

* **Hardware:** ESP32 Development Board (e.g., ESP32-DevKitC) or [Wokwi ESP32 Simulator](https://wokwi.com/).
* **IDE:** Arduino IDE (v1.8.x or v2.x) or PlatformIO.
* **Core Libraries:**
  * `WiFi.h` (Included in ESP32 Arduino Core)
  * `HTTPClient.h` (Included in ESP32 Arduino Core)

---

## ⚙️ Configuration & Setup

1. **Clone or Copy the Code:**
   Open the sketch in your preferred IDE.

2. **Configure Wi-Fi Credentials:**
   Update the `ssid` and `password` variables at the top of the file if running on physical hardware:
   ```cpp
   const char* ssid = "YOUR_WIFI_SSID";
   const char* password = "YOUR_WIFI_PASSWORD";
   ```

3. **Configure Endpoint URL (Optional):**
   By default, telemetry is sent to `httpbin.org` for testing:
   ```cpp
   String url = "http://httpbin.org/get?sensor=" + String(value);
   ```
   Replace this with your own REST API endpoint, webhook, or IoT cloud platform URL.

4. **Upload & Monitor:**
   * Flash the firmware to your ESP32.
   * Open the Serial Monitor at **`115200` baud** to view real-time sensor readings and HTTP response codes.

---

## 📊 System Architecture

```text
+-----------------------------------+       +-----------------------------------+
|         CORE 1 (Arduino)          |       |         CORE 0 (FreeRTOS)         |
|                                   |       |                                   |
|  loop() [Every 100ms]             |       |  wifiTask() [Every 3000ms]        |
|   ├── Read GPIO 34 (12-bit ADC)   |       |   ├── Read volatile sensorValue   |
|   ├── Update volatile sensorValue | ----> |   ├── Set LED_PIN HIGH            |
|   └── Print value to Serial       |       |   ├── Send HTTP GET Request       |
|                                   |       |   ├── Print HTTP Status Code      |
|                                   |       |   └── Set LED_PIN LOW             |
+-----------------------------------+       +-----------------------------------+
```

---

## 🖥️ Sample Serial Output

```text
ESP32 IoT Telemetry Node Started!
Connecting to WiFi...
Sensor Value: 1842
Sensor Value: 1845
WiFi Connected!
IP Address: 10.13.37.2
Sensor Value: 1850
HTTP Response: 200
Data transmitted successfully!
Sensor Value: 1848
```

---

## 📄 License

This project is open-source and available under the [MIT License](https://opensource.org/licenses/MIT).
