/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/enterprise_workspace/text.c
 *
 * PURPOSE:
 *   Validate owned UTF-8 values and stable identifiers before storage or display.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <string.h>
#include <stdint.h>

bool EwsId(const char *text)
{
    size_t i;
    if (text == NULL || text[0] == '\0') return false;
    for (i = 0U; i < UMI_ENTERPRISE_ID_CAPACITY; ++i) {
        unsigned char ch = (unsigned char)text[i];
        if (ch == 0U) return i > 0U;
        if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
            (ch >= '0' && ch <= '9')) continue;
        if (i == 0U || (ch != '.' && ch != '_' && ch != '-')) return false;
    }
    return false;
}

/* Decode scalars without reading beyond length. Surrogates, overlong UTF-8,
 * NUL and non-display controls cannot enter saved records or GTK labels. */
bool EwsUtf8(const char *text, size_t length, bool multiline)
{
    size_t i = 0U;
    if (text == NULL) return false;
    while (i < length) {
        uint32_t ch = (unsigned char)text[i++];
        unsigned more;
        uint32_t minimum;
        if (ch < 0x80U) {
            if (ch == 0U || ch == 0x7fU ||
                (ch < 0x20U && !(multiline && (ch == '\n' || ch == '\r' || ch == '\t')))) return false;
            continue;
        }
        if (ch >= 0xc2U && ch <= 0xdfU) { more = 1U; minimum = 0x80U; ch &= 0x1fU; }
        else if (ch >= 0xe0U && ch <= 0xefU) { more = 2U; minimum = 0x800U; ch &= 0x0fU; }
        else if (ch >= 0xf0U && ch <= 0xf4U) { more = 3U; minimum = 0x10000U; ch &= 7U; }
        else return false;
        if (length - i < more) return false;
        for (unsigned j = 0U; j < more; ++j) {
            unsigned char next = (unsigned char)text[i++];
            if ((next & 0xc0U) != 0x80U) return false;
            ch = (ch << 6U) | (next & 0x3fU);
        }
        if (ch < minimum || ch > 0x10ffffU || (ch >= 0xd800U && ch <= 0xdfffU) ||
            (ch >= 0x80U && ch <= 0x9fU)) return false;
    }
    return true;
}
bool EwsText(const char *text, size_t capacity, bool empty, bool multiline)
{
    size_t length = 0U;
    if (text == NULL) return false;
    while (length < capacity && text[length] != '\0') ++length;
    return length < capacity && (empty || length > 0U) && EwsUtf8(text, length, multiline);
}
UmiStatus EwsCopy(char *destination, size_t capacity, const char *source)
{
    size_t length = 0U;
    if (destination == NULL || source == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    while (length < capacity && source[length] != '\0') ++length;
    if (length == capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}
size_t EwsDatasetIndex(const EwsState *state, const char *id)
{
    for (size_t i = 0U; i < state->datasetCount; ++i)
        if (strcmp(state->datasets[i].info.id, id) == 0) return i;
    return SIZE_MAX;
}
size_t EwsJobIndex(const EwsState *state, const char *id)
{
    for (size_t i = 0U; i < state->jobCount; ++i)
        if (strcmp(state->jobs[i].info.id, id) == 0) return i;
    return SIZE_MAX;
}
