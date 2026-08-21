/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout_data/value_codec.c
 *
 * PURPOSE:
 *   Implement compact field encoding with explicit escaping, bounded parsing and typed numeric accessors for Data Server records.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout_data/value_codec.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



static bool field_name_valid(const char *name)
{
    const unsigned char *cursor = (const unsigned char *)name;
    if (name == NULL || name[0] == '\0') return false;
    while (*cursor != 0U) {
        if (!(isalnum(*cursor) != 0 || *cursor == (unsigned char)'_' ||
              *cursor == (unsigned char)'-' ||
              *cursor == (unsigned char)'.')) {
            return false;
        }
        ++cursor;
    }
    return true;
}

void umi_workbench_layout_data_field_set_init(
    UmiWorkbenchLayoutDataFieldSet *fields)
{
    if (fields == NULL) return;
    (void)memset(fields, 0, sizeof(*fields));
    fields->structure_size = sizeof(*fields);
}

static size_t field_index(
    const UmiWorkbenchLayoutDataFieldSet *fields,
    const char *name)
{
    size_t index;
    if (fields == NULL || name == NULL) {
        return UMI_WORKBENCH_LAYOUT_DATA_INDEX_NONE;
    }
    for (index = 0U; index < fields->count; ++index) {
        if (strcmp(fields->fields[index].name, name) == 0) return index;
    }
    return UMI_WORKBENCH_LAYOUT_DATA_INDEX_NONE;
}

