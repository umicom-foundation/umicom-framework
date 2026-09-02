/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout_data/value_codec.c
 *
 * PURPOSE:
 *   Implement compact field encoding with explicit escaping, bounded parsing and typed numeric accessors for Data Server records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout_data/value_codec.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



/* Check that field name satisfies its contract before another service relies on it. */
static bool field_name_valid(const char *name)
{
    const unsigned char *cursor = (const unsigned char *)name;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (name == NULL || name[0] == '\0') return false;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != 0U) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (!(isalnum(*cursor) != 0 || *cursor == (unsigned char)'_' ||
              *cursor == (unsigned char)'-' ||
              *cursor == (unsigned char)'.')) {
            return false;
        }
        ++cursor;
    }
    return true;
}

/*
 * Initialise workbench layout data field set from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_layout_data_field_set_init(
    UmiWorkbenchLayoutDataFieldSet *fields)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (fields == NULL) return;
    (void)memset(fields, 0, sizeof(*fields));
    fields->structure_size = sizeof(*fields);
}

/* Provide the field index operation used by this module and its client applications. */
static size_t field_index(
    const UmiWorkbenchLayoutDataFieldSet *fields,
    const char *name)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (fields == NULL || name == NULL) {
        return UMI_WORKBENCH_LAYOUT_DATA_INDEX_NONE;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < fields->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(fields->fields[index].name, name) == 0) return index;
    }
    return UMI_WORKBENCH_LAYOUT_DATA_INDEX_NONE;
}

/*
 * Provide the workbench layout data field set put operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_data_field_set_put(
    UmiWorkbenchLayoutDataFieldSet *fields,
    const char *name,
    const char *value)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (fields == NULL || fields->structure_size < sizeof(*fields) ||
        !field_name_valid(name) || value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = field_index(fields, name);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == UMI_WORKBENCH_LAYOUT_DATA_INDEX_NONE) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (fields->count >= 64U) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = fields->count++;
        status = umi_workbench_layout_data_copy_text(
            fields->fields[index].name,
            sizeof(fields->fields[index].name), name, false);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            fields->count--;
            return status;
        }
    }
    return umi_workbench_layout_data_copy_text(
        fields->fields[index].value,
        sizeof(fields->fields[index].value), value, true);
}

/*
 * Provide the workbench layout data field set put u64 operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_data_field_set_put_u64(
    UmiWorkbenchLayoutDataFieldSet *fields,
    const char *name,
    uint64_t value)
{
    char text[32];
    int written = snprintf(text, sizeof(text), "%llu",
                           (unsigned long long)value);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(text)) {
        return UMI_STATUS_INTERNAL_ERROR;
    }
    return umi_workbench_layout_data_field_set_put(fields, name, text);
}

/*
 * Provide the workbench layout data field set put u32 operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_data_field_set_put_u32(
    UmiWorkbenchLayoutDataFieldSet *fields,
    const char *name,
    uint32_t value)
{
    return umi_workbench_layout_data_field_set_put_u64(
        fields, name, (uint64_t)value);
}

/*
 * Provide the workbench layout data field set put bool operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_data_field_set_put_bool(
    UmiWorkbenchLayoutDataFieldSet *fields,
    const char *name,
    bool value)
{
    return umi_workbench_layout_data_field_set_put(
        fields, name, value ? "true" : "false");
}

/*
 * Provide the workbench layout data field set get operation used by this module and its
 * client applications.
 */
const char *umi_workbench_layout_data_field_set_get(
    const UmiWorkbenchLayoutDataFieldSet *fields,
    const char *name)
{
    const size_t index = field_index(fields, name);
    return index != UMI_WORKBENCH_LAYOUT_DATA_INDEX_NONE
        ? fields->fields[index].value : NULL;
}

/*
 * Provide the workbench layout data field set get u64 operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_data_field_set_get_u64(
    const UmiWorkbenchLayoutDataFieldSet *fields,
    const char *name,
    uint64_t *out_value)
{
    const char *text;
    char *end = NULL;
    unsigned long long value;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_value = 0U;
    text = umi_workbench_layout_data_field_set_get(fields, name);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return UMI_STATUS_NOT_FOUND;
    value = strtoull(text, &end, 10);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (end == NULL || *end != '\0') return UMI_STATUS_PARSE_ERROR;
    *out_value = (uint64_t)value;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout data field set get u32 operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_data_field_set_get_u32(
    const UmiWorkbenchLayoutDataFieldSet *fields,
    const char *name,
    uint32_t *out_value)
{
    uint64_t value;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_layout_data_field_set_get_u64(
        fields, name, &value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (value > UINT32_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    *out_value = (uint32_t)value;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout data field set get bool operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_data_field_set_get_bool(
    const UmiWorkbenchLayoutDataFieldSet *fields,
    const char *name,
    bool *out_value)
{
    const char *text;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    text = umi_workbench_layout_data_field_set_get(fields, name);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return UMI_STATUS_NOT_FOUND;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(text, "true") == 0 || strcmp(text, "1") == 0) {
        *out_value = true;
        return UMI_STATUS_OK;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(text, "false") == 0 || strcmp(text, "0") == 0) {
        *out_value = false;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_PARSE_ERROR;
}

/* Provide the unreserved operation used by this module and its client applications. */
static bool unreserved(unsigned char character)
{
    return isalnum(character) != 0 ||
           character == (unsigned char)'-' ||
           character == (unsigned char)'_' ||
           character == (unsigned char)'.' ||
           character == (unsigned char)'~' ||
           character == (unsigned char)' ';
}

