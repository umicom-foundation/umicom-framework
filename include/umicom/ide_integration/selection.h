/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/selection.h
 *
 * PURPOSE:
 *   Validate and retain active editor selection/cursor context independently of
 *   a concrete text widget.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_SELECTION_H
#define UMICOM_IDE_INTEGRATION_SELECTION_H
#include "umicom/ide_integration/types.h"
#ifdef __cplusplus
extern "C" {
#endif

void umi_ide_editor_selection_init(UmiIdeEditorSelection *selection);

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

UmiStatus umi_ide_editor_selection_validate(
    const UmiIdeEditorSelection *selection);

#ifdef __cplusplus
}
#endif
#endif
