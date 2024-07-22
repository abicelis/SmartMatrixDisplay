# OC Transpo Bus Schedule and Weather Matrix Display
This repository contains the code that runs an ESP32-based 128x64 Smart Matrix Display on a Mini D1 board to create a real-time display for bus schedule information and weather updates. The display is an LED matrix screen like the onex on Aliexpress

# Features
- Displays bus arrival times on configurable bus stops.
- Fetches weather data (temperature, conditions, etc.) from an online API.
- Automatic brightness control based on ambient light.

# Hardware Components
- ESP32 Mini D1 board
- LED Full-Color P2.5 Module SMD2121 128x64 Pixels Display
- https://github.com/hallard/WeMos-Matrix-Shield-DMA

# Running the code
 - Clone this repository to your local machine.
- Install VS Code and the Platformio plugin.
- Open the project.
- Create a `Config.h` file (Copy `Config.h.sample`)

Feel free to improve and adapt this project for your specific needs!

# Inspiration
- https://github.com/sschueller/vbz-fahrgastinformation/
- https://github.com/scahp/ArduinoSmartClockWithWeather-

# APIS
- https://api.octranspo1.com/ for obtaining Bus Schedule data.
- https://open-meteo.com/en/docs for obtaining Weather data.
- https://developer.accuweather.com for obtaining Weather data.

# Libraries and other tools
- Fantastic https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-DMA for hooking up the LED Matrix display.
- ArduinoJson https://arduinojson.org/
- For converting images into C-style arrays https://javl.github.io/image2cpp/
- Amazing image editor https://www.photopea.com/
- Converting Android XML Vectors to SVGs https://shapeshifter.design/
- Tool for disassembling APKs https://apktool.org/

# Useful PlatformIO shortcuts
    - Build: Ctrl + Alt + B
    - Shortcuts: Shift + cmd + P