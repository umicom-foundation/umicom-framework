/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/document_view.h
 *
 * PURPOSE:
 *   Define toolkit-neutral document-tab state including identity, title, dirty
 *   state, activation and pinning.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_DOCUMENT_VIEW_H
#define UMICOM_UI_DOCUMENT_VIEW_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_DOCUMENT_VIEW_MAX 256U


typedef struct UmiUiDocumentViewSnapshot {
    char view_id[UMI_UI_ID_CAPACITY];
    char document_id[UMI_UI_ID_CAPACITY];
    char title[UMI_UI_TEXT_CAPACITY];
    char icon_name[UMI_UI_ICON_CAPACITY];
    int dirty;
    int active;
    int pinned;
    int32_t order;
} UmiUiDocumentViewSnapshot;

typedef struct UmiUiDocumentViewModel UmiUiDocumentViewModel;

UmiStatus umi_ui_document_view_model_create(UmiUiDocumentViewModel **out_model);
void umi_ui_document_view_model_destroy(UmiUiDocumentViewModel *model);
UmiStatus umi_ui_document_view_model_upsert(UmiUiDocumentViewModel *model,
                                     const UmiUiDocumentViewSnapshot *item);
UmiStatus umi_ui_document_view_model_remove(UmiUiDocumentViewModel *model, const char *item_id);
UmiStatus umi_ui_document_view_model_find(const UmiUiDocumentViewModel *model, const char *item_id,
                                   UmiUiDocumentViewSnapshot *out_item);
UmiStatus umi_ui_document_view_model_at(const UmiUiDocumentViewModel *model, size_t index,
                                 UmiUiDocumentViewSnapshot *out_item);
size_t umi_ui_document_view_model_count(const UmiUiDocumentViewModel *model);
uint64_t umi_ui_document_view_model_revision(const UmiUiDocumentViewModel *model);

#ifdef __cplusplus
}
#endif

#endif
