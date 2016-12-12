#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <linux/input.h>


/* this macro is used to tell if "bit" is set in "array"
 * it selects a byte from the array, and does a boolean AND
 * operation with a byte that only has the relevant bit set.
 * eg. to check for the 12th bit, we do (array[1] & 1<<4)
 */
#define test_bit(bit, array) (array[bit/8] & (1<<(bit%8)))


void is_touch_screen(unsigned char *abs_bitmask, unsigned char *key_bitmask){
	// See if this is a touch pad.
	// Is this a new modern multi-touch driver?
	if (test_bit(ABS_MT_POSITION_X, abs_bitmask)
			&& test_bit(ABS_MT_POSITION_Y, abs_bitmask)) {
		// Some joysticks such as the PS3 controller report axes that conflict
		// with the ABS_MT range.  Try to confirm that the device really is
		// a touch screen.
		if (test_bit(BTN_TOUCH, key_bitmask)) {
			printf("INPUT_DEVICE_CLASS_TOUCH | INPUT_DEVICE_CLASS_TOUCH_MT\n");
		}
		// Is this an old style single-touch driver?
	} else if (test_bit(BTN_TOUCH, key_bitmask)
			&& test_bit(ABS_X, abs_bitmask)
			&& test_bit(ABS_Y, abs_bitmask)) {
		printf("INPUT_DEVICE_CLASS_TOUCH\n");
	}
}



int main(int argc, char** argv) {
	int fd;
	int i = 0;
	int j = 0;
	char filename[64];

	unsigned char absBitmask[(ABS_MAX + 1) / 8];
	unsigned char keyBitmask[(KEY_MAX + 1) / 8];

	for (i=0; i<32; i++){
		snprintf(filename,sizeof(filename), "/dev/input/event%d", i);
		fd = open(filename, O_RDWR);
		if (fd > 0){
			ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(absBitmask)), absBitmask);
			ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keyBitmask)), keyBitmask);
			
			printf("%s (absBitmask):\n", filename);
			for (j=0;j<(ABS_MAX + 1) / 8;j++){
				printf("0x%x ", absBitmask[j]);
			}
			printf("\n");
			
			printf("%s (keyBitmask):\n", filename);
			for (j=0;j<(KEY_MAX + 1) / 8;j++){
				printf("0x%x ", keyBitmask[j]);
			}
			printf("\n");
			close(fd);
			
			// Is touchscreen ??
			is_touch_screen(absBitmask, keyBitmask);
		}
	}

	return 0;
}
