#ifndef LIB_H
#define LIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// Display dimensions
#define WIDTH	16
#define HEIGHT	6

// Single RGB pixel
typedef struct __attribute__((__packed__))
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} pixel_t;

// Full frame
typedef struct __attribute__((__packed__))
{
	uint16_t delay; // Frame length in milliseconds, min. 50ms
	pixel_t pixel[WIDTH*HEIGHT];
} frame_t;

// Writes single frame to output
void write_frame(const frame_t *frame);
// Gets total animation time (sum of times of all frames generated so far)
uint32_t total_time();

// Prototype of user implemented animation function called from main
// Timeout: time in ms after which function should return (comparing to value from total_time())
extern void animate(const uint32_t timeout);

// Simple color space converter (Hue, Saturation, Value) to RGB
// Arguments range: 0 to 1
pixel_t hsv2rgb(double h, double s, double v);

#ifdef __cplusplus
}
#endif

#endif /* LIB_H */
