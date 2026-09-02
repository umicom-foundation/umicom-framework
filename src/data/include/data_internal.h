/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/include/data_internal.h
 *
 * PURPOSE:
 *   Provide private key formatting, string duplication and hexadecimal encoding helpers shared by Data Server adapters and repositories.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_INTERNAL_H
#define UMICOM_DATA_INTERNAL_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/base/status.h"

/**
 * Provide the data strdup operation used by this module and its client applications.
 */
static inline char *umi_data_strdup(const char *value)
{
    size_t length;
    char *copy;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) value = "";
    length = strlen(value);
    copy = (char *)malloc(length + 1U);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (copy == NULL) return NULL;
    (void)memcpy(copy, value, length + 1U);
    return copy;
}

/**
 * Provide the data key operation used by this module and its client applications.
 */
static inline UmiStatus umi_data_key(char *out_key,
                                     size_t capacity,
                                     const char *prefix,
                                     const char *key)
{
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_key == NULL || capacity == 0U || prefix == NULL || key == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(out_key, capacity, "%s%s", prefix, key);
    return written < 0 || (size_t)written >= capacity
        ? UMI_STATUS_CAPACITY_EXCEEDED
        : UMI_STATUS_OK;
}

/**
 * Provide the hex digit operation used by this module and its client applications.
 */
static inline char umi_hex_digit(unsigned value)
{
    return (char)(value < 10U ? ('0' + value) : ('a' + (value - 10U)));
}

/**
 * Provide the hex value operation used by this module and its client applications.
 */
static inline int umi_hex_value(char value)
{
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (value >= '0' && value <= '9') return value - '0';
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (value >= 'a' && value <= 'f') return value - 'a' + 10;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (value >= 'A' && value <= 'F') return value - 'A' + 10;
    return -1;
}

/**
 * Write hex in its stable representation and report capacity or input failures to the
 * caller.
 */
static inline UmiStatus umi_hex_encode(const void *bytes,
                                       size_t size,
                                       char **out_text)
{
    const unsigned char *input = (const unsigned char *)bytes;
    char *text;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_text == NULL || (size > 0U && bytes == NULL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_text = NULL;
    text = (char *)malloc(size * 2U + 1U);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < size; ++index) {
        text[index * 2U] = umi_hex_digit((unsigned)(input[index] >> 4U));
        text[index * 2U + 1U] = umi_hex_digit((unsigned)(input[index] & 0x0FU));
    }
    text[size * 2U] = '\0';
    *out_text = text;
    return UMI_STATUS_OK;
}

/**
 * Read hex into validated module state and return a status when input cannot be used.
 */
static inline UmiStatus umi_hex_decode(const char *text,
                                       unsigned char **out_bytes,
                                       size_t *out_size)
{
    size_t length;
    size_t index;
    unsigned char *bytes;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_bytes == NULL || out_size == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_bytes = NULL;
    *out_size = 0U;
    length = strlen(text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if ((length % 2U) != 0U) return UMI_STATUS_PARSE_ERROR;
    bytes = (unsigned char *)malloc(length / 2U + 1U);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bytes == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < length; index += 2U) {
        int high = umi_hex_value(text[index]);
        int low = umi_hex_value(text[index + 1U]);
        /* Apply this branch only when its contract condition is satisfied. */
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
