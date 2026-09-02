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

/**
 * Represent the editor line index data shared with callers of this public contract.
 */
typedef struct UmiEditorLineIndex UmiEditorLineIndex;

/**
 * Represent the editor line location data shared with callers of this public contract.
 */
typedef struct UmiEditorLineLocation {
    size_t line;
    size_t column_bytes;
    size_t line_start_offset;
    size_t line_end_offset;
} UmiEditorLineLocation;

/**
 * Initialise editor line index from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_editor_line_index_create(UmiEditorLineIndex **out_index);
/**
 * Release or reset state held by editor line index so the same storage can be reused
 * safely.
 */
void umi_editor_line_index_destroy(UmiEditorLineIndex *index);
/**
 * Provide the editor line index rebuild operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_line_index_rebuild(UmiEditorLineIndex *index,
                                         const char *bytes,
                                         size_t byte_count,
                                         uint64_t source_revision);
/**
 * Provide the editor line index locate offset operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_line_index_locate_offset(
    const UmiEditorLineIndex *index,
    size_t offset,
    UmiEditorLineLocation *out_location);
/**
 * Provide the editor line index line range operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_line_index_line_range(const UmiEditorLineIndex *index,
                                            size_t line,
                                            size_t *out_start_offset,
                                            size_t *out_end_offset);
/**
 * Return the number of records represented by editor line index without changing their
 * state.
 */
size_t umi_editor_line_index_count(const UmiEditorLineIndex *index);
/**
 * Provide the editor line index source revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_line_index_source_revision(const UmiEditorLineIndex *index);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_LINE_INDEX_H */