/*
 * Provide the workbench layout data value escape operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_data_value_escape(
    const char *text,
    char *buffer,
    size_t capacity,
    size_t *out_required)
{
    static const char hexadecimal[] = "0123456789ABCDEF";
    size_t required = 1U;
    size_t written = 0U;
    const unsigned char *cursor;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || (buffer == NULL && capacity != 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    cursor = (const unsigned char *)text;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != 0U) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (unreserved(*cursor) && *cursor != (unsigned char)'%' &&
            *cursor != (unsigned char)'=' && *cursor != (unsigned char)'\n' &&
            *cursor != (unsigned char)'\r') {
            required += 1U;
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (buffer != NULL && written + 1U < capacity) {
                buffer[written] = (char)*cursor;
            }
            written += 1U;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            required += 3U;
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (buffer != NULL && written + 3U < capacity) {
                buffer[written] = '%';
                buffer[written + 1U] = hexadecimal[*cursor >> 4U];
                buffer[written + 2U] = hexadecimal[*cursor & 0x0FU];
            }
            written += 3U;
        }
        ++cursor;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_required != NULL) *out_required = required;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (buffer == NULL) return UMI_STATUS_OK;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required > capacity) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity > 0U) buffer[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    buffer[written] = '\0';
    return UMI_STATUS_OK;
}

/* Provide the hex value operation used by this module and its client applications. */
static int hex_value(char character)
{
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (character >= '0' && character <= '9') return character - '0';
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (character >= 'a' && character <= 'f') {
        return character - 'a' + 10;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (character >= 'A' && character <= 'F') {
        return character - 'A' + 10;
    }
    return -1;
}

/*
 * Provide the workbench layout data value unescape operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_data_value_unescape(
    const char *text,
    char *buffer,
    size_t capacity,
    size_t *out_required)
{
    size_t source = 0U;
    size_t written = 0U;
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || (buffer == NULL && capacity != 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(text);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (source < length) {
        unsigned char character;
        /* Apply this branch only when its contract condition is satisfied. */
        if (text[source] == '%') {
            int high;
            int low;
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (source + 2U >= length) return UMI_STATUS_PARSE_ERROR;
            high = hex_value(text[source + 1U]);
            low = hex_value(text[source + 2U]);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (high < 0 || low < 0) return UMI_STATUS_PARSE_ERROR;
            character = (unsigned char)((high << 4) | low);
            source += 3U;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            character = (unsigned char)text[source++];
        }
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (buffer != NULL && written + 1U < capacity) {
            buffer[written] = (char)character;
        }
        written += 1U;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_required != NULL) *out_required = written + 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (buffer == NULL) return UMI_STATUS_OK;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written + 1U > capacity) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity > 0U) buffer[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    buffer[written] = '\0';
    return UMI_STATUS_OK;
}

/*
 * Write workbench layout data value in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_workbench_layout_data_value_encode(
    const UmiWorkbenchLayoutDataFieldSet *fields,
    char *buffer,
    size_t capacity,
    size_t *out_required)
{
    size_t required = 1U;
    size_t written = 0U;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (fields == NULL || fields->structure_size < sizeof(*fields) ||
        (buffer == NULL && capacity != 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < fields->count; ++index) {
        size_t escaped_required = 0U;
        UmiStatus status = umi_workbench_layout_data_value_escape(
            fields->fields[index].value, NULL, 0U, &escaped_required);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        required += strlen(fields->fields[index].name) + 1U +
                    escaped_required - 1U + 1U;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_required != NULL) *out_required = required;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (buffer == NULL) return UMI_STATUS_OK;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required > capacity) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity > 0U) buffer[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < fields->count; ++index) {
        const size_t name_length = strlen(fields->fields[index].name);
        size_t escaped_required = 0U;
        UmiStatus status;
        (void)memcpy(buffer + written, fields->fields[index].name,
                     name_length);
        written += name_length;
        buffer[written++] = '=';
        status = umi_workbench_layout_data_value_escape(
            fields->fields[index].value,
            buffer + written, capacity - written, &escaped_required);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        written += escaped_required - 1U;
        buffer[written++] = '\n';
    }
    buffer[written] = '\0';
    return UMI_STATUS_OK;
}

/*
 * Read workbench layout data value into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_workbench_layout_data_value_decode(
    const char *value,
    UmiWorkbenchLayoutDataFieldSet *out_fields)
{
    const char *cursor;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || out_fields == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_layout_data_field_set_init(out_fields);
    cursor = value;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0') {
        const char *equals = strchr(cursor, '=');
        const char *newline = strchr(cursor, '\n');
        char name[64];
        char escaped[UMI_WORKBENCH_LAYOUT_DATA_VALUE_CAPACITY];
        char decoded[UMI_WORKBENCH_LAYOUT_DATA_VALUE_CAPACITY];
        size_t name_length;
        size_t value_length;
        UmiStatus status;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (equals == NULL || (newline != NULL && equals > newline)) {
            return UMI_STATUS_PARSE_ERROR;
        }
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (newline == NULL) newline = cursor + strlen(cursor);
        name_length = (size_t)(equals - cursor);
        value_length = (size_t)(newline - equals - 1);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (name_length == 0U || name_length >= sizeof(name) ||
            value_length >= sizeof(escaped)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        (void)memcpy(name, cursor, name_length);
        name[name_length] = '\0';
        (void)memcpy(escaped, equals + 1, value_length);
        escaped[value_length] = '\0';
        status = umi_workbench_layout_data_value_unescape(
            escaped, decoded, sizeof(decoded), NULL);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = umi_workbench_layout_data_field_set_put(
            out_fields, name, decoded);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        cursor = *newline == '\0' ? newline : newline + 1;
    }
    return UMI_STATUS_OK;
}
