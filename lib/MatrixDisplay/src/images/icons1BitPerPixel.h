#include <Arduino.h>

// 1 bit per Pixel
// Editables and source bitmaps: /icons
// Using https://javl.github.io/image2cpp/ t convert to C-style arrays. See ./README for more info.

// 'Bus', 7x9px
const unsigned char icon_Bus [] PROGMEM = {
	0x7c, 0x82, 0xfe, 0x82, 0x82, 0xfe, 0xba, 0xfe, 0x44
};
// 'Sun', 7x9px
const unsigned char icon_Sun [] PROGMEM = {
	0x00, 0x10, 0x44, 0x38, 0xba, 0x38, 0x44, 0x10, 0x00
};