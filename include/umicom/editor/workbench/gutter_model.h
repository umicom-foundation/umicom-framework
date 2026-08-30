/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/gutter_model.h
 *
 * PURPOSE:
 *   Describe reusable editor gutter channels and their visibility.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral editor-workbench capability extends canonical
 *   Umicom::editor and composes Framework-owned UI semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_GUTTER_MODEL_H
#define UMICOM_EDITOR_WORKBENCH_GUTTER_MODEL_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbGutterModel { bool line_numbers; bool folding; bool diagnostics; bool bookmarks; uint32_t width; } UmiEditorWbGutterModel;
void umi_editor_wb_gutter_model_init(UmiEditorWbGutterModel *state); uint32_t umi_editor_wb_gutter_model_channel_count(const UmiEditorWbGutterModel *state);

#ifdef __cplusplus
}
#endif
#endif
