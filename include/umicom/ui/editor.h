/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/editor.h
 *
 * PURPOSE:
 *   Define editor presentation state that remains independent from GtkSourceView and
 *   other concrete text-editor widgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_EDITOR_H
#define UMICOM_UI_EDITOR_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_EDITOR_MAX 256U


/**
 * Represent the ui editor snapshot data shared with callers of this public contract.
 */
typedef struct UmiUiEditorSnapshot {
    char editor_id[UMI_UI_ID_CAPACITY];
    char document_id[UMI_UI_ID_CAPACITY];
    char language_id[UMI_UI_ID_CAPACITY];
    int64_t cursor_offset;
    int64_t selection_start;
    int64_t selection_end;
    int32_t top_line;
    int read_only;
    int focused;
} UmiUiEditorSnapshot;

/**
 * Represent the ui editor model data shared with callers of this public contract.
 */
typedef struct UmiUiEditorModel UmiUiEditorModel;

/**
 * Initialise ui editor model from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_editor_model_create(UmiUiEditorModel **out_model);
/**
 * Release or reset state held by ui editor model so the same storage can be reused safely.
 */
void umi_ui_editor_model_destroy(UmiUiEditorModel *model);
/**
 * Provide the ui editor model upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_editor_model_upsert(UmiUiEditorModel *model,
                                     const UmiUiEditorSnapshot *item);
/**
 * Remove ui editor model while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_ui_editor_model_remove(UmiUiEditorModel *model, const char *item_id);
/**
 * Find ui editor model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_editor_model_find(const UmiUiEditorModel *model, const char *item_id,
                                   UmiUiEditorSnapshot *out_item);
/**
 * Find ui editor model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_editor_model_at(const UmiUiEditorModel *model, size_t index,
                                 UmiUiEditorSnapshot *out_item);
/**
 * Return the number of records represented by ui editor model without changing their
 * state.
 */
size_t umi_ui_editor_model_count(const UmiUiEditorModel *model);
/**
 * Provide the ui editor model revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_editor_model_revision(const UmiUiEditorModel *model);

#ifdef __cplusplus
}
#endif

#endif
