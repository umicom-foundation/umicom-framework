/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/base/utf8.c
 *
 * PURPOSE:
 *   Own the strict Unicode scalar decoder shared by document conversion and evidence writers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/base/utf8.h"

/* Reuse the document decoder at the lowest common dependency boundary. This
 * preserves its scalar and failure contracts for every existing document user. */
int UmiUtf8DecodeScalar(const unsigned char *bytes,
                              size_t count,
                              size_t *out_used,
                              uint32_t *out_scalar)
{
    unsigned char first;
    uint32_t scalar;
    size_t used;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bytes == NULL || count == 0U || out_used == NULL || out_scalar == NULL) return 0;
    first = bytes[0];
    /* Apply this branch only when its contract condition is satisfied. */
    if (first < 0x80U) {
        *out_used = 1U;
        *out_scalar = first;
        return 1;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (first >= 0xC2U && first <= 0xDFU) {
        used = 2U;
        scalar = (uint32_t)(first & 0x1FU);
    } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (first >= 0xE0U && first <= 0xEFU) {
        used = 3U;
        scalar = (uint32_t)(first & 0x0FU);
    } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (first >= 0xF0U && first <= 0xF4U) {
        used = 4U;
        scalar = (uint32_t)(first & 0x07U);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        return 0;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (used > count) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 1U; index < used; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if ((bytes[index] & 0xC0U) != 0x80U) return 0;
        scalar = (scalar << 6U) | (uint32_t)(bytes[index] & 0x3FU);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if ((used == 3U && scalar < 0x800U) ||
        (used == 4U && scalar < 0x10000U) ||
        scalar > 0x10FFFFU ||
        (scalar >= 0xD800U && scalar <= 0xDFFFU)) {
        return 0;
    }
    *out_used = used;
    *out_scalar = scalar;
    return 1;
}

int UmiUtf8Validate(const unsigned char *bytes,
                               size_t byte_count,
                               size_t *out_error_offset)
{
    size_t offset = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bytes == NULL && byte_count > 0U) return 0;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (offset < byte_count) {
        size_t used = 0U;
        uint32_t scalar = 0U;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!UmiUtf8DecodeScalar(bytes + offset, byte_count - offset, &used, &scalar)) {
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (out_error_offset != NULL) *out_error_offset = offset;
            return 0;
        }
        offset += used;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_error_offset != NULL) *out_error_offset = byte_count;
    return 1;
}

