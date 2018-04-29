#include <stdio.h>
#include <stdlib.h>
//#include "userled.h"

void userLED(int LED_number, int on_off)
{
	FILE* fd;
	if (LED_number > 3 && LED_number < 0)
	{
		printf("Invalid led number");
		exit(-1);
	}

	if (on_off != 0 && on_off != 1)
	{
		printf("Input should be either 0 or 1");
	}
	switch(LED_number)
	{
		case 1:
			fd = fopen("/sys/devices/platform/leds/leds/beaglebone:green:usr0/brightness", "w");
			fprintf(fd,"%i",on_off);
			break;
		case 2:
			fd = fopen("/sys/devices/platform/leds/leds/beaglebone:green:usr1/brightness", "w");
			fprintf(fd,"%i",on_off);
			break;
		case 3:
			fd = fopen("/sys/devices/platform/leds/leds/beaglebone:green:usr2/brightness", "w");
			fprintf(fd,"%i",on_off);
			break;
	}
	fclose(fd);
}
