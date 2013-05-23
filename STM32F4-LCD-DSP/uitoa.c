#include <stddef.h>

int uitoa(unsigned int value, char * buf, int max) {
	int n = 0;
	int i = 0;
	unsigned int tmp = 0;

	if (NULL == buf) {
		return -3;
	}

	if (2 > max) {
		return -4;
	}

	i=1;
	tmp = value;
	for (;;) {
		tmp /= 10;
		if (0 >= tmp) {
			break;
		}
		i++;
	}
	if (i >= max) {
		buf[0] = '?';
		buf[1] = 0x0;
		return 2;
	}

	n = i;
	tmp = value;
	buf[i--] = 0x0;
	for (;;) {
		buf[i--] = (tmp % 10) + '0';
		tmp /= 10;
		if (0 >= tmp) {
			break;
		}
	}
	if (-1 != i) {
		buf[i--] = '-';
	}
	return n;
}
