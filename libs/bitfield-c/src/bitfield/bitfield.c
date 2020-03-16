#include <bitfield/bitfield.h>
#include <limits.h>
#include <string.h>
#include <stddef.h>
#include <sys/param.h>

uint64_t bitmask ( const uint8_t bit_count )
{
    if ( bit_count >= 64 )
    {
        return 0xFFFFFFFFFFFFFFFF;
    }
    else
    {
        return ( ( ( uint64_t ) 0x1 ) << bit_count ) - 1;
    }
}

uint8_t get_nibble(const uint8_t source[], const uint32_t source_length,
                const uint32_t nibble_index) {
    uint32_t byte_index = nibble_index / 2;
    uint8_t result = get_byte(source, source_length, byte_index);
    if(nibble_index % 2 == 0) {
        result >>= NIBBLE_SIZE;
    }
    result &= (uint8_t)bitmask(NIBBLE_SIZE);
    return result;
}

uint8_t get_byte(const uint8_t source[], const uint32_t source_length,
        const uint32_t byte_index) {
    if(byte_index < source_length) {
        return source[byte_index];
    }
    return 0;
}

uint64_t get_bitfield(const uint8_t source[], const uint32_t source_length,
                const uint32_t offset, const uint8_t bit_count) {
    if(bit_count > 64 || bit_count < 1) {
        // TODO error reporting?
        return 0;
    }

    ArrayOrBytes combined;
    memset(combined.bytes, 0, sizeof(combined.bytes));
    if(copy_bits_right_aligned(source, source_length, offset, bit_count,
            combined.bytes, sizeof(combined.bytes))) {
        if(BYTE_ORDER == LITTLE_ENDIAN) {
            combined.whole = __builtin_bswap64(combined.whole);
        }
    } else {
        // debug("couldn't copy enough bits from source")
    }
    return combined.whole;
}

bool set_nibble(const uint32_t nibble_index, const uint8_t value,
        uint8_t* destination, const uint32_t destination_length) {
    return copy_bits((uint8_t*)&value, CHAR_BIT, NIBBLE_SIZE, NIBBLE_SIZE, destination,
            destination_length, nibble_index * NIBBLE_SIZE);
}

bool set_bitfield(const uint64_t value, const uint8_t offset,
        const uint8_t bit_count, uint8_t destination[],
        uint32_t destination_length) {
    if(value > bitmask(bit_count)) {
        return false;
    }

    ArrayOrBytes combined = {
        whole: value
    };

    if(BYTE_ORDER == LITTLE_ENDIAN) {
        combined.whole = __builtin_bswap64(combined.whole);
    }

    return copy_bits(combined.bytes, sizeof(combined.bytes),
            (uint32_t)(sizeof(combined.bytes) * CHAR_BIT - bit_count),
			bit_count,
            destination, destination_length, offset);
}
