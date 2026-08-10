/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/editor.h
 *
 * PURPOSE:
 *   Define editor presentation state that remains independent from GtkSourceView and
 *   other concrete text-editor widgets.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiUiEditorModel UmiUiEditorModel;

UmiStatus umi_ui_editor_model_create(UmiUiEditorModel **out_model);
void umi_ui_editor_model_destroy(UmiUiEditorModel *model);
UmiStatus umi_ui_editor_model_upsert(UmiUiEditorModel *model,
                                     const UmiUiEditorSnapshot *item);
UmiStatus umi_ui_editor_model_remove(UmiUiEditorModel *model, const char *item_id);
UmiStatus umi_ui_editor_model_find(const UmiUiEditorModel *model, const char *item_id,
                                   UmiUiEditorSnapshot *out_item);
UmiStatus umi_ui_editor_model_at(const UmiUiEditorModel *model, size_t index,
                                 UmiUiEditorSnapshot *out_item);
size_t umi_ui_editor_model_count(const UmiUiEditorModel *model);
uint64_t umi_ui_editor_model_revision(const UmiUiEditorModel *model);

#ifdef __cplusplus
}
#endif

#endif
