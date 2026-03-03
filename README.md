# ESP32-S3 WiFi RSSI Motion Tracker 📡🏃‍♂️

A "sensor-less" motion detection system that uses WiFi signal fluctuations (RSSI) to detect human presence and movement. This project features a real-time web dashboard with live graphing and physical NeoPixel feedback.



## 🚀 How It Works
Physical objects and human bodies absorb and reflect 2.4GHz WiFi signals. By monitoring the **Received Signal Strength Indicator (RSSI)** at high frequency, this firmware detects the "noise" created by movement. 
* **Low Variance:** Static environment (Idle).
* **High Variance:** Movement detected in the signal path.

## ✨ Features
* **Real-Time Web Dashboard:** Hosted directly on the ESP32-S3.
* **Live Graphing:** Visualizes motion intensity using `Chart.js`.
* **Visual Alerts:** Onboard NeoPixel changes color based on activity levels.
* **Audio Triggers:** Browser-side "beep" alert for high-motion events.
* **No Extra Hardware:** Works entirely via the built-in WiFi antenna.

## 🛠️ Hardware Requirements
* **Microcontroller:** ESP32-S3 (specifically configured for GPIO 48 NeoPixel, common on DevKits).
* **WiFi Connection:** A stable connection to a local Access Point.

## 📦 Installation & Setup
1. **Prerequisites:** * Install [Arduino IDE](https://www.arduino.cc/en/software).
   * Install the `Adafruit_NeoPixel` library via the Library Manager.
2. **Configuration:** * Open the `.ino` file.
   * Update the `ssid` and `password` variables with your network credentials.
3. **Flash:** * Select your ESP32-S3 board and the correct COM port.
   * Upload the code.
4. **Access:** * Open the Serial Monitor (115200 baud) to find the IP Address.
   * Navigate to that IP in your web browser.

## 📊 Data Visualization
The web interface provides:
* **Status Indicator:** Displays "Idle", "Movement Detected", or "HIGH MOTION!".
* **Intensity Meter:** A visual scale of the current signal disturbance.
* **Live Line Chart:** A 40-point history of motion intensity.

## ⚠️ Limitations
* **Range:** Accuracy depends on the distance between the ESP32 and the WiFi router.
* **False Positives:** Other electronic interference or network congestion may occasionally trigger the sensor.

## 📜 License
MIT License. Feel free to use and modify for your own lab experiments!
