/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/source_location.c
 *
 * PURPOSE:
 *   Implement validation, comparison and display formatting for canonical
 *   source locations without assuming a URI scheme or text widget.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/source_location.h"

#include <stdio.h>
#include <string.h>

static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    size_t length;

    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_source_location_initialize(
    UmiEditorSourceLocation *location,
    const char *uri,
    uint64_t line,
    uint64_t column)
{
    UmiStatus status;

    if (location == NULL || uri == NULL || uri[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(location, 0, sizeof(*location));
    location->struct_size = (uint32_t)sizeof(*location);
    location->api_version = UMI_EDITOR_SOURCE_LOCATION_API_VERSION;
    location->kind = UMI_EDITOR_SOURCE_LOCATION_FILE;
    location->line = line;
    location->column = column;
    location->end_line = line;
    location->end_column = column;
    status = copy_text(location->uri, sizeof(location->uri), uri);
    if (status != UMI_STATUS_OK) {
        (void)memset(location, 0, sizeof(*location));
    }
    return status;
}

UmiStatus umi_editor_source_location_validate(
    const UmiEditorSourceLocation *location)
{
    if (location == NULL ||
        location->struct_size != (uint32_t)sizeof(*location) ||
        location->api_version != UMI_EDITOR_SOURCE_LOCATION_API_VERSION ||
        location->uri[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (location->kind < UMI_EDITOR_SOURCE_LOCATION_UNKNOWN ||
        location->kind > UMI_EDITOR_SOURCE_LOCATION_DIAGNOSTIC ||
        location->end_line < location->line ||
        (location->end_line == location->line &&
         location->end_column < location->column) ||
        location->end_byte_offset < location->byte_offset) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

int umi_editor_source_location_same_position(
    const UmiEditorSourceLocation *left,
    const UmiEditorSourceLocation *right)
{
    if (umi_editor_source_location_validate(left) != UMI_STATUS_OK ||
        umi_editor_source_location_validate(right) != UMI_STATUS_OK) {
        return 0;
    }
    return strcmp(left->uri, right->uri) == 0 &&
           left->line == right->line &&
           left->column == right->column;
}

int umi_editor_source_location_compare(
    const UmiEditorSourceLocation *left,
    const UmiEditorSourceLocation *right)
{
    int uri_order;

    if (left == right) return 0;
    if (left == NULL) return -1;
    if (right == NULL) return 1;
    uri_order = strcmp(left->uri, right->uri);
    if (uri_order != 0) return uri_order;
    if (left->line < right->line) return -1;
    if (left->line > right->line) return 1;
    if (left->column < right->column) return -1;
    if (left->column > right->column) return 1;
    if (left->end_line < right->end_line) return -1;
    if (left->end_line > right->end_line) return 1;
    if (left->end_column < right->end_column) return -1;
    if (left->end_column > right->end_column) return 1;
    return 0;
}

UmiStatus umi_editor_source_location_format(
    const UmiEditorSourceLocation *location,
    char *out_text,
    size_t out_capacity)
{
    int written;

    if (umi_editor_source_location_validate(location) != UMI_STATUS_OK ||
        out_text == NULL || out_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(out_text,
                       out_capacity,
                       "%s:%llu:%llu",
                       location->uri,
                       (unsigned long long)(location->line + 1U),
                       (unsigned long long)(location->column + 1U));
    if (written < 0) return UMI_STATUS_INTERNAL_ERROR;
    if ((size_t)written >= out_capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}
