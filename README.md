# Creating Icons. Black & white bitmaps to C arrays.
Find the original image bitmaps and PSDs in ./icons. These monochrome bitmaps translate to single bits where one bit = one pixel (1=LED ON, 0=LED OFF).

They are arrangeg into an array of 'char' (8bit). To create one:

    - Go to https://javl.github.io/image2cpp/
    - Upload image(s)
    - 'Canvas size' should populate automatically (16x16 pixel)
    - Check 'Invert Image Color'
    - Verify Preview(s)
    - Set 'Code output format' to 'Arduino code'
    - Set 'Draw Mode' to 'Horizontal - 1 bit bytes per pixel"
    - Copy code to project. Draw images using drawBitmap().


# Useful PlatformIO shortcuts
    - Build: Ctrl + Alt + B
    - Shortcuts: Shift + cmd + P


# Inspiration
- https://github.com/sschueller/vbz-fahrgastinformation/
- https://github.com/scahp/ArduinoSmartClockWithWeather-


# APIS
- https://api.octranspo1.com/ for obtaining Bus Schedule data.
- https://open-meteo.com/en/docs for obtaining Weather data.


# Libraries and other tools
- Fantastic https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-DMA for hooking up the LED Matrix display.
- ArduinoJson https://arduinojson.org/
- For converting images into C-style arrays https://javl.github.io/image2cpp/
- Amazing image editor https://www.photopea.com/