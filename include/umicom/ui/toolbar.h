/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/toolbar.h
 *
 * PURPOSE:
 *   Define a toolkit-neutral toolbar contribution model shared by desktop, web
 *   and headless frontend adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_TOOLBAR_H
#define UMICOM_UI_TOOLBAR_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_TOOLBAR_MAX 256U


typedef struct UmiUiToolbarSnapshot {
    char item_id[UMI_UI_ID_CAPACITY];
    char toolbar_id[UMI_UI_ID_CAPACITY];
    char action_id[UMI_UI_ID_CAPACITY];
    char group_id[UMI_UI_ID_CAPACITY];
    int separator;
    int32_t order;
} UmiUiToolbarSnapshot;

typedef struct UmiUiToolbarModel UmiUiToolbarModel;

UmiStatus umi_ui_toolbar_model_create(UmiUiToolbarModel **out_model);
void umi_ui_toolbar_model_destroy(UmiUiToolbarModel *model);
UmiStatus umi_ui_toolbar_model_upsert(UmiUiToolbarModel *model,
                                     const UmiUiToolbarSnapshot *item);
UmiStatus umi_ui_toolbar_model_remove(UmiUiToolbarModel *model, const char *item_id);
UmiStatus umi_ui_toolbar_model_find(const UmiUiToolbarModel *model, const char *item_id,
                                   UmiUiToolbarSnapshot *out_item);
UmiStatus umi_ui_toolbar_model_at(const UmiUiToolbarModel *model, size_t index,
                                 UmiUiToolbarSnapshot *out_item);
size_t umi_ui_toolbar_model_count(const UmiUiToolbarModel *model);
uint64_t umi_ui_toolbar_model_revision(const UmiUiToolbarModel *model);

#ifdef __cplusplus
}
#endif

#endif
