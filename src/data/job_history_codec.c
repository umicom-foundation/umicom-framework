/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/job_history_codec.c
 * PURPOSE: Validate bounded job records before accepting persisted state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "job_history_internal.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

/* Identifiers become part of storage keys; punctuation that could change the
 * namespace is rejected. Captions are opaque text and are hex-encoded below. */
bool UmiJobHistoryIdentifier(const char *text, size_t capacity)
{
    if (text == NULL || text[0] == '\0')
        return false;
    for (size_t i = 0; i < capacity; ++i)
    {
        unsigned char c = (unsigned char)text[i];
        if (c == 0)
            return true;
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '.' ||
              c == '_' || c == '-'))
            return false;
    }
    return false;
}
bool UmiJobHistoryCaption(const char *text, size_t capacity)
{
    if (text == NULL || text[0] == '\0')
        return false;
    for (size_t i = 0; i < capacity; ++i)
    {
        unsigned char c = (unsigned char)text[i];
        if (c == 0)
            return true;
        if (c < 32U || c == 127U)
            return false;
    }
    return false;
}
/* Manual decimal parsing rejects overflow, signs and trailing junk without
 * depending on locale or the platform width of unsigned long. */
bool UmiJobHistoryReadNumber(const char **cursor, uint64_t *out, char delimiter)
{
    const char *p = *cursor;
    uint64_t value = 0;
    if (*p < '0' || *p > '9')
        return false;
    do
    {
        unsigned digit = (unsigned)(*p - '0');
        if (value > (UINT64_MAX - digit) / 10U)
            return false;
        value = value * 10U + digit;
        ++p;
    } while (*p >= '0' && *p <= '9');
    if (*p != delimiter)
        return false;
    *out = value;
    *cursor = delimiter == '\0' ? p : p + 1;
    return true;
}
static bool valid_entry(const UmiJobHistoryEntry *e)
{
    if (e->id == 0 || e->revision == 0 || e->total_steps == 0 || e->total_steps > 64U ||
        e->completed_steps > e->total_steps || !UmiJobHistoryIdentifier(e->kind, sizeof(e->kind)) ||
        !UmiJobHistoryCaption(e->label, sizeof(e->label)))
        return false;
    switch (e->state)
    {
    case UMI_JOB_HISTORY_PREPARED:
        return e->completed_steps == 0 && e->result == UMI_STATUS_OK;
    case UMI_JOB_HISTORY_RUNNING:
        return e->result == UMI_STATUS_OK;
    case UMI_JOB_HISTORY_SUCCEEDED:
        return e->completed_steps == e->total_steps && e->result == UMI_STATUS_OK;
    case UMI_JOB_HISTORY_CANCELLED:
        return e->result == UMI_STATUS_CANCELLED;
    case UMI_JOB_HISTORY_FAILED:
        return e->result > UMI_STATUS_OK && e->result <= UMI_STATUS_BUSY && e->result != UMI_STATUS_CANCELLED;
    default:
        return false;
    }
}
static void encode_text(const char *text, char *out)
{
    static const char digits[] = "0123456789abcdef";
    while (*text != '\0')
    {
        unsigned char c = (unsigned char)*text++;
        *out++ = digits[c >> 4U];
        *out++ = digits[c & 15U];
    }
    *out = '\0';
}
static int digit_value(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    return -1;
}
static bool decode_text(const char **cursor, char *out, size_t capacity, char delimiter)
{
    size_t count = 0;
    const char *p = *cursor;
    while (*p != delimiter)
    {
        int high = digit_value(*p);
        if (high < 0)
            return false;
        ++p;
        int low = digit_value(*p);
        if (low < 0 || count + 1U >= capacity)
            return false;
        ++p;
        int value = high * 16 + low;
        if (value == 0)
            return false;
        out[count++] = (char)(unsigned char)value;
    }
    out[count] = '\0';
    *cursor = delimiter == '\0' ? p : p + 1;
    return true;
}
UmiStatus UmiJobHistoryEncode(const UmiJobHistoryEntry *entry, char *out, size_t capacity)
{
    if (entry == NULL || out == NULL || !valid_entry(entry))
        return UMI_STATUS_INVALID_ARGUMENT;
    char kind[UMI_JOB_HISTORY_KIND_CAPACITY * 2U], label[UMI_JOB_HISTORY_LABEL_CAPACITY * 2U];
    char wire[UMI_JOB_HISTORY_WIRE_CAPACITY];
    encode_text(entry->kind, kind);
    encode_text(entry->label, label);
    /* The first field is a storage format marker, not a product release number.
     * Future codecs must explicitly migrate it rather than guessing layouts. */
    int length = snprintf(wire, sizeof(wire), "1|%" PRIu64 "|%" PRIu64 "|%u|%u|%u|%u|%s|%s", entry->id,
                          entry->revision, (unsigned)entry->state, entry->completed_steps, entry->total_steps,
                          (unsigned)entry->result, kind, label);
    if (length < 0 || (size_t)length >= sizeof(wire))
        return UMI_STATUS_INTERNAL_ERROR;
    if ((size_t)length >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, wire, (size_t)length + 1U);
    return UMI_STATUS_OK;
}
UmiStatus UmiJobHistoryDecode(const char *text, UmiJobHistoryEntry *out)
{
    if (text == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiJobHistoryEntry entry = {0};
    uint64_t numbers[7];
    const char *cursor = text;
    for (size_t i = 0; i < 7U; ++i)
        if (!UmiJobHistoryReadNumber(&cursor, &numbers[i], '|'))
            return UMI_STATUS_PARSE_ERROR;
    if (numbers[0] != 1U || numbers[3] > UMI_JOB_HISTORY_CANCELLED || numbers[4] > 64U || numbers[5] > 64U ||
        numbers[6] > UMI_STATUS_BUSY)
        return UMI_STATUS_PARSE_ERROR;
    entry.id = numbers[1];
    entry.revision = numbers[2];
    entry.state = (UmiJobHistoryState)numbers[3];
    entry.completed_steps = (unsigned)numbers[4];
    entry.total_steps = (unsigned)numbers[5];
    entry.result = (UmiStatus)numbers[6];
    if (!decode_text(&cursor, entry.kind, sizeof(entry.kind), '|') ||
        !decode_text(&cursor, entry.label, sizeof(entry.label), '\0') || !valid_entry(&entry))
        return UMI_STATUS_PARSE_ERROR;
    *out = entry;
    return UMI_STATUS_OK;
}
