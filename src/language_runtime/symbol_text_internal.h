/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/symbol_text_internal.h
 * PURPOSE: Share decoded symbol labels between source outlines and call relationship readers.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_SYMBOL_TEXT_INTERNAL_H
#define UMICOM_LANGUAGE_RUNTIME_SYMBOL_TEXT_INTERNAL_H
#include "source_location_internal.h"
static UmiStatus SymbolText(const UmiJsonTree *tree, int object, const char *name, int required, char **out)
{
    int node = -1;
    UmiStatus status = LocationMember(tree, object, name, required, &node);
    if (status != UMI_STATUS_OK || node < 0)
        return status;
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_STRING)
        return UMI_STATUS_PARSE_ERROR;
    const char *raw = NULL;
    size_t bytes = 0U;
    status = UmiJsonTreeSourceSpan(tree, node, &raw, &bytes);
    (void)raw;
    char *text = status == UMI_STATUS_OK ? malloc(bytes + 1U) : NULL;
    if (status == UMI_STATUS_OK && text == NULL)
        status = UMI_STATUS_OUT_OF_MEMORY;
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeText(tree, node, text, bytes + 1U);
    if (status == UMI_STATUS_OK && strlen(text) > 4096U)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK)
        *out = text;
    else
        free(text);
    return status;
}
/* Classify already validated UTF-8 without depending on a process locale. */
static int SymbolVisibleName(const char *text)
{
    const unsigned char *p = (const unsigned char *)text;
    while (*p != '\0')
    {
        uint32_t value = *p++;
        unsigned extra = 0U;
        if (value >= 0xf0U)
        {
            value &= 7U;
            extra = 3U;
        }
        else if (value >= 0xe0U)
        {
            value &= 15U;
            extra = 2U;
        }
        else if (value >= 0xc0U)
        {
            value &= 31U;
            extra = 1U;
        }
        while (extra != 0U)
        {
            value = (value << 6U) | (*p++ & 63U);
            --extra;
        }
        int whitespace = (value >= 9U && value <= 13U) || value == 32U || value == 0x85U || value == 0xa0U ||
                         value == 0x1680U || (value >= 0x2000U && value <= 0x200aU) || value == 0x2028U ||
                         value == 0x2029U || value == 0x202fU || value == 0x205fU || value == 0x3000U;
        if (!whitespace)
            return 1;
    }
    return 0;
}
#endif
