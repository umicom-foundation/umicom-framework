/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/base64.c
 *
 * PURPOSE:
 *   Implement strict bounded base64 decoding for debugger memory reads.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/base64.h"

#include <ctype.h>
#include <string.h>

/* Provide the value of operation used by this module and its client applications. */
static int value_of(unsigned char value)
{
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (value >= 'A' && value <= 'Z') return value - 'A';
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (value >= 'a' && value <= 'z') return 26 + value - 'a';
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (value >= '0' && value <= '9') return 52 + value - '0';
    /* Apply this branch only when its contract condition is satisfied. */
    if (value == '+') return 62;
    /* Apply this branch only when its contract condition is satisfied. */
    if (value == '/') return 63;
    return -1;
}

/*
 * Read debug runtime base64 into validated module state and return a status when input
 * cannot be used.
 */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_bytes == NULL || out_count == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    cursor = (const unsigned char *)text;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0') {
        int value;

        /* Apply this branch only when its contract condition is satisfied. */
        if (isspace(*cursor)) {
            ++cursor;
            continue;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (*cursor == '=') break;

        value = value_of(*cursor++);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (value < 0) return UMI_STATUS_PARSE_ERROR;

        accumulator = (accumulator << 6) | (unsigned)value;
        bits += 6U;

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (bits >= 8U) {
            bits -= 8U;
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (used >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
            out_bytes[used++] =
                (unsigned char)((accumulator >> bits) & 0xFFU);
        }
    }

    *out_count = used;
    return UMI_STATUS_OK;
}
