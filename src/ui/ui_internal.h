/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/ui_internal.h
 *
 * PURPOSE:
 *   Share private string-copy and revision helpers between toolkit-neutral UI
 *   model implementations without exposing implementation details publicly.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_INTERNAL_H
#define UMICOM_UI_INTERNAL_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/**
 * Provide the ui copy text operation used by this module and its client applications.
 */
static inline int umi_ui_copy_text(char *destination, size_t capacity, const char *source)
{
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return 0;
    written = snprintf(destination, capacity, "%s", source != NULL ? source : "");
    return written >= 0 && (size_t)written < capacity;
}

/**
 * Provide the ui next revision operation used by this module and its client applications.
 */
static inline uint64_t umi_ui_next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

/* Validate complete UTF-8 scalar values while locating the terminator. This
 * stops at NUL; it never treats a short caller string as a full-size array. */
static inline int umi_ui_text_is_valid_utf8(const char *text, size_t capacity)
{
    size_t i = 0U;
    if (text == NULL) return 0;
    while (i < capacity) {
        const unsigned char c = (unsigned char)text[i++];
        unsigned need;
        uint32_t value, minimum;
        if (c == 0U) return 1;
        if (c < 0x80U) continue;
        if (c >= 0xc2U && c <= 0xdfU) { need = 1U; value = c & 0x1fU; minimum = 0x80U; }
        else if (c >= 0xe0U && c <= 0xefU) { need = 2U; value = c & 0x0fU; minimum = 0x800U; }
        else if (c >= 0xf0U && c <= 0xf4U) { need = 3U; value = c & 0x07U; minimum = 0x10000U; }
        else return 0;
        while (need-- != 0U) {
            unsigned char next;
            if (i >= capacity) return 0;
            next = (unsigned char)text[i++];
            if ((next & 0xc0U) != 0x80U) return 0;
            value = (value << 6U) | (uint32_t)(next & 0x3fU);
        }
        if (value < minimum || value > 0x10ffffU ||
            (value >= 0xd800U && value <= 0xdfffU)) return 0;
    }
    return 0;
}


#endif
