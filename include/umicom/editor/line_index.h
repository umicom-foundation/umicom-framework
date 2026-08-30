/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/line_index.h
 *
 * PURPOSE:
 *   Map text byte offsets and zero-based line numbers without depending on a
 *   GUI text widget or language service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_LINE_INDEX_H
#define UMICOM_EDITOR_LINE_INDEX_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorLineIndex UmiEditorLineIndex;

typedef struct UmiEditorLineLocation {
    size_t line;
    size_t column_bytes;
    size_t line_start_offset;
    size_t line_end_offset;
} UmiEditorLineLocation;

UmiStatus umi_editor_line_index_create(UmiEditorLineIndex **out_index);
void umi_editor_line_index_destroy(UmiEditorLineIndex *index);
UmiStatus umi_editor_line_index_rebuild(UmiEditorLineIndex *index,
                                         const char *bytes,
                                         size_t byte_count,
                                         uint64_t source_revision);
UmiStatus umi_editor_line_index_locate_offset(
    const UmiEditorLineIndex *index,
    size_t offset,
    UmiEditorLineLocation *out_location);
UmiStatus umi_editor_line_index_line_range(const UmiEditorLineIndex *index,
                                            size_t line,
                                            size_t *out_start_offset,
                                            size_t *out_end_offset);
size_t umi_editor_line_index_count(const UmiEditorLineIndex *index);
uint64_t umi_editor_line_index_source_revision(const UmiEditorLineIndex *index);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_LINE_INDEX_H */
