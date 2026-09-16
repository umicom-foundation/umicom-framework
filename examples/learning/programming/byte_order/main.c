/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/byte_order/main.c
 *
 * PURPOSE:
 *   Bytes, endianness and explicit encoding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static void EncodeBigEndian(uint32_t value, unsigned char out[4])
{
    out[0] = (unsigned char)(value >> 24U);
    out[1] = (unsigned char)(value >> 16U);
    out[2] = (unsigned char)(value >> 8U);
    out[3] = (unsigned char)value;
}
static uint32_t DecodeBigEndian(const unsigned char in[4])
{
    return ((uint32_t)in[0] << 24U) | ((uint32_t)in[1] << 16U) |
           ((uint32_t)in[2] << 8U) | (uint32_t)in[3];
}
int main(void)
{
    /* These private helpers require non-null four-byte arrays. */
    unsigned char bytes[4];
    EncodeBigEndian(UINT32_C(0x12345678),bytes);
    if (bytes[0]!=0x12U || bytes[1]!=0x34U || bytes[2]!=0x56U || bytes[3]!=0x78U ||
        DecodeBigEndian(bytes)!=UINT32_C(0x12345678)) return EXIT_FAILURE;
    EncodeBigEndian(UINT32_MAX,bytes);
    if (DecodeBigEndian(bytes)!=UINT32_MAX) return EXIT_FAILURE;
    EncodeBigEndian(0U,bytes);
    if (DecodeBigEndian(bytes)!=0U) return EXIT_FAILURE;
    puts("0x12345678 -> 12 34 56 78 in big-endian order");
    puts("PASS: byte_order");
    return EXIT_SUCCESS;
}
