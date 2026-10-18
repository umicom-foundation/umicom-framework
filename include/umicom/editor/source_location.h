/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/source_location.h
 *
 * PURPOSE:
 *   Define a reusable, toolkit-neutral source location shared by navigation,
 *   symbols, references, bookmarks, diagnostics and edit history services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_SOURCE_LOCATION_H
#define UMICOM_EDITOR_SOURCE_LOCATION_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_SOURCE_LOCATION_API_VERSION 1U
#define UMI_EDITOR_SOURCE_URI_CAPACITY 1024U
#define UMI_EDITOR_SOURCE_LABEL_CAPACITY 256U
#define UMI_EDITOR_SOURCE_SYMBOL_ID_CAPACITY 128U
#define UMI_EDITOR_SOURCE_PREVIEW_CAPACITY 512U

/**
 * List the named editor source location kind values accepted by this public contract.
 */
typedef enum UmiEditorSourceLocationKind {
    UMI_EDITOR_SOURCE_LOCATION_UNKNOWN = 0,
    UMI_EDITOR_SOURCE_LOCATION_FILE = 1,
    UMI_EDITOR_SOURCE_LOCATION_SYMBOL = 2,
    UMI_EDITOR_SOURCE_LOCATION_DEFINITION = 3,
    UMI_EDITOR_SOURCE_LOCATION_DECLARATION = 4,
    UMI_EDITOR_SOURCE_LOCATION_REFERENCE = 5,
    UMI_EDITOR_SOURCE_LOCATION_IMPLEMENTATION = 6,
    UMI_EDITOR_SOURCE_LOCATION_BOOKMARK = 7,
    UMI_EDITOR_SOURCE_LOCATION_EDIT = 8,
    UMI_EDITOR_SOURCE_LOCATION_DIAGNOSTIC = 9
} UmiEditorSourceLocationKind;

/**
 * Represent the editor source location data shared with callers of this public contract.
 */
typedef struct UmiEditorSourceLocation {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorSourceLocationKind kind;
    uint64_t line;
    uint64_t column;
    uint64_t end_line;
    uint64_t end_column;
    uint64_t byte_offset;
    uint64_t end_byte_offset;
    uint64_t document_revision;
    int32_t score;
    char uri[UMI_EDITOR_SOURCE_URI_CAPACITY];
    char label[UMI_EDITOR_SOURCE_LABEL_CAPACITY];
    char symbol_id[UMI_EDITOR_SOURCE_SYMBOL_ID_CAPACITY];
    char preview[UMI_EDITOR_SOURCE_PREVIEW_CAPACITY];
} UmiEditorSourceLocation;

/**
 * Initialise editor source location from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_source_location_initialize(
    UmiEditorSourceLocation *location,
    const char *uri,
    uint64_t line,
    uint64_t column);
/**
 * Check that editor source location satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_editor_source_location_validate(
    const UmiEditorSourceLocation *location);
/**
 * Provide the editor source location same position operation used by this module and its
 * client applications.
 */
int umi_editor_source_location_same_position(
    const UmiEditorSourceLocation *left,
    const UmiEditorSourceLocation *right);
/**
 * Provide the editor source location compare operation used by this module and its client
 * applications.
 */
int umi_editor_source_location_compare(
    const UmiEditorSourceLocation *left,
    const UmiEditorSourceLocation *right);
/**
 * Provide the editor source location format operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_source_location_format(
    const UmiEditorSourceLocation *location,
    char *out_text,
    size_t out_capacity);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_editor_source_location_archive_encode(const UmiEditorSourceLocation *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_editor_source_location_archive_decode(const void *bytes, size_t byte_count,
    UmiEditorSourceLocation *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiEditorSourceLocation object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_editor_source_location_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiEditorSourceLocation *workspace, UmiEditorSourceLocation *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_editor_source_location_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiEditorSourceLocation *expected, UmiEditorSourceLocation *value);


#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_SOURCE_LOCATION_H */
