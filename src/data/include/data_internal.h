/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/include/data_internal.h
 *
 * PURPOSE:
 *   Provide private key formatting, string duplication and hexadecimal encoding helpers shared by Data Server adapters and repositories.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_INTERNAL_H
#define UMICOM_DATA_INTERNAL_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/base/status.h"

static inline char *umi_data_strdup(const char *value)
{
    size_t length;
    char *copy;
    if (value == NULL) value = "";
    length = strlen(value);
    copy = (char *)malloc(length + 1U);
    if (copy == NULL) return NULL;
    (void)memcpy(copy, value, length + 1U);
    return copy;
}

static inline UmiStatus umi_data_key(char *out_key,
                                     size_t capacity,
                                     const char *prefix,
                                     const char *key)
{
    int written;
    if (out_key == NULL || capacity == 0U || prefix == NULL || key == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(out_key, capacity, "%s%s", prefix, key);
    return written < 0 || (size_t)written >= capacity
        ? UMI_STATUS_CAPACITY_EXCEEDED
        : UMI_STATUS_OK;
}

static inline char umi_hex_digit(unsigned value)
{
    return (char)(value < 10U ? ('0' + value) : ('a' + (value - 10U)));
}

static inline int umi_hex_value(char value)
{
    if (value >= '0' && value <= '9') return value - '0';
    if (value >= 'a' && value <= 'f') return value - 'a' + 10;
    if (value >= 'A' && value <= 'F') return value - 'A' + 10;
    return -1;
}

static inline UmiStatus umi_hex_encode(const void *bytes,
                                       size_t size,
                                       char **out_text)
{
    const unsigned char *input = (const unsigned char *)bytes;
    char *text;
    size_t index;
    if (out_text == NULL || (size > 0U && bytes == NULL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_text = NULL;
    text = (char *)malloc(size * 2U + 1U);
    if (text == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    for (index = 0U; index < size; ++index) {
        text[index * 2U] = umi_hex_digit((unsigned)(input[index] >> 4U));
        text[index * 2U + 1U] = umi_hex_digit((unsigned)(input[index] & 0x0FU));
    }
    text[size * 2U] = '\0';
    *out_text = text;
    return UMI_STATUS_OK;
}

static inline UmiStatus umi_hex_decode(const char *text,
                                       unsigned char **out_bytes,
                                       size_t *out_size)
{
    size_t length;
    size_t index;
    unsigned char *bytes;
    if (text == NULL || out_bytes == NULL || out_size == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_bytes = NULL;
    *out_size = 0U;
    length = strlen(text);
    if ((length % 2U) != 0U) return UMI_STATUS_PARSE_ERROR;
    bytes = (unsigned char *)malloc(length / 2U + 1U);
    if (bytes == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    for (index = 0U; index < length; index += 2U) {
        int high = umi_hex_value(text[index]);
        int low = umi_hex_value(text[index + 1U]);
        if (high < 0 || low < 0) {
            free(bytes);
            return UMI_STATUS_PARSE_ERROR;
        }
        bytes[index / 2U] = (unsigned char)((high << 4) | low);
    }
    bytes[length / 2U] = '\0';
    *out_bytes = bytes;
    *out_size = length / 2U;
    return UMI_STATUS_OK;
}

#endif
