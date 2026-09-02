/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/internal.h
 *
 * PURPOSE:
 *   Provide private string, hashing, indexing and formatting helpers shared only by the workbench layout implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Provide the workbench layout text present operation used by this module and its client
 * applications.
 */
static inline bool umi_workbench_layout_text_present(const char *text)
{
    return text != NULL && text[0] != '\0';
}

/**
 * Provide the workbench layout text equal operation used by this module and its client
 * applications.
 */
static inline bool umi_workbench_layout_text_equal(
    const char *left,
    const char *right)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left == NULL || right == NULL) {
        return left == right;
    }
    return strcmp(left, right) == 0;
}

/**
 * Provide the workbench layout text equal case insensitive operation used by this module
 * and its client applications.
 */
static inline bool umi_workbench_layout_text_equal_case_insensitive(
    const char *left,
    const char *right)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left == NULL || right == NULL) {
        return left == right;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; left[index] != '\0' && right[index] != '\0'; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (tolower((unsigned char)left[index]) !=
            tolower((unsigned char)right[index])) {
            return false;
        }
    }
    return left[index] == right[index];
}

/**
 * Provide the workbench layout text contains case insensitive operation used by this
 * module and its client applications.
 */
static inline bool umi_workbench_layout_text_contains_case_insensitive(
    const char *text,
    const char *needle)
{
    size_t text_length;
    size_t needle_length;
    size_t start;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || needle == NULL) {
        return false;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (needle[0] == '\0') {
        return true;
    }

    text_length = strlen(text);
    needle_length = strlen(needle);
    /* Apply this branch only when its contract condition is satisfied. */
    if (needle_length > text_length) {
        return false;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (start = 0U; start + needle_length <= text_length; ++start) {
        bool matched = true;
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < needle_length; ++index) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (tolower((unsigned char)text[start + index]) !=
                tolower((unsigned char)needle[index])) {
                matched = false;
                break;
            }
        }
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (matched) {
            return true;
        }
    }
    return false;
}

/**
 * Provide the workbench layout copy text operation used by this module and its client
 * applications.
 */
static inline UmiStatus umi_workbench_layout_copy_text(
    char *destination,
    size_t capacity,
    const char *source,
    bool allow_empty)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!allow_empty && source[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/**
 * Provide the workbench layout hash bytes operation used by this module and its client
 * applications.
 */
static inline uint64_t umi_workbench_layout_hash_bytes(
    uint64_t hash,
    const void *data,
    size_t length)
{
    const unsigned char *bytes = (const unsigned char *)data;
    size_t index;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < length; ++index) {
        hash ^= (uint64_t)bytes[index];
        hash *= UMI_WORKBENCH_LAYOUT_FNV_PRIME;
    }
    return hash;
}

/**
 * Provide the workbench layout hash text operation used by this module and its client
 * applications.
 */
static inline uint64_t umi_workbench_layout_hash_text(
    uint64_t hash,
    const char *text)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) {
        const unsigned char marker = 0xFFU;
        return umi_workbench_layout_hash_bytes(hash, &marker, 1U);
    }
    return umi_workbench_layout_hash_bytes(hash, text, strlen(text) + 1U);
}

/**
 * Provide the workbench layout hash u64 operation used by this module and its client
 * applications.
 */
static inline uint64_t umi_workbench_layout_hash_u64(
    uint64_t hash,
    uint64_t value)
{
    return umi_workbench_layout_hash_bytes(hash, &value, sizeof(value));
}

/**
 * Provide the workbench layout hash i32 operation used by this module and its client
 * applications.
 */
static inline uint64_t umi_workbench_layout_hash_i32(
    uint64_t hash,
    int32_t value)
{
    return umi_workbench_layout_hash_bytes(hash, &value, sizeof(value));
}

/**
 * Return the number of records represented by workbench layout hash without changing their
 * state.
 */
static inline uint64_t umi_workbench_layout_hash_size(
    uint64_t hash,
    size_t value)
{
    return umi_workbench_layout_hash_bytes(hash, &value, sizeof(value));
}

/**
 * Provide the workbench layout hash double operation used by this module and its client
 * applications.
 */
static inline uint64_t umi_workbench_layout_hash_double(
    uint64_t hash,
    double value)
{
    return umi_workbench_layout_hash_bytes(hash, &value, sizeof(value));
}

/**
 * Provide the workbench layout compare text operation used by this module and its client
 * applications.
 */
static inline int umi_workbench_layout_compare_text(
    const char *left,
    const char *right)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left == NULL && right == NULL) return 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left == NULL) return -1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (right == NULL) return 1;
    return strcmp(left, right);
}

/**
 * Provide the workbench layout format operation used by this module and its client
 * applications.
 */
static inline UmiStatus umi_workbench_layout_format(
    char *buffer,
    size_t capacity,
    const char *format,
    ...)
{
    va_list arguments;
    int written;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (buffer == NULL || capacity == 0U || format == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    va_start(arguments, format);
    written = vsnprintf(buffer, capacity, format, arguments);
    va_end(arguments);

    /* Apply this branch only when its contract condition is satisfied. */
    if (written < 0) {
        buffer[0] = '\0';
        return UMI_STATUS_INTERNAL_ERROR;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if ((size_t)written >= capacity) {
        buffer[capacity - 1U] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

/**
 * Check that workbench layout index satisfies its contract before another service relies
 * on it.
 */
static inline bool umi_workbench_layout_index_valid(
    size_t index,
    size_t count)
{
    return index != UMI_WORKBENCH_LAYOUT_INDEX_NONE && index < count;
}

/**
 * Provide the workbench layout node id exists operation used by this module and its client
 * applications.
 */
static inline bool umi_workbench_layout_node_id_exists(
    const UmiWorkbenchLayoutDocument *document,
    const char *node_id)
{
    return umi_workbench_layout_document_find_node_index(document, node_id) !=
           UMI_WORKBENCH_LAYOUT_INDEX_NONE;
}

/**
 * Provide the workbench layout clear result message operation used by this module and its
 * client applications.
 */
static inline void umi_workbench_layout_clear_result_message(
    char *message,
    size_t capacity)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (message != NULL && capacity > 0U) {
        message[0] = '\0';
    }
}

#endif
