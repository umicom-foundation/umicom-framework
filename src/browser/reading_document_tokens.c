/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/browser/reading_document_tokens.c
 *
 * PURPOSE:
 *   Read bounded tags, quoted attributes and text entities without browser execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "reading_document_internal.h"
#include <stdint.h>
#include <string.h>

int ReadingSpace(unsigned char ch)
{ return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n' || ch == '\f'; }
static unsigned char Lower(unsigned char ch)
{ return ch >= 'A' && ch <= 'Z' ? (unsigned char)(ch + ('a' - 'A')) : ch; }
int ReadingEqual(const char *a, size_t count, const char *b)
{
    if (strlen(b) != count) return 0;
    for (size_t i = 0U; i < count; ++i)
        if (Lower((unsigned char)a[i]) != (unsigned char)b[i]) return 0;
    return 1;
}
static int NameByte(unsigned char ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
        (ch >= '0' && ch <= '9') || ch == '-' || ch == '_' || ch == ':';
}
UmiStatus ReadingParseTag(const char *bytes, size_t size, size_t *cursor, ReadingTag *tag)
{
    size_t p = *cursor;
    memset(tag, 0, sizeof(*tag));
    if (p >= size || bytes[p++] != '<') return UMI_STATUS_INVALID_ARGUMENT;
    if (p < size && bytes[p] == '/') { tag->closing = 1; ++p; }
    size_t start = p;
    while (p < size && NameByte((unsigned char)bytes[p])) ++p;
    size_t length = p - start;
    if (length == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (length >= sizeof(tag->name)) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < length; ++i) tag->name[i] = (char)Lower((unsigned char)bytes[start + i]);
    if (p < size && !ReadingSpace((unsigned char)bytes[p]) && bytes[p] != '>' && bytes[p] != '/')
        return UMI_STATUS_INVALID_ARGUMENT;
    start = p;
    char quote = 0;
    for (; p < size; ++p) {
        char ch = bytes[p];
        if (quote != 0) { if (ch == quote) quote = 0; }
        else if (ch == '"' || ch == '\'') quote = ch;
        else if (ch == '>') break;
        else if (ch == '<') return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (p == size || quote != 0) return UMI_STATUS_INVALID_ARGUMENT;
    size_t end = p;
    while (end > start && ReadingSpace((unsigned char)bytes[end - 1U])) --end;
    if (end > start && bytes[end - 1U] == '/') { tag->self_closing = 1; --end; }
    if (tag->closing) {
        for (size_t i = start; i < end; ++i)
            if (!ReadingSpace((unsigned char)bytes[i])) return UMI_STATUS_INVALID_ARGUMENT;
        if (tag->self_closing) return UMI_STATUS_INVALID_ARGUMENT;
    }
    tag->attributes = bytes + start;
    tag->attributes_size = end - start;
    *cursor = p + 1U;
    return UMI_STATUS_OK;
}
UmiStatus ReadingDecode(const char *bytes, size_t size, size_t *consumed,
    char output[4], size_t *written)
{
    if (size == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    *consumed = 1U; *written = 1U; output[0] = bytes[0];
    if (bytes[0] != '&') return UMI_STATUS_OK;
    static const struct { const char *text; char value; } named[] = {
        {"&amp;", '&'}, {"&lt;", '<'}, {"&gt;", '>'}, {"&quot;", '"'},
        {"&apos;", '\''}, {"&nbsp;", ' '}
    };
    for (size_t i = 0U; i < sizeof(named)/sizeof(named[0]); ++i) {
        size_t length = strlen(named[i].text);
        if (size >= length && memcmp(bytes, named[i].text, length) == 0) {
            *consumed = length; output[0] = named[i].value; return UMI_STATUS_OK;
        }
    }
    if (size < 3U || bytes[1] != '#') return UMI_STATUS_OK;
    size_t p = 2U;
    uint32_t base = 10U, value = 0U;
    if (p < size && (bytes[p] == 'x' || bytes[p] == 'X')) { base = 16U; ++p; }
    size_t first = p;
    while (p < size && bytes[p] != ';') {
        unsigned char ch = Lower((unsigned char)bytes[p]);
        uint32_t digit;
        if (ch >= '0' && ch <= '9') digit = (uint32_t)(ch - '0');
        else if (base == 16U && ch >= 'a' && ch <= 'f') digit = (uint32_t)(ch - 'a') + 10U;
        else return UMI_STATUS_INVALID_ARGUMENT;
        if (value > (UINT32_C(0x10ffff) - digit)/base) return UMI_STATUS_INVALID_ARGUMENT;
        value = value*base + digit; ++p;
        if (p - first > 8U) return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (p == size || p == first || value == 0U || (value >= 0xd800U && value <= 0xdfffU) ||
        (value < 0x20U && value != '\t' && value != '\n' && value != '\r') ||
        (value >= 0x7fU && value <= 0x9fU)) return UMI_STATUS_INVALID_ARGUMENT;
    *consumed = p + 1U;
    if (value <= 0x7fU) output[0] = (char)value;
    else if (value <= 0x7ffU) {
        *written = 2U; output[0] = (char)(0xc0U | (value >> 6U));
        output[1] = (char)(0x80U | (value & 0x3fU));
    } else if (value <= 0xffffU) {
        *written = 3U; output[0] = (char)(0xe0U | (value >> 12U));
        output[1] = (char)(0x80U | ((value >> 6U) & 0x3fU));
        output[2] = (char)(0x80U | (value & 0x3fU));
    } else {
        *written = 4U; output[0] = (char)(0xf0U | (value >> 18U));
        output[1] = (char)(0x80U | ((value >> 12U) & 0x3fU));
        output[2] = (char)(0x80U | ((value >> 6U) & 0x3fU));
        output[3] = (char)(0x80U | (value & 0x3fU));
    }
    return UMI_STATUS_OK;
}
UmiStatus ReadingAttribute(const ReadingTag *tag, const char *name,
    char *output, size_t capacity, int *found)
{
    const char *bytes = tag->attributes;
    size_t size = tag->attributes_size, p = 0U;
    *found = 0; output[0] = '\0';
    while (p < size) {
        while (p < size && ReadingSpace((unsigned char)bytes[p])) ++p;
        if (p == size) break;
        size_t start = p;
        while (p < size && NameByte((unsigned char)bytes[p])) ++p;
        if (p == start) return UMI_STATUS_INVALID_ARGUMENT;
        int wanted = ReadingEqual(bytes + start, p - start, name);
        while (p < size && ReadingSpace((unsigned char)bytes[p])) ++p;
        size_t begin = p, end = p;
        if (p < size && bytes[p] == '=') {
            ++p;
            while (p < size && ReadingSpace((unsigned char)bytes[p])) ++p;
            if (p == size) return UMI_STATUS_INVALID_ARGUMENT;
            if (bytes[p] == '"' || bytes[p] == '\'') {
                char quote = bytes[p++]; begin = p;
                while (p < size && bytes[p] != quote) ++p;
                if (p == size) return UMI_STATUS_INVALID_ARGUMENT;
                end = p++;
            } else {
                begin = p;
                while (p < size && !ReadingSpace((unsigned char)bytes[p])) {
                    if (bytes[p] == '"' || bytes[p] == '\'' || bytes[p] == '=' || bytes[p] == '<' || bytes[p] == 0x60)
                        return UMI_STATUS_INVALID_ARGUMENT;
                    ++p;
                }
                end = p;
            }
            if (p < size && !ReadingSpace((unsigned char)bytes[p])) return UMI_STATUS_INVALID_ARGUMENT;
        }
        if (wanted) {
            if (*found) return UMI_STATUS_INVALID_ARGUMENT;
            *found = 1; size_t written = 0U;
            for (size_t i = begin; i < end;) {
                char decoded[4]; size_t used, count;
                UmiStatus status = ReadingDecode(bytes + i, end - i, &used, decoded, &count);
                if (status != UMI_STATUS_OK) return status;
                if (count >= capacity - written) return UMI_STATUS_CAPACITY_EXCEEDED;
                memcpy(output + written, decoded, count); written += count; i += used;
            }
            output[written] = '\0';
        }
    }
    return UMI_STATUS_OK;
}
