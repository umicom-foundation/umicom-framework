/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/pane.h
 *
 * PURPOSE:
 *   Define workbench pane metadata, placement, visibility and sizing without exposing
 *   GTK, Qt, Wt or browser widget types.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_PANE_H
#define UMICOM_UI_PANE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_PANE_MAX 256U


typedef struct UmiUiPaneSnapshot {
    char pane_id[UMI_UI_ID_CAPACITY];
    char title[UMI_UI_TEXT_CAPACITY];
    char view_type[UMI_UI_ID_CAPACITY];
    char icon_name[UMI_UI_ICON_CAPACITY];
    UmiUiPlacement placement;
    int32_t order;
    int visible;
    int closable;
    int movable;
    UmiUiSize preferred_size;
} UmiUiPaneSnapshot;

typedef struct UmiUiPaneModel UmiUiPaneModel;

UmiStatus umi_ui_pane_model_create(UmiUiPaneModel **out_model);
void umi_ui_pane_model_destroy(UmiUiPaneModel *model);
UmiStatus umi_ui_pane_model_upsert(UmiUiPaneModel *model,
                                     const UmiUiPaneSnapshot *item);
UmiStatus umi_ui_pane_model_remove(UmiUiPaneModel *model, const char *item_id);
UmiStatus umi_ui_pane_model_find(const UmiUiPaneModel *model, const char *item_id,
                                   UmiUiPaneSnapshot *out_item);
UmiStatus umi_ui_pane_model_at(const UmiUiPaneModel *model, size_t index,
                                 UmiUiPaneSnapshot *out_item);
size_t umi_ui_pane_model_count(const UmiUiPaneModel *model);
uint64_t umi_ui_pane_model_revision(const UmiUiPaneModel *model);

#ifdef __cplusplus
}
#endif

#endif
