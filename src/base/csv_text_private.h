/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/base/csv_text_private.h
 * PURPOSE: Keep CSV import and export on the same bounded UTF-8 text policy.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CSV_TEXT_PRIVATE_H
#define UMICOM_CSV_TEXT_PRIVATE_H
#include <stddef.h>
#include <stdint.h>
#include "utf8_text_private.h"

/* Reject malformed UTF-8 and embedded controls before either CSV direction
 * retains text. The caller supplies its whole-input or individual-field bound. */
/* Preserve combined Unicode/control validation for review. The replacement
 * shares Unicode validity with JSON reporting while retaining CSV controls. */
#if 0
static inline int CsvTextValid(const char *text, size_t length, size_t maximum)
{
    if (length > maximum || (text == NULL && length != 0U)) return 0;
    for (size_t i = 0U; i < length;) {
        unsigned char c = (unsigned char)text[i++];
        if (c < 0x80U) {
            if ((c < 0x20U && c != '\t' && c != '\r' && c != '\n') || c == 0x7fU) return 0;
            continue;
        }
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
static inline int CsvTextValid(const char *text, size_t length, size_t maximum)
{
    if (length > maximum || !UmiUtf8TextValid(text, length)) return 0;
    for (size_t i = 0U; i < length; ++i) {
        unsigned char c = (unsigned char)text[i];
        if ((c < 0x20U && c != '\t' && c != '\r' && c != '\n') || c == 0x7fU) return 0;
    }
    return 1;
}

#endif
