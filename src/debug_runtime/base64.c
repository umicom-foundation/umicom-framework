/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/base64.c
 *
 * PURPOSE:
 *   Implement strict bounded base64 decoding for debugger memory reads.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/base64.h"

#include <ctype.h>
#include <string.h>

static int value_of(unsigned char value)
{
    if (value >= 'A' && value <= 'Z') return value - 'A';
    if (value >= 'a' && value <= 'z') return 26 + value - 'a';
    if (value >= '0' && value <= '9') return 52 + value - '0';
    if (value == '+') return 62;
    if (value == '/') return 63;
    return -1;
}

UmiStatus umi_debug_runtime_base64_decode(
    const char *text,
    unsigned char *out_bytes,
    size_t capacity,
    size_t *out_count)
{
    unsigned accumulator = 0U;
    unsigned bits = 0U;
    size_t used = 0U;
    const unsigned char *cursor;

    if (text == NULL || out_bytes == NULL || out_count == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    cursor = (const unsigned char *)text;

    while (*cursor != '\0') {
        int value;

        if (isspace(*cursor)) {
            ++cursor;
            continue;
        }

        if (*cursor == '=') break;

        value = value_of(*cursor++);
        if (value < 0) return UMI_STATUS_PARSE_ERROR;

        accumulator = (accumulator << 6) | (unsigned)value;
        bits += 6U;

        if (bits >= 8U) {
            bits -= 8U;
            if (used >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
            out_bytes[used++] =
                (unsigned char)((accumulator >> bits) & 0xFFU);
        }
    }

    *out_count = used;
    return UMI_STATUS_OK;
}
