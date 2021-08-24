#include <stdio.h>
#include <stdint.h>
#include "../include/canbus-binding/utils/frame-codec.hpp"

uint8_t data[8];

void raz()
{
	for(unsigned k = 0 ; k < sizeof data ; k++)
		data[k] = 0;
}

void show()
{
	for(unsigned i = 0 ; i < sizeof data ; i++) {
		printf(" ");
		for (unsigned j = 0 ; j < 8 ; j++)
			printf("%c", '0'+((data[i]>>(7-j))&1));
	}
}

void p(uint32_t bit_position, uint32_t bit_size, int bigendian)
{
	unsigned i, j;
	uint64_t x, value = (uint64_t)(int64_t)-1;
	raz();
	frame_codec::encode(value, data, bit_position, bit_size, bigendian);
	printf("test for %2d %2d %c", bit_position, bit_size, bigendian?'B':'L');
	show();
	printf("\n");
	for(i = j = 0 ; i < bit_size ; i++) {
		raz();
		value = 1;
		value <<= i;
		frame_codec::encode(value, data, bit_position, bit_size, bigendian);
		x = frame_codec::decode(data, bit_position, bit_size, bigendian, 0);
		j += (x == value);
		printf("%16llx", (long long unsigned)value);
		show();
		printf("\n");
	}
	x = frame_codec::decode(data, bit_position, bit_size, bigendian, 1);
	printf("pos %16llx  neg %16llx\n", (long long unsigned)value, (long long unsigned)x);
	printf("= %d/%d\n\n", j, bit_size);
}

void q(uint32_t bit_position, uint32_t bit_size, int bigendian)
{
	uint32_t lp;
	if (!bigendian)
		lp = bit_position + bit_size;
	else {
		lp = (bit_position - (bit_position & 7)) + (7 - (bit_position & 7));
		lp += bit_size - 1;
		lp = (lp - (lp & 7)) + (7 - (lp & 7)) + 1;
	}
	if (lp <= 64)
		p(bit_position, bit_size, bigendian);
}

int main(int ac, char **av)
{
	int f = 0;
	uint32_t bp, bs;
	uint32_t bp0 = 0, bs0 = 1;
	uint32_t mbp = 64, mbs = 64;
	for (bs = bs0 ; bs <= mbs ; bs++)
		for (bp = bp0 ; bp < mbp ; bp++)
			q(bp, bs, f);
	for (bs = bs0 ; bs <= mbs ; bs++)
		for (bp = bp0 ; bp < mbp ; bp++)
			q(bp, bs, !f);
	return 0;
}


