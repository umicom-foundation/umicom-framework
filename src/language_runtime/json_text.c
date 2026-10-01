/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/json_text.c
 * PURPOSE: Validate complete UTF-8 strings before publishing them to native consumers.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/json_text.h"
#include <stdint.h>
#include <string.h>
static int Hex(unsigned char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}
static UmiStatus Hex4(const unsigned char *text, size_t size, size_t *at, uint32_t *out)
{
    if (size - *at < 4U) return UMI_STATUS_PARSE_ERROR;
    uint32_t cp = 0U;
    for (size_t i = 0; i < 4U; ++i) {
        int h = Hex(text[(*at)++]);
        if (h < 0) return UMI_STATUS_PARSE_ERROR;
        cp = cp * 16U + (uint32_t)h;
    }
    *out = cp; return UMI_STATUS_OK;
}
static UmiStatus Next(const unsigned char *text, size_t size, size_t *at, uint32_t *out)
{
    uint32_t cp = text[(*at)++];
    if (cp == '\\') {
        if (*at == size) return UMI_STATUS_PARSE_ERROR;
        cp = text[(*at)++];
        switch (cp) {
        case '"': case '\\': case '/': break;
        case 'b': cp = '\b'; break; case 'f': cp = '\f'; break;
        case 'n': cp = '\n'; break; case 'r': cp = '\r'; break; case 't': cp = '\t'; break;
        case 'u': {
            UmiStatus status = Hex4(text, size, at, &cp);
            if (status != UMI_STATUS_OK) return status;
            if (cp >= 0xd800U && cp <= 0xdbffU) {
                if (size - *at < 6U || text[*at] != '\\' || text[*at + 1U] != 'u') return UMI_STATUS_PARSE_ERROR;
                *at += 2U; uint32_t low;
                status = Hex4(text, size, at, &low);
                if (status != UMI_STATUS_OK || low < 0xdc00U || low > 0xdfffU) return UMI_STATUS_PARSE_ERROR;
                cp = 0x10000U + (cp - 0xd800U) * 1024U + low - 0xdc00U;
            } else if (cp >= 0xdc00U && cp <= 0xdfffU) return UMI_STATUS_PARSE_ERROR;
            break;
        }
        default: return UMI_STATUS_PARSE_ERROR;
        }
    } else if (cp < 0x20U || cp == '"') return UMI_STATUS_PARSE_ERROR;
    else if (cp >= 0x80U) {
        unsigned extra; uint32_t minimum;
        if (cp >= 0xc2U && cp <= 0xdfU) { extra = 1; minimum = 0x80U; cp &= 31U; }
        else if (cp >= 0xe0U && cp <= 0xefU) { extra = 2; minimum = 0x800U; cp &= 15U; }
        else if (cp >= 0xf0U && cp <= 0xf4U) { extra = 3; minimum = 0x10000U; cp &= 7U; }
        else return UMI_STATUS_PARSE_ERROR;
        if (size - *at < extra) return UMI_STATUS_PARSE_ERROR;
        for (unsigned i = 0; i < extra; ++i) {
            unsigned char byte = text[(*at)++];
            if ((byte & 0xc0U) != 0x80U) return UMI_STATUS_PARSE_ERROR;
            cp = cp * 64U + (byte & 63U);
        }
        if (cp < minimum || cp > 0x10ffffU || (cp >= 0xd800U && cp <= 0xdfffU)) return UMI_STATUS_PARSE_ERROR;
    }
    if (cp == 0U) return UMI_STATUS_PARSE_ERROR;
    *out = cp; return UMI_STATUS_OK;
}
static size_t Encode(uint32_t cp, char *out)
{
    unsigned char bytes[4]; size_t n;
    if (cp < 0x80U) { n = 1U; bytes[0] = (unsigned char)cp; }
    else if (cp < 0x800U) { n = 2U; bytes[0] = (unsigned char)(0xc0U | (cp >> 6U)); bytes[1] = (unsigned char)(0x80U | (cp & 63U)); }
    else if (cp < 0x10000U) {
        n = 3U; bytes[0] = (unsigned char)(0xe0U | (cp >> 12U));
        bytes[1] = (unsigned char)(0x80U | ((cp >> 6U) & 63U)); bytes[2] = (unsigned char)(0x80U | (cp & 63U));
    } else {
        n = 4U; bytes[0] = (unsigned char)(0xf0U | (cp >> 18U)); bytes[1] = (unsigned char)(0x80U | ((cp >> 12U) & 63U));
        bytes[2] = (unsigned char)(0x80U | ((cp >> 6U) & 63U)); bytes[3] = (unsigned char)(0x80U | (cp & 63U));
    }
    if (out != NULL) for (size_t i = 0; i < n; ++i) out[i] = (char)bytes[i];
    return n;
}
UmiStatus UmiLanguageRuntimeJsonText(const UmiLanguageRuntimeJsonDocument *document, int token, char *out, size_t capacity)
{
    if (document == NULL || document->json == NULL || out == NULL || capacity == 0U || token < 0 ||
        (size_t)token >= document->token_count || document->token_count > UMI_LANGUAGE_RUNTIME_MAX_TOKENS || document->tokens[token].type != UMI_LANGUAGE_RUNTIME_JSON_STRING)
        return UMI_STATUS_INVALID_ARGUMENT;
    const UmiLanguageRuntimeJsonToken *value = &document->tokens[token];
    if (value->start < 0 || value->end < value->start || (size_t)value->end > strlen(document->json))
        return UMI_STATUS_INVALID_ARGUMENT;
    const unsigned char *text = (const unsigned char *)document->json + value->start;
    size_t size = (size_t)(value->end - value->start), used = 0U, at = 0U;
    /* First pass proves validity and capacity before touching caller output. */
    while (at < size) {
        uint32_t cp; UmiStatus status = Next(text, size, &at, &cp);
        if (status != UMI_STATUS_OK) return status;
        size_t n = Encode(cp, NULL);
        if (n >= capacity - used) return UMI_STATUS_CAPACITY_EXCEEDED;
        used += n;
    }
    at = used = 0U;
    while (at < size) {
        uint32_t cp = 0U; (void)Next(text, size, &at, &cp); used += Encode(cp, out + used);
    }
    out[used] = '\0'; return UMI_STATUS_OK;
}
