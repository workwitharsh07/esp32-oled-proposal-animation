# 💍 ESP32 OLED Proposal Animation

Display a cinematic **marriage proposal animation** on a **128×64 SSD1306 OLED** using an ESP32. This project demonstrates I²C communication, bitmap rendering, smooth animations, and text effects with the ESP32 framework.

---

## ✨ Features

- 💍 Cinematic marriage proposal animation
- ✨ Smooth bitmap and text animations
- 📺 Automatic scene transitions
- ⚡ Lightweight and optimized for ESP32
- 🛠️ Easy to customize messages and graphics
- 📚 Beginner-friendly and well-structured code

---

## 🛠️ Hardware Required

| Component | Quantity |
|-----------|:--------:|
| ESP32 Development Board | 1 |
| SSD1306 OLED Display (128×64) | 1 |
| Breadboard | 1 |
| Jumper Wires | As Required |
| USB Cable | 1 |

---

## 🔌 Wiring Diagram

| OLED Pin | ESP32 Pin |
|-----------|-----------|
| VCC | 3.3V |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

### Circuit Diagram

<p align="center">
  <img src="circuit.webp" alt="ESP32 OLED Wiring Diagram" width="700">
</p>

---

## 💻 Software Requirements

Install the following libraries using the **Arduino IDE Library Manager**:

- Adafruit GFX
- Adafruit SSD1306

---

## 🚀 Getting Started

1. Download or clone this repository.
2. Extract the ZIP file (if downloaded).
3. Open the `love-proposal.ino` file from the project folder.
4. Arduino IDE will automatically load all required project files.
5. Install the required libraries.
6. Select your ESP32 board and COM port.
7. Upload the sketch.

---

## 💻 Source Code

The complete Arduino sketch is available in **love-proposal.ino**.

---

## 📸 Expected Result

After uploading the code:

- 🌌 A galaxy particle animation appears.
- ❤️ A heart formation animation is displayed.
- 💍 A ring box opens with a sparkling diamond.
- ⌨️ "Will You Marry Me?" is revealed with a typewriter effect.
- 🔄 The complete animation repeats continuously.

---

## ⚙️ Customization

You can personalize the project by modifying:

- Proposal message
- Animation timing
- Bitmap graphics
- Transition effects
- Display speed

---

## ⚠️ Troubleshooting

| Problem | Solution |
|---------|----------|
| OLED remains blank | Check the wiring and I²C connections |
| Upload failed | Verify the ESP32 board and COM port |
| Graphics not visible | Confirm the required libraries are installed |
| Display not detected | Check the OLED I²C address and power connections |

---

## 📚 Technologies Used

- ESP32
- Arduino IDE
- C++
- SSD1306 OLED Display
- I²C Communication
- Adafruit GFX
- Adafruit SSD1306

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
