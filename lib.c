#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#ifdef __MINGW32__
#include <fcntl.h>
#include <io.h>
#endif

#include "lib.h"

int main(int argc, char **argv)
{
	const struct __attribute__((__packed__))
	{
		char magic[4];
		uint8_t width;
		uint8_t height;
	} header = {{'L','L','E','2'}, WIDTH, HEIGHT};
	uint32_t timeout;

	// Initialize random seed
	srand(time(NULL));

	// Reopen stdout for binary data
#ifdef __MINGW32__
	// windows
	if(_setmode(_fileno(stdout), _O_BINARY) == -1 )
#else
	if(!freopen(NULL, "wb", stdout))
#endif
	{
		fprintf(stderr, "Stdout reopen failed!\n");
		exit(1);
	}

	// Read cmdline arguments
	if(argc != 2 || sscanf(argv[1], "%u", &timeout) != 1 || timeout == 0)
	{
		fprintf(stderr, "Incorrect parameters!\n");
		exit(1);
	}
	timeout *= 1000; // Convert to ms

	// Write animation header
	if(fwrite(&header, sizeof(header), 1, stdout) != 1)
	{
		fprintf(stderr, "Header write error!\n");
		exit(1);
	}

	// Call main animation loop
	animate(timeout);

	return 0;
}

uint32_t time_acc = 0;
void write_frame(const frame_t *frame)
{
	time_acc += frame->delay;
	if(fwrite(frame, sizeof(frame_t), 1, stdout) != 1)
	{
		fprintf(stderr, "Frame write error!\n");
		exit(1);
	}
	fflush(stdout);
}

uint32_t total_time()
{
	return time_acc;
}

pixel_t hsv2rgb(double h, double s, double v)
{
	pixel_t result;

    double r = 0, g = 0, b = 0;

    double i = floor(h * 6);
    double f = h * 6 - i;
    double p = v * (1 - s);
    double q = v * (1 - f * s);
    double t = v * (1 - (1 - f) * s);

    switch((int)i % 6)
    {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }

	result.r = r * 255;
	result.g = g * 255;
	result.b = b * 255;
	return result;
}

