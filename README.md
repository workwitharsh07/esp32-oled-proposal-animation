# 🔋 Smart Battery Dashboard V1

A smart battery monitoring system built with an ESP32 to track **real-time battery voltage, charging status, and battery percentage** through a clean web-based dashboard.

---

## ✨ Features

- 🔋 Real-time battery voltage monitoring
- 📊 Live battery percentage calculation
- ⚡ Charging status detection
- 🔌 Battery connection detection
- 🔴 Low battery indication
- ⚡ Animated charging indicator
- 🌊 Animated battery level visualization
- 📱 Modern web-based dashboard
- 📶 Wireless monitoring over Wi-Fi
- 🔄 Automatic live data updates
- ⚡ Lightweight and optimized for ESP32
- 📚 Beginner-friendly and easy to customize

---

## 🛠️ Hardware Required

| Component | Quantity |
|-----------|:--------:|
| ESP32 Development Board | 1 |
| 18650 Battery | 1 |
| TP4056 Charging Module | 1 |
| 2kΩ Resistor | 1 |
| 1kΩ Resistor | 3 |
| Breadboard | 1 |
| Jumper Wires | As Required |
| USB Cable | 1 |

---

## 🔌 Wiring Diagram

| Connection | ESP32 Pin |
|------------|-----------|
| Battery Voltage Monitoring | GPIO 34 |
| Charging Status Monitoring | GPIO 35 |
| Ground | GND |

### Circuit Diagram

![Circuit Diagram](circuit.webp)

---

## 💻 Software Requirements

Install the following using the **Arduino IDE**:

- ESP32 Board Package

The project uses:

- WiFi
- WebServer

---

## 🚀 Getting Started

1. Download or clone this repository.
2. Extract the ZIP file if downloaded.
3. Open the main `.ino` file from the project folder.
4. Enter your Wi-Fi name and password in the Wi-Fi configuration section.
5. Connect the battery monitoring circuit according to the wiring diagram.
6. Select your ESP32 board and COM Port.
7. Upload the sketch.
8. Open the Serial Monitor at **115200 baud**.
9. Wait for the ESP32 to connect to Wi-Fi.
10. Copy the IP address displayed in the Serial Monitor.
11. Open the IP address in a web browser.
12. Start monitoring your battery in real time.

---

## 💻 Source Code

The complete Arduino sketch is available in the main `.ino` file.

The ESP32 continuously monitors the battery voltage and charging input, calculates the battery percentage, detects the current battery status, and updates the web dashboard with live data.

---

## 📊 Expected Result

After uploading the code:

- 🔋 The live battery percentage is displayed.
- 📈 The current battery voltage is monitored.
- ⚡ The dashboard detects when the battery is charging.
- 🔴 A low battery state is shown when the battery level is low.
- 🚫 The system detects when no battery is connected.
- 🌊 The battery level updates with animated effects.
- 📱 The dashboard can be accessed from a phone or computer connected to the same Wi-Fi network.

---

## ⚙️ Customization

You can customize the project by modifying:

- Wi-Fi credentials
- Battery voltage calculation
- Battery percentage calculation
- Battery detection threshold
- Charging detection threshold
- Voltage smoothing
- Percentage smoothing
- Dashboard design
- Update intervals
- Battery animation effects

---

## 📶 Wi-Fi Setup

The ESP32 **does not create its own hotspot**.

It connects to an existing Wi-Fi network, such as:

- 📱 Phone hotspot
- 🏠 Home Wi-Fi
- 💻 Laptop/PC hotspot

After connecting, the ESP32 displays its local IP address in the Serial Monitor. Open that IP address from a device connected to the **same network**.

---

## ⚠️ Troubleshooting

| Problem | Solution |
|---------|----------|
| Dashboard doesn't open | Make sure both devices are connected to the same Wi-Fi network |
| ESP32 doesn't connect | Verify the Wi-Fi name and password |
| Battery voltage is incorrect | Check the voltage divider wiring and resistor values |
| Battery always shows 0% | Check the battery connection and GPIO 34 wiring |
| Charging status is incorrect | Check the TP4056 connection and GPIO 35 wiring |
| Battery shows no connection | Check the battery wiring and detection threshold |
| IP address not shown | Open the Serial Monitor at 115200 baud |
| Upload failed | Verify the ESP32 board and COM Port |

---

## 📚 Technologies Used

- ESP32
- Arduino IDE
- C++
- Wi-Fi
- WebServer
- HTML
- CSS
- JavaScript
- ESP32 ADC
- Battery Voltage Monitoring

---

## ⭐ Support

If you found this project helpful, consider giving this repository a **⭐ Star**.

Your support helps **LifeTronix** create more open-source Arduino, ESP32, IoT, and Robotics projects.

---

## 📄 License

This project is licensed under the **MIT License**.

---

<p align="center">
Made with ❤️ by <strong>LifeTronix</strong>
</p>
