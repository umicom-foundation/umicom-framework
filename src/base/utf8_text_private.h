/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/base/utf8_text_private.h
 *
 * PURPOSE:
 *   Validate complete UTF-8 byte spans independently of report-format control policies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UTF8_TEXT_PRIVATE_H
#define UMICOM_UTF8_TEXT_PRIVATE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/utf8.h"

/* This validates Unicode scalar encoding, not a file format. ASCII controls
 * are valid bytes here; CSV and JSON apply their own rejection/escaping rules. */
/* The public base service now supplies offsets and single-scalar decoding as
 * well as span validation. CSV, structured reports and learning text retain
 * this private entry point so all consumers use one active Unicode policy.
 * Keep the previous validator for engineering comparison. */
#if 0
static inline int UmiUtf8TextValid(const char *text, size_t length)
{
    if (text == NULL && length != 0U) return 0;
    for (size_t i = 0U; i < length;) {
        unsigned char c = (unsigned char)text[i++];
        if (c < 0x80U) continue;
        unsigned following;
        uint32_t value, minimum;
        if (c >= 0xc2U && c <= 0xdfU) { following = 1U; value = c & 0x1fU; minimum = 0x80U; }
        else if (c >= 0xe0U && c <= 0xefU) { following = 2U; value = c & 0x0fU; minimum = 0x800U; }
        else if (c >= 0xf0U && c <= 0xf4U) { following = 3U; value = c & 0x07U; minimum = 0x10000U; }
        else return 0;
        if (length - i < following) return 0;
        for (unsigned j = 0U; j < following; ++j) {
            c = (unsigned char)text[i++];
            if ((c & 0xc0U) != 0x80U) return 0;
            value = (value << 6U) | (c & 0x3fU);
        }
        if (value < minimum || value > 0x10ffffU || (value >= 0xd800U && value <= 0xdfffU)) return 0;
    }
    return 1;
}
#endif
static inline int UmiUtf8TextValid(const char *text, size_t length)
{
    return UmiUtf8Validate((const unsigned char *)text, length, NULL);
}

#endif
