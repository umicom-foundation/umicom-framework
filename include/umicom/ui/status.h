/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/status.h
 *
 * PURPOSE:
 *   Define status-bar items with stable identity, priority and owned presentation text.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_STATUS_H
#define UMICOM_UI_STATUS_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_STATUS_MAX 256U


typedef struct UmiUiStatusSnapshot {
    char item_id[UMI_UI_ID_CAPACITY];
    char text[UMI_UI_DESCRIPTION_CAPACITY];
    char tooltip[UMI_UI_DESCRIPTION_CAPACITY];
    char command_id[UMI_UI_ID_CAPACITY];
    int32_t priority;
    int visible;
} UmiUiStatusSnapshot;

typedef struct UmiUiStatusModel UmiUiStatusModel;

UmiStatus umi_ui_status_model_create(UmiUiStatusModel **out_model);
void umi_ui_status_model_destroy(UmiUiStatusModel *model);
UmiStatus umi_ui_status_model_upsert(UmiUiStatusModel *model,
                                     const UmiUiStatusSnapshot *item);
UmiStatus umi_ui_status_model_remove(UmiUiStatusModel *model, const char *item_id);
UmiStatus umi_ui_status_model_find(const UmiUiStatusModel *model, const char *item_id,
                                   UmiUiStatusSnapshot *out_item);
UmiStatus umi_ui_status_model_at(const UmiUiStatusModel *model, size_t index,
                                 UmiUiStatusSnapshot *out_item);
size_t umi_ui_status_model_count(const UmiUiStatusModel *model);
uint64_t umi_ui_status_model_revision(const UmiUiStatusModel *model);

#ifdef __cplusplus
}
#endif

#endif
