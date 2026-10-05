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
#include "../base/value_archive_internal.h"

#include <stdio.h>
#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Initialise editor source location from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_source_location_initialize(
    UmiEditorSourceLocation *location,
    const char *uri,
    uint64_t line,
    uint64_t column)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)memset(location, 0, sizeof(*location));
    }
    return status;
}

/*
 * Check that editor source location satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_editor_source_location_validate(
    const UmiEditorSourceLocation *location)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (location == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(location->uri, '\0', sizeof(location->uri)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(location->label, '\0', sizeof(location->label)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(location->symbol_id, '\0', sizeof(location->symbol_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(location->preview, '\0', sizeof(location->preview)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (location == NULL ||
        location->struct_size != (uint32_t)sizeof(*location) ||
        location->api_version != UMI_EDITOR_SOURCE_LOCATION_API_VERSION ||
        location->uri[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
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

/*
 * Provide the editor source location same position operation used by this module and its
 * client applications.
 */
int umi_editor_source_location_same_position(
    const UmiEditorSourceLocation *left,
    const UmiEditorSourceLocation *right)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_editor_source_location_validate(left) != UMI_STATUS_OK ||
        umi_editor_source_location_validate(right) != UMI_STATUS_OK) {
        return 0;
    }
    return strcmp(left->uri, right->uri) == 0 &&
           left->line == right->line &&
           left->column == right->column;
}

/*
 * Provide the editor source location compare operation used by this module and its client
 * applications.
 */
int umi_editor_source_location_compare(
    const UmiEditorSourceLocation *left,
    const UmiEditorSourceLocation *right)
{
    int uri_order;

    /* Apply this branch only when its contract condition is satisfied. */
    if (left == right) return 0;
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
    uri_order = strcmp(left->uri, right->uri);
    /* Apply this branch only when its contract condition is satisfied. */
    if (uri_order != 0) return uri_order;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->line < right->line) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->line > right->line) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->column < right->column) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->column > right->column) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->end_line < right->end_line) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->end_line > right->end_line) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->end_column < right->end_column) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->end_column > right->end_column) return 1;
    return 0;
}

/*
 * Provide the editor source location format operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_source_location_format(
    const UmiEditorSourceLocation *location,
    char *out_text,
    size_t out_capacity)
{
    int written;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (written < 0) return UMI_STATUS_INTERNAL_ERROR;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if ((size_t)written >= out_capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiEditorSourceLocationArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xd5eb4daf024cb69d);
    schema = (schema ^ (uint64_t)sizeof(((UmiEditorSourceLocation *)0)->uri)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiEditorSourceLocation *)0)->label)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiEditorSourceLocation *)0)->symbol_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiEditorSourceLocation *)0)->preview)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiEditorSourceLocationArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U + sizeof(((UmiEditorSourceLocation *)0)->uri) - 1U +
        8U + sizeof(((UmiEditorSourceLocation *)0)->label) - 1U +
        8U + sizeof(((UmiEditorSourceLocation *)0)->symbol_id) - 1U +
        8U + sizeof(((UmiEditorSourceLocation *)0)->preview) - 1U;
}
static void UmiEditorSourceLocationArchiveWrite(UmiArchiveWriter *writer, const UmiEditorSourceLocation *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->api_version);
    UmiArchiveWriteSigned(writer, (int64_t)value->kind);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->line);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->column);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->end_line);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->end_column);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->byte_offset);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->end_byte_offset);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->document_revision);
    UmiArchiveWriteSigned(writer, (int64_t)value->score);
    UmiArchiveWriteText(writer, value->uri, sizeof(value->uri));
    UmiArchiveWriteText(writer, value->label, sizeof(value->label));
    UmiArchiveWriteText(writer, value->symbol_id, sizeof(value->symbol_id));
    UmiArchiveWriteText(writer, value->preview, sizeof(value->preview));
}
static void UmiEditorSourceLocationArchiveRead(UmiArchiveReader *reader, UmiEditorSourceLocation *value)
{
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->kind = (UmiEditorSourceLocationKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->line = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->column = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->end_line = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->end_column = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->byte_offset = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->end_byte_offset = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->document_revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->score = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    UmiArchiveReadText(reader, value->uri, sizeof(value->uri));
    UmiArchiveReadText(reader, value->label, sizeof(value->label));
    UmiArchiveReadText(reader, value->symbol_id, sizeof(value->symbol_id));
    UmiArchiveReadText(reader, value->preview, sizeof(value->preview));
}
static UmiStatus UmiEditorSourceLocationArchiveValidate(const UmiEditorSourceLocation *value)
{
    return umi_editor_source_location_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_editor_source_location_archive_encode, umi_editor_source_location_archive_decode,
    UmiEditorSourceLocation, UmiEditorSourceLocationArchiveSchema, UmiEditorSourceLocationArchiveBound, UmiEditorSourceLocationArchiveWrite, UmiEditorSourceLocationArchiveRead, UmiEditorSourceLocationArchiveValidate)
