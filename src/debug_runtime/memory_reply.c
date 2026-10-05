/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/memory_reply.c
 * PURPOSE: Validate memory payloads before publishing any bytes to a debugger view.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/memory_inspection.h"
#include "umicom/language_runtime/json_document.h"
#include "umicom/language_runtime/json_text.h"
#include <stdlib.h>
#include <string.h>

/* Escaped member names participate in duplicate detection. Decode keys before
 * comparing them, and leave unrecognized extension values uninterpreted. */
static UmiStatus MemoryMember(const UmiLanguageRuntimeJsonDocument *doc, int object, const char *name,
                              int *out)
{
    *out = -1;
    if (object < 0 || doc->tokens[object].type != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        return UMI_STATUS_PARSE_ERROR;
    size_t count = umi_language_runtime_json_object_count(doc, object);
    for (size_t i = 0U; i < count; ++i)
    {
        int key, value;
        char text[256];
        UmiStatus status = umi_language_runtime_json_object_entry_at(doc, object, i, &key, &value);
        if (status == UMI_STATUS_OK)
            status = UmiLanguageRuntimeJsonText(doc, key, text, sizeof text);
        if (status != UMI_STATUS_OK)
            return status;
        if (strcmp(text, name) == 0)
        {
            if (*out >= 0)
                return UMI_STATUS_PARSE_ERROR;
            *out = value;
        }
    }
    return UMI_STATUS_OK;
}
static UmiStatus MemoryText(const UmiLanguageRuntimeJsonDocument *doc, int token, char *out, size_t capacity)
{
    if (token < 0 || doc->tokens[token].type != UMI_LANGUAGE_RUNTIME_JSON_STRING)
        return UMI_STATUS_PARSE_ERROR;
    return UmiLanguageRuntimeJsonText(doc, token, out, capacity);
}
static int MemoryAddress(const char *text)
{
    int hex = text[0] == '0' && text[1] == 'x';
    const unsigned char *p = (const unsigned char *)text + (hex ? 2 : 0);
    if (*p == 0)
        return 0;
    for (; *p; ++p)
        if (!(*p >= '0' && *p <= '9') && !(hex && ((*p >= 'a' && *p <= 'f') || (*p >= 'A' && *p <= 'F'))))
            return 0;
    return 1;
}
static int MemoryDigit(unsigned char c)
{
    if (c >= 'A' && c <= 'Z')
        return c - 'A';
    if (c >= 'a' && c <= 'z')
        return c - 'a' + 26;
    if (c >= '0' && c <= '9')
        return c - '0' + 52;
    if (c == '+')
        return 62;
    if (c == '/')
        return 63;
    return -1;
}
/* This decoder deliberately checks trailing padding and unused bits. The
 * legacy decoder remains available; a reviewed capture must not accept a
 * valid prefix followed by corrupt data and silently display partial bytes. */
static UmiStatus MemoryBase64(const char *text, uint32_t requested, UmiDebugMemoryBytes *out)
{
    size_t length = strlen(text);
    if (length % 4U != 0U)
        return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 0U; i < length; i += 4U)
    {
        int a = MemoryDigit((unsigned char)text[i]), b = MemoryDigit((unsigned char)text[i + 1U]);
        int c = text[i + 2U] == '=' ? 0 : MemoryDigit((unsigned char)text[i + 2U]);
        int d = text[i + 3U] == '=' ? 0 : MemoryDigit((unsigned char)text[i + 3U]);
        int pad = text[i + 2U] == '=' ? 2 : text[i + 3U] == '=' ? 1 : 0;
        if (a < 0 || b < 0 || c < 0 || d < 0 || (pad && i + 4U != length) ||
            (pad == 2 && text[i + 3U] != '=') || (pad == 2 && (b & 15) != 0) || (pad == 1 && (c & 3) != 0))
            return UMI_STATUS_PARSE_ERROR;
        size_t n = 3U - (size_t)pad;
        if (n > (size_t)requested - out->count)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        unsigned packed = ((unsigned)a << 18) | ((unsigned)b << 12) | ((unsigned)c << 6) | (unsigned)d;
        for (size_t j = 0U; j < n; ++j)
            out->bytes[out->count++] = (unsigned char)(packed >> (16U - (unsigned)j * 8U));
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiDebugMemoryDecode(const char *json, uint32_t requested, UmiDebugMemoryBytes *out)
{
    if (json == NULL || out == NULL || requested == 0U || requested > UMI_DEBUG_MEMORY_CAPTURE_CAPACITY)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiLanguageRuntimeJsonDocument *doc = malloc(sizeof *doc);
    UmiDebugMemoryBytes *value = calloc(1U, sizeof *value);
    /* Base64 expands each group of three bytes to four characters. */
    char *data = calloc(1U, ((UMI_DEBUG_MEMORY_CAPTURE_CAPACITY + 2U) / 3U) * 4U + 1U);
    if (doc == NULL || value == NULL || data == NULL)
    {
        free(doc);
        free(value);
        free(data);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    int body = -1, address = -1, bytes = -1, unreadable = -1;
    UmiStatus status = UmiLanguageRuntimeJsonParseComplete(json, doc);
    if (status == UMI_STATUS_OK)
        status = MemoryMember(doc, 0, "body", &body);
    if (status == UMI_STATUS_OK && body < 0)
        status = UMI_STATUS_NOT_FOUND;
    if (status == UMI_STATUS_OK)
        status = MemoryMember(doc, body, "address", &address);
    if (status == UMI_STATUS_OK)
        status = MemoryText(doc, address, value->address, sizeof value->address);
    if (status == UMI_STATUS_OK && !MemoryAddress(value->address))
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = MemoryMember(doc, body, "data", &bytes);
    if (status == UMI_STATUS_OK && bytes >= 0)
        status = MemoryText(doc, bytes, data, ((UMI_DEBUG_MEMORY_CAPTURE_CAPACITY + 2U) / 3U) * 4U + 1U);
    if (status == UMI_STATUS_OK)
        status = MemoryBase64(data, requested, value);
    if (status == UMI_STATUS_OK)
        status = MemoryMember(doc, body, "unreadableBytes", &unreadable);
    if (status == UMI_STATUS_OK && unreadable >= 0)
    {
        const UmiLanguageRuntimeJsonToken *token = &doc->tokens[unreadable];
        if (token->type != UMI_LANGUAGE_RUNTIME_JSON_PRIMITIVE || token->end <= token->start)
            status = UMI_STATUS_PARSE_ERROR;
        for (int i = token->start; status == UMI_STATUS_OK && i < token->end; ++i)
        {
            unsigned char c = (unsigned char)doc->json[i];
            if (c < '0' || c > '9')
            {
                status = UMI_STATUS_PARSE_ERROR;
                break;
            }
            uint64_t digit = (uint64_t)(c - '0');
            if (value->unreadable > ((uint64_t)UMI_DEBUG_MEMORY_OFFSET_LIMIT - digit) / 10U)
            {
                status = UMI_STATUS_CAPACITY_EXCEEDED;
                break;
            }
            value->unreadable = value->unreadable * 10U + digit;
        }
    }
    if (status == UMI_STATUS_OK)
        *out = *value;
    free(doc);
    free(value);
    free(data);
    return status;
}
