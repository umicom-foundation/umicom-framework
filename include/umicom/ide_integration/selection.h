/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/selection.h
 *
 * PURPOSE:
 *   Validate and retain active editor selection/cursor context independently of
 *   a concrete text widget.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_SELECTION_H
#define UMICOM_IDE_INTEGRATION_SELECTION_H
#include "umicom/ide_integration/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialise ide editor selection from caller-provided values so later operations receive
 * a known state.
 */
void umi_ide_editor_selection_init(UmiIdeEditorSelection *selection);

/**
 * Copy ide editor selection into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_ide_editor_selection_set(
    UmiIdeEditorSelection *selection,
    const char *document_id,
    const char *path,
    const char *language_id,
    const char *text,
    uint32_t start_line,
    uint32_t start_column,
    uint32_t end_line,
    uint32_t end_column,
    uint64_t document_revision,
    int dirty);

/**
 * Check that ide editor selection satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ide_editor_selection_validate(
    const UmiIdeEditorSelection *selection);

#ifdef __cplusplus
}
#endif
#endif
