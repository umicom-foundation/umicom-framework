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

UmiStatus umi_editor_source_location_initialize(
    UmiEditorSourceLocation *location,
    const char *uri,
    uint64_t line,
    uint64_t column);
UmiStatus umi_editor_source_location_validate(
    const UmiEditorSourceLocation *location);
int umi_editor_source_location_same_position(
    const UmiEditorSourceLocation *left,
    const UmiEditorSourceLocation *right);
int umi_editor_source_location_compare(
    const UmiEditorSourceLocation *left,
    const UmiEditorSourceLocation *right);
UmiStatus umi_editor_source_location_format(
    const UmiEditorSourceLocation *location,
    char *out_text,
    size_t out_capacity);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_SOURCE_LOCATION_H */
