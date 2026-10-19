/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/qualification_parse_fields.c
 * Purpose: Parse bounded canonical report fields without interpreting names as commands.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "qualification_parse_internal.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
bool UmiQualificationFieldIs(UmiQualificationField field, const char *text)
{
    size_t length = strlen(text);
    return field.length == length && memcmp(field.bytes, text, length) == 0;
}
UmiStatus UmiQualificationReadRow(UmiQualificationReader *reader, UmiQualificationField *fields,
    size_t capacity, size_t *outCount)
{
    const char *start = reader->next;
    size_t count = 0U;
    for (const char *cursor = start; cursor < reader->end; ++cursor) {
        if (*cursor != '\t' && *cursor != '\n') continue;
        if (count == capacity) return UMI_STATUS_PARSE_ERROR;
        fields[count++] = (UmiQualificationField){start, (size_t)(cursor - start)};
        if (*cursor == '\n') {
            reader->next = cursor + 1; *outCount = count; return UMI_STATUS_OK;
        }
        start = cursor + 1;
    }
    return UMI_STATUS_PARSE_ERROR;
}
UmiStatus UmiQualificationReadNumber(UmiQualificationField field, uint64_t maximum, uint64_t *out)
{
    if (field.length == 0U || (field.length > 1U && field.bytes[0] == '0'))
        return UMI_STATUS_PARSE_ERROR;
    uint64_t value = 0U;
    for (size_t i = 0U; i < field.length; ++i) {
        unsigned char c = (unsigned char)field.bytes[i];
        if (c < '0' || c > '9') return UMI_STATUS_PARSE_ERROR;
        uint64_t digit = (uint64_t)(c - '0');
        if (digit > maximum || value > (maximum - digit) / 10U) return UMI_STATUS_PARSE_ERROR;
        value = value * 10U + digit;
    }
    *out = value; return UMI_STATUS_OK;
}
UmiStatus UmiQualificationReadDigest(UmiQualificationField field, bool optional, char out[65])
{
    if (field.length == 0U && optional) { out[0] = '\0'; return UMI_STATUS_OK; }
    if (field.length != 64U) return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 0U; i < field.length; ++i) {
        char c = field.bytes[i];
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'))) return UMI_STATUS_PARSE_ERROR;
    }
    memcpy(out, field.bytes, 64U); out[64] = '\0'; return UMI_STATUS_OK;
}
static int Hex(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}
UmiStatus UmiQualificationReadName(UmiQualificationReader *reader, UmiQualificationField field, char **out)
{
    if (field.length == 0U || field.length % 2U != 0U) return UMI_STATUS_PARSE_ERROR;
    size_t length = field.length / 2U;
    if (length > UMI_RELEASE_INVENTORY_FIELD_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length + 1U > UMI_RELEASE_INVENTORY_TEXT_LIMIT - reader->names)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    char *name = malloc(length + 1U);
    if (name == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    for (size_t i = 0U; i < length; ++i) {
        int high = Hex(field.bytes[2U * i]), low = Hex(field.bytes[2U * i + 1U]);
        if (high < 0 || low < 0 || (high == 0 && low == 0)) {
            free(name); return UMI_STATUS_PARSE_ERROR;
        }
        name[i] = (char)((unsigned)high * 16U + (unsigned)low);
    }
    name[length] = '\0'; reader->names += length + 1U; *out = name;
    return UMI_STATUS_OK;
}
static UmiStatus ExitCode(UmiQualificationField field, UmiReleaseQualificationResult *result)
{
    if (UmiQualificationFieldIs(field, "-")) return UMI_STATUS_OK;
    bool negative = field.length > 0U && field.bytes[0] == '-';
    if (negative) { ++field.bytes; --field.length; }
    uint64_t maximum = negative ? (uint64_t)INT_MAX + 1U : (uint64_t)INT_MAX, value = 0U;
    UmiStatus status = UmiQualificationReadNumber(field, maximum, &value);
    if (status != UMI_STATUS_OK || (negative && value == 0U)) return UMI_STATUS_PARSE_ERROR;
    result->exitCodeKnown = true;
    result->exitCode = negative ? (value == maximum ? INT_MIN : -(int)value) : (int)value;
    return UMI_STATUS_OK;
}
UmiStatus UmiQualificationReadResult(const UmiQualificationField *fields, size_t count,
    UmiReleaseQualificationResult *result, bool *recorded)
{
    if (count == 1U && UmiQualificationFieldIs(fields[0], "missing")) return UMI_STATUS_OK;
    if (count != 5U) return UMI_STATUS_PARSE_ERROR;
    const char *names[] = {"not-run", "passed", "failed", "skipped", "cancelled", "timed-out", "disabled"};
    size_t outcome = 0U;
    while (outcome < sizeof(names) / sizeof(names[0]) && !UmiQualificationFieldIs(fields[0], names[outcome])) ++outcome;
    if (outcome == sizeof(names) / sizeof(names[0])) return UMI_STATUS_PARSE_ERROR;
    result->outcome = (UmiReleaseQualificationOutcome)outcome;
    uint64_t statusValue = 0U;
    UmiStatus status = UmiQualificationReadNumber(fields[1], (uint64_t)UMI_STATUS_BUSY, &statusValue);
    result->status = (UmiStatus)statusValue;
    if (status == UMI_STATUS_OK) status = ExitCode(fields[2], result);
    if (status == UMI_STATUS_OK) status = UmiQualificationReadNumber(fields[3], UINT64_MAX, &result->durationMs);
    if (status == UMI_STATUS_OK) status = UmiQualificationReadDigest(fields[4], false, result->logSha256);
    if (status == UMI_STATUS_OK && UmiQualificationResultValid(result) != UMI_STATUS_OK) status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) *recorded = true;
    return status;
}