UmiStatus umi_workbench_layout_data_field_set_put(
    UmiWorkbenchLayoutDataFieldSet *fields,
    const char *name,
    const char *value)
{
    size_t index;
    UmiStatus status;
    if (fields == NULL || fields->structure_size < sizeof(*fields) ||
        !field_name_valid(name) || value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = field_index(fields, name);
    if (index == UMI_WORKBENCH_LAYOUT_DATA_INDEX_NONE) {
        if (fields->count >= 64U) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = fields->count++;
        status = umi_workbench_layout_data_copy_text(
            fields->fields[index].name,
            sizeof(fields->fields[index].name), name, false);
        if (status != UMI_STATUS_OK) {
            fields->count--;
            return status;
        }
    }
    return umi_workbench_layout_data_copy_text(
        fields->fields[index].value,
        sizeof(fields->fields[index].value), value, true);
}

UmiStatus umi_workbench_layout_data_field_set_put_u64(
    UmiWorkbenchLayoutDataFieldSet *fields,
    const char *name,
    uint64_t value)
{
    char text[32];
    int written = snprintf(text, sizeof(text), "%llu",
                           (unsigned long long)value);
    if (written < 0 || (size_t)written >= sizeof(text)) {
        return UMI_STATUS_INTERNAL_ERROR;
    }
    return umi_workbench_layout_data_field_set_put(fields, name, text);
}

UmiStatus umi_workbench_layout_data_field_set_put_u32(
    UmiWorkbenchLayoutDataFieldSet *fields,
    const char *name,
    uint32_t value)
{
    return umi_workbench_layout_data_field_set_put_u64(
        fields, name, (uint64_t)value);
}

UmiStatus umi_workbench_layout_data_field_set_put_bool(
    UmiWorkbenchLayoutDataFieldSet *fields,
    const char *name,
    bool value)
{
    return umi_workbench_layout_data_field_set_put(
        fields, name, value ? "true" : "false");
}

const char *umi_workbench_layout_data_field_set_get(
    const UmiWorkbenchLayoutDataFieldSet *fields,
    const char *name)
{
    const size_t index = field_index(fields, name);
    return index != UMI_WORKBENCH_LAYOUT_DATA_INDEX_NONE
        ? fields->fields[index].value : NULL;
}

UmiStatus umi_workbench_layout_data_field_set_get_u64(
    const UmiWorkbenchLayoutDataFieldSet *fields,
    const char *name,
    uint64_t *out_value)
{
    const char *text;
    char *end = NULL;
    unsigned long long value;
    if (out_value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_value = 0U;
    text = umi_workbench_layout_data_field_set_get(fields, name);
    if (text == NULL) return UMI_STATUS_NOT_FOUND;
    value = strtoull(text, &end, 10);
    if (end == NULL || *end != '\0') return UMI_STATUS_PARSE_ERROR;
    *out_value = (uint64_t)value;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_data_field_set_get_u32(
    const UmiWorkbenchLayoutDataFieldSet *fields,
    const char *name,
    uint32_t *out_value)
{
    uint64_t value;
    UmiStatus status;
    if (out_value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_layout_data_field_set_get_u64(
        fields, name, &value);
    if (status != UMI_STATUS_OK) return status;
    if (value > UINT32_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    *out_value = (uint32_t)value;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_data_field_set_get_bool(
    const UmiWorkbenchLayoutDataFieldSet *fields,
    const char *name,
    bool *out_value)
{
    const char *text;
    if (out_value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    text = umi_workbench_layout_data_field_set_get(fields, name);
    if (text == NULL) return UMI_STATUS_NOT_FOUND;
    if (strcmp(text, "true") == 0 || strcmp(text, "1") == 0) {
        *out_value = true;
        return UMI_STATUS_OK;
    }
    if (strcmp(text, "false") == 0 || strcmp(text, "0") == 0) {
        *out_value = false;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_PARSE_ERROR;
}

static bool unreserved(unsigned char character)
{
    return isalnum(character) != 0 ||
           character == (unsigned char)'-' ||
           character == (unsigned char)'_' ||
           character == (unsigned char)'.' ||
           character == (unsigned char)'~' ||
           character == (unsigned char)' ';
}

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
    if (text == NULL || (buffer == NULL && capacity != 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    cursor = (const unsigned char *)text;
    while (*cursor != 0U) {
        if (unreserved(*cursor) && *cursor != (unsigned char)'%' &&
            *cursor != (unsigned char)'=' && *cursor != (unsigned char)'\n' &&
            *cursor != (unsigned char)'\r') {
            required += 1U;
            if (buffer != NULL && written + 1U < capacity) {
                buffer[written] = (char)*cursor;
            }
            written += 1U;
        } else {
            required += 3U;
            if (buffer != NULL && written + 3U < capacity) {
                buffer[written] = '%';
                buffer[written + 1U] = hexadecimal[*cursor >> 4U];
                buffer[written + 2U] = hexadecimal[*cursor & 0x0FU];
            }
            written += 3U;
        }
        ++cursor;
    }
    if (out_required != NULL) *out_required = required;
    if (buffer == NULL) return UMI_STATUS_OK;
    if (required > capacity) {
        if (capacity > 0U) buffer[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    buffer[written] = '\0';
    return UMI_STATUS_OK;
}

static int hex_value(char character)
{
    if (character >= '0' && character <= '9') return character - '0';
    if (character >= 'a' && character <= 'f') {
        return character - 'a' + 10;
    }
    if (character >= 'A' && character <= 'F') {
        return character - 'A' + 10;
    }
    return -1;
}

UmiStatus umi_workbench_layout_data_value_unescape(
    const char *text,
    char *buffer,
    size_t capacity,
    size_t *out_required)
{
    size_t source = 0U;
    size_t written = 0U;
    size_t length;
    if (text == NULL || (buffer == NULL && capacity != 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(text);
    while (source < length) {
        unsigned char character;
        if (text[source] == '%') {
            int high;
            int low;
            if (source + 2U >= length) return UMI_STATUS_PARSE_ERROR;
            high = hex_value(text[source + 1U]);
            low = hex_value(text[source + 2U]);
            if (high < 0 || low < 0) return UMI_STATUS_PARSE_ERROR;
            character = (unsigned char)((high << 4) | low);
            source += 3U;
        } else {
            character = (unsigned char)text[source++];
        }
        if (buffer != NULL && written + 1U < capacity) {
            buffer[written] = (char)character;
        }
        written += 1U;
    }
    if (out_required != NULL) *out_required = written + 1U;
    if (buffer == NULL) return UMI_STATUS_OK;
    if (written + 1U > capacity) {
        if (capacity > 0U) buffer[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    buffer[written] = '\0';
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_data_value_encode(
    const UmiWorkbenchLayoutDataFieldSet *fields,
    char *buffer,
    size_t capacity,
    size_t *out_required)
{
    size_t required = 1U;
    size_t written = 0U;
    size_t index;
    if (fields == NULL || fields->structure_size < sizeof(*fields) ||
        (buffer == NULL && capacity != 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < fields->count; ++index) {
        size_t escaped_required = 0U;
        UmiStatus status = umi_workbench_layout_data_value_escape(
            fields->fields[index].value, NULL, 0U, &escaped_required);
        if (status != UMI_STATUS_OK) return status;
        required += strlen(fields->fields[index].name) + 1U +
                    escaped_required - 1U + 1U;
    }
    if (out_required != NULL) *out_required = required;
    if (buffer == NULL) return UMI_STATUS_OK;
    if (required > capacity) {
        if (capacity > 0U) buffer[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
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
        if (status != UMI_STATUS_OK) return status;
        written += escaped_required - 1U;
        buffer[written++] = '\n';
    }
    buffer[written] = '\0';
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_data_value_decode(
    const char *value,
    UmiWorkbenchLayoutDataFieldSet *out_fields)
{
    const char *cursor;
    if (value == NULL || out_fields == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_layout_data_field_set_init(out_fields);
    cursor = value;
    while (*cursor != '\0') {
        const char *equals = strchr(cursor, '=');
        const char *newline = strchr(cursor, '\n');
        char name[64];
        char escaped[UMI_WORKBENCH_LAYOUT_DATA_VALUE_CAPACITY];
        char decoded[UMI_WORKBENCH_LAYOUT_DATA_VALUE_CAPACITY];
        size_t name_length;
        size_t value_length;
        UmiStatus status;
        if (equals == NULL || (newline != NULL && equals > newline)) {
            return UMI_STATUS_PARSE_ERROR;
        }
        if (newline == NULL) newline = cursor + strlen(cursor);
        name_length = (size_t)(equals - cursor);
        value_length = (size_t)(newline - equals - 1);
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
        if (status != UMI_STATUS_OK) return status;
        status = umi_workbench_layout_data_field_set_put(
            out_fields, name, decoded);
        if (status != UMI_STATUS_OK) return status;
        cursor = *newline == '\0' ? newline : newline + 1;
    }
    return UMI_STATUS_OK;
}
