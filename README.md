# Creating Icons. Black & white bitmaps to C arrays.
Find the original image bitmaps and PSDs in ./icons. These monochrome bitmaps translate to single bits where one bit = one pixel (1=LED ON, 0=LED OFF).

They are arrangeg into an array of 'char' (8bit). To create one:

    - Go to https://javl.github.io/image2cpp/
    - Upload image(s)
    - 'Canvas size' should populate automatically (16x16 pixel)
    - Check 'Invert Image Color'
    - Verify Preview(s)
    - Set 'Code output format' to 'Arduino code'
    - Set 'Draw Mode' to:
        - 'Horizontal - 1 bit per pixel', for "Black and White" images
            
        - 'Horizontal - 2 bytes per pixel (565)', for full color images.
    - Copy code to project. Draw images using drawBitmap() or drawRGBBitmap()

## Horizontal - 1 bit per pixel

These C-style arrays are 1 bit = 1 pixel binary 1 means "draw color" 0 means "draw nothing".
On the drawBitmap() call, one can specify the color that is to be drawn when bits are set to 1. One also specifies the width and height of the final image, size isn't encoded into the array, just 'color'.

### Array example

```
const unsigned char icon [] PROGMEM = {
	0x00, 0x03, ...

```

### Explanation of array values 

Values are represented in hex: 

 - So 0x00 is 0000 0000 in binary (All those pixels off)
 - 0x03 is 0000 0011 in binary (Only the last two pixels on!)


## Horizontal - 2 bytes per pixel (565)

These C-style arrays contain 16-bit numbers, which correspond to a pixel color in a 16-bit color space, often referred to as RGB565. In RGB565, the color is represented as a 16-bit number where there are:
- 5 bits for R
- 6 bits for G
- 5 bits for B

Which means:

- The most significant 5 bits represent the red component R.
- The next 6 bits represent the green component G.
- The least significant 5 bits represent the blue component B.

On the drawRGBBitmap() call, one specifies the width and height of the final image, size isn't encoded into the array, just 'color'.

### Array example

```
const uint16_t icon [] PROGMEM = {
	0xF81F, 0xFFFF, ...
```

### Explanation of array values 

For example, 0xF81F in binary is 1111100000011111, when split into RGB components:

R: 11111 (in binary) or 31 (in decimal)
G: 000000 (in binary) or 63 (in decimal)
B: 11111 (in binary) or 31 (in decimal)

Which would be purple (red + blue)




# Useful PlatformIO shortcuts
    - Build: Ctrl + Alt + B
    - Shortcuts: Shift + cmd + P


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