/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/theme_validation_internal.h
 *
 * PURPOSE:
 *   Share bounded colour and font-list admission across toolkit-neutral appearance and editor theme owners.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_UI_THEME_VALIDATION_INTERNAL_H
#define UMICOM_UI_THEME_VALIDATION_INTERNAL_H
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "../base/storage_internal.h"

/* Callers supply the actual readable inline field capacity. A bounded scan
 * admits storage; it cannot establish pointer lifetime or concurrent safety. */
static inline bool UmiUiThemeTextIsBounded(const char *text, size_t capacity,
                                          bool require_nonempty)
{
    return text != NULL && capacity != 0U &&
           (!require_nonempty || text[0] != '\0') &&
           memchr(text, '\0', capacity) != NULL;
}

/* Semantic tokens deliberately retain the existing six-digit RGB contract.
 * No renderer CSS parser, named colour, alpha channel or truncation is used. */
static inline bool UmiUiThemeHexColourIsValid(const char *text, size_t capacity)
{
    if (text == NULL || capacity < 8U || text[0] != '#') return false;
    for (size_t index = 1U; index < 7U; ++index) {
        const unsigned char c = (unsigned char)text[index];
        if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') ||
              (c >= 'a' && c <= 'f'))) return false;
    }
    return text[7] == '\0';
}

/* Both arguments must already be admitted #RRGGBB values. Case changes the
 * spelling, not the colour, so presentation rules compare normalized digits. */
static inline bool UmiUiThemeHexColoursEqual(const char *left, const char *right)
{
    for (size_t index = 1U; index < 7U; ++index) {
        unsigned char a = (unsigned char)left[index], b = (unsigned char)right[index];
        if (a >= 'a' && a <= 'f') a = (unsigned char)(a - ('a' - 'A'));
        if (b >= 'a' && b <= 'f') b = (unsigned char)(b - ('a' - 'A'));
        if (a != b) return false;
    }
    return true;
}

/* Preserve the appearance profile's existing ASCII fallback grammar. Each
 * comma-separated family needs an alphanumeric name, not just punctuation. */
static inline bool UmiUiThemeFontListIsValid(const char *font, size_t capacity)
{
    if (!UmiUiThemeTextIsBounded(font, capacity, true)) return false;
    bool named = false;
    for (const unsigned char *p = (const unsigned char *)font; ; ++p) {
        const unsigned char c = *p;
        if (c == ',' || c == '\0') {
            if (!named) return false;
            if (c == '\0') return true;
            named = false;
        } else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
                   (c >= '0' && c <= '9')) {
            named = true;
        } else if (c != ' ' && c != '-' && c != '_' && c != '.') {
            return false;
        }
    }
}
#endif
