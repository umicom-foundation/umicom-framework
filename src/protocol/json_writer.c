/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/json_writer.c
 *
 * PURPOSE:
 *   Emit complete bounded JSON values with sticky errors and validated Unicode.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/protocol/json_writer.h"
#include "umicom/base/utf8.h"
#include <stdio.h>
#include <string.h>

static UmiStatus Admit(UmiJsonWriter *writer, const char *text)
{
    if (writer == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (writer->status != UMI_STATUS_OK) return writer->status;
    if (text == NULL || writer->buffer == NULL || writer->capacity == 0U ||
        writer->length >= writer->capacity)
        return writer->status = UMI_STATUS_INVALID_ARGUMENT;
    if (!UmiUtf8Validate((const unsigned char *)text, strlen(text), NULL))
        return writer->status = UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
void UmiJsonWriterInit(UmiJsonWriter *writer, char *buffer, size_t capacity)
{
    if (writer == NULL) return;
    *writer = (UmiJsonWriter){buffer, capacity, 0U,
        buffer != NULL && capacity != 0U ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT};
    if (buffer != NULL && capacity != 0U) buffer[0] = '\0';
}
UmiStatus UmiJsonWriterRaw(UmiJsonWriter *writer, const char *text)
{
    UmiStatus status = Admit(writer, text);
    if (status != UMI_STATUS_OK) return status;
    size_t length = strlen(text);
    /* Subtract first: length + requested + terminator could overflow size_t. */
    if (length >= writer->capacity - writer->length)
        return writer->status = UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(writer->buffer + writer->length, text, length + 1U);
    writer->length += length;
    return UMI_STATUS_OK;
}
UmiStatus UmiJsonWriterString(UmiJsonWriter *writer, const char *text)
{
    UmiStatus status = Admit(writer, text);
    if (status != UMI_STATUS_OK) return status;
    size_t remaining = writer->capacity - writer->length - 1U;
    if (remaining < 2U) return writer->status = UMI_STATUS_CAPACITY_EXCEEDED;
    remaining -= 2U; /* opening and closing quotes */
    for (const unsigned char *p = (const unsigned char *)text; *p != 0U; ++p) {
        size_t needed = *p == '"' || *p == '\\' || *p == '\n' || *p == '\r' || *p == '\t' ? 2U :
            *p < 0x20U ? 6U : 1U;
        if (needed > remaining) return writer->status = UMI_STATUS_CAPACITY_EXCEEDED;
        remaining -= needed;
    }
    /* Only write after the entire value fits. No allocation or fallible operation
     * remains, so a refused value cannot leave an unterminated JSON string. */
    char *out = writer->buffer + writer->length;
    *out++ = '"';
    static const char hex[] = "0123456789abcdef";
    for (const unsigned char *p = (const unsigned char *)text; *p != 0U; ++p) {
        if (*p == '"' || *p == '\\') { *out++ = '\\'; *out++ = (char)*p; }
        else if (*p == '\n' || *p == '\r' || *p == '\t') {
            *out++ = '\\'; *out++ = *p == '\n' ? 'n' : *p == '\r' ? 'r' : 't';
        } else if (*p < 0x20U) {
            *out++ = '\\'; *out++ = 'u'; *out++ = '0'; *out++ = '0';
            *out++ = hex[*p >> 4U]; *out++ = hex[*p & 15U];
        } else *out++ = (char)*p;
    }
    *out++ = '"'; *out = '\0';
    writer->length = (size_t)(out - writer->buffer);
    return UMI_STATUS_OK;
}
UmiStatus UmiJsonWriterUInt64(UmiJsonWriter *writer, uint64_t value)
{
    char digits[32];
    int count = snprintf(digits, sizeof digits, "%llu", (unsigned long long)value);
    if (count < 0 || (size_t)count >= sizeof digits) {
        if (writer != NULL && writer->status == UMI_STATUS_OK) writer->status = UMI_STATUS_INTERNAL_ERROR;
        return writer != NULL ? writer->status : UMI_STATUS_INVALID_ARGUMENT;
    }
    return UmiJsonWriterRaw(writer, digits);
}
UmiStatus UmiJsonWriterInt64(UmiJsonWriter *writer, int64_t value)
{
    char digits[32];
    int count = snprintf(digits, sizeof digits, "%lld", (long long)value);
    if (count < 0 || (size_t)count >= sizeof digits) {
        if (writer != NULL && writer->status == UMI_STATUS_OK) writer->status = UMI_STATUS_INTERNAL_ERROR;
        return writer != NULL ? writer->status : UMI_STATUS_INVALID_ARGUMENT;
    }
    return UmiJsonWriterRaw(writer, digits);
}
UmiStatus UmiJsonWriterBool(UmiJsonWriter *writer, int value)
{ return UmiJsonWriterRaw(writer, value ? "true" : "false"); }
