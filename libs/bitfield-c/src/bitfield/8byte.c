#include <bitfield/bitfield.h>
#include <bitfield/8byte.h>
#include <stddef.h>
#include <limits.h>
#include <string.h>

#define EIGHTBYTE_BIT (8 * sizeof(uint64_t))

uint8_t eightbyte_get_nibble(const uint64_t source, const uint8_t nibble_index,
        const bool data_is_big_endian) {

    return (uint8_t) eightbyte_get_bitfield(source, (uint8_t) (NIBBLE_SIZE * nibble_index),
            NIBBLE_SIZE, data_is_big_endian);
}

uint8_t eightbyte_get_byte(uint64_t source, const uint8_t byte_index,
        const bool data_is_big_endian) {
    if(data_is_big_endian) {
        source = __builtin_bswap64(source);
    }
    return (source >> (EIGHTBYTE_BIT - ((byte_index + 1) * CHAR_BIT))) & 0xFF;
}

// TODO is this funciton necessary anymore? is it any faster for uint64_t than
// get_bitfield(data[], ...)? is the performance better on a 32 bit platform
// like the PIC32?
uint64_t eightbyte_get_bitfield(uint64_t source, const uint8_t offset,
        const uint8_t bit_count, const bool data_is_big_endian) {
	uint8_t startByte = offset / CHAR_BIT;
	uint8_t endByte = (uint8_t) ((offset + bit_count - 1) / CHAR_BIT);

    if(!data_is_big_endian) {
        source = __builtin_bswap64(source);
    }

    uint8_t* bytes = (uint8_t*)&source;
    uint64_t ret = bytes[startByte];
    if(startByte != endByte) {
        // The lowest byte address contains the most significant bit.
        uint32_t i;
        for(i = startByte + 1; i <= endByte; i++) {
            ret = ret << 8;
            ret = ret | bytes[i];
        }
    }

    ret >>= 8 - find_end_bit(offset + bit_count);
    return ret & bitmask(bit_count);
}

bool eightbyte_set_bitfield(uint64_t value, const uint8_t offset,
        const uint8_t bit_count, uint64_t* destination) {
    if(value > bitmask(bit_count)) {
        return false;
    }

    uint8_t shiftDistance = (uint8_t) (EIGHTBYTE_BIT - offset - bit_count);
    value <<= shiftDistance;
    *destination &= ~(bitmask(bit_count) << shiftDistance);
    *destination |= value;
    return true;
}
