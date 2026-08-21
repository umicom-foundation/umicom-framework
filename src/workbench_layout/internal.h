/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/internal.h
 *
 * PURPOSE:
 *   Provide private string, hashing, indexing and formatting helpers shared only by the workbench layout implementation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_INTERNAL_H
#define UMICOM_WORKBENCH_LAYOUT_INTERNAL_H

#include <ctype.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/workbench_layout/workbench_layout.h"

#define UMI_WORKBENCH_LAYOUT_FNV_OFFSET UINT64_C(14695981039346656037)
#define UMI_WORKBENCH_LAYOUT_FNV_PRIME UINT64_C(1099511628211)

static inline bool umi_workbench_layout_text_present(const char *text)
{
    return text != NULL && text[0] != '\0';
}

static inline bool umi_workbench_layout_text_equal(
    const char *left,
    const char *right)
{
    if (left == NULL || right == NULL) {
        return left == right;
    }
    return strcmp(left, right) == 0;
}

static inline bool umi_workbench_layout_text_equal_case_insensitive(
    const char *left,
    const char *right)
{
    size_t index;
    if (left == NULL || right == NULL) {
        return left == right;
    }
    for (index = 0U; left[index] != '\0' && right[index] != '\0'; ++index) {
        if (tolower((unsigned char)left[index]) !=
            tolower((unsigned char)right[index])) {
            return false;
        }
    }
    return left[index] == right[index];
}

static inline bool umi_workbench_layout_text_contains_case_insensitive(
    const char *text,
    const char *needle)
{
    size_t text_length;
    size_t needle_length;
    size_t start;
    size_t index;

    if (text == NULL || needle == NULL) {
        return false;
    }
    if (needle[0] == '\0') {
        return true;
    }

    text_length = strlen(text);
    needle_length = strlen(needle);
    if (needle_length > text_length) {
        return false;
    }

    for (start = 0U; start + needle_length <= text_length; ++start) {
        bool matched = true;
        for (index = 0U; index < needle_length; ++index) {
            if (tolower((unsigned char)text[start + index]) !=
                tolower((unsigned char)needle[index])) {
                matched = false;
                break;
            }
        }
        if (matched) {
            return true;
        }
    }
    return false;
}

static inline UmiStatus umi_workbench_layout_copy_text(
    char *destination,
    size_t capacity,
    const char *source,
    bool allow_empty)
{
    size_t length;

    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!allow_empty && source[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(source);
    if (length >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

static inline uint64_t umi_workbench_layout_hash_bytes(
    uint64_t hash,
    const void *data,
    size_t length)
{
    const unsigned char *bytes = (const unsigned char *)data;
    size_t index;

    for (index = 0U; index < length; ++index) {
        hash ^= (uint64_t)bytes[index];
        hash *= UMI_WORKBENCH_LAYOUT_FNV_PRIME;
    }
    return hash;
}

static inline uint64_t umi_workbench_layout_hash_text(
    uint64_t hash,
    const char *text)
{
    if (text == NULL) {
        const unsigned char marker = 0xFFU;
        return umi_workbench_layout_hash_bytes(hash, &marker, 1U);
    }
    return umi_workbench_layout_hash_bytes(hash, text, strlen(text) + 1U);
}

static inline uint64_t umi_workbench_layout_hash_u64(
    uint64_t hash,
    uint64_t value)
{
    return umi_workbench_layout_hash_bytes(hash, &value, sizeof(value));
}

static inline uint64_t umi_workbench_layout_hash_i32(
    uint64_t hash,
    int32_t value)
{
    return umi_workbench_layout_hash_bytes(hash, &value, sizeof(value));
}

static inline uint64_t umi_workbench_layout_hash_size(
    uint64_t hash,
    size_t value)
{
    return umi_workbench_layout_hash_bytes(hash, &value, sizeof(value));
}

static inline uint64_t umi_workbench_layout_hash_double(
    uint64_t hash,
    double value)
{
    return umi_workbench_layout_hash_bytes(hash, &value, sizeof(value));
}

static inline int umi_workbench_layout_compare_text(
    const char *left,
    const char *right)
{
    if (left == NULL && right == NULL) return 0;
    if (left == NULL) return -1;
    if (right == NULL) return 1;
    return strcmp(left, right);
}

static inline UmiStatus umi_workbench_layout_format(
    char *buffer,
    size_t capacity,
    const char *format,
    ...)
{
    va_list arguments;
    int written;

    if (buffer == NULL || capacity == 0U || format == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    va_start(arguments, format);
    written = vsnprintf(buffer, capacity, format, arguments);
    va_end(arguments);

    if (written < 0) {
        buffer[0] = '\0';
        return UMI_STATUS_INTERNAL_ERROR;
    }
    if ((size_t)written >= capacity) {
        buffer[capacity - 1U] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

static inline bool umi_workbench_layout_index_valid(
    size_t index,
    size_t count)
{
    return index != UMI_WORKBENCH_LAYOUT_INDEX_NONE && index < count;
}

static inline bool umi_workbench_layout_node_id_exists(
    const UmiWorkbenchLayoutDocument *document,
    const char *node_id)
{
    return umi_workbench_layout_document_find_node_index(document, node_id) !=
           UMI_WORKBENCH_LAYOUT_INDEX_NONE;
}

static inline void umi_workbench_layout_clear_result_message(
    char *message,
    size_t capacity)
{
    if (message != NULL && capacity > 0U) {
        message[0] = '\0';
    }
}

#endif
