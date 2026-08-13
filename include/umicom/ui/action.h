/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/action.h
 *
 * PURPOSE:
 *   Define toolkit-neutral action metadata that binds visible UI intent to one
 *   canonical Framework command identifier.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ACTION_H
#define UMICOM_UI_ACTION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_ACTION_MAX 256U


typedef struct UmiUiActionSnapshot {
    char action_id[UMI_UI_ID_CAPACITY];
    char command_id[UMI_UI_ID_CAPACITY];
    char label[UMI_UI_TEXT_CAPACITY];
    char tooltip[UMI_UI_DESCRIPTION_CAPACITY];
    char icon_name[UMI_UI_ICON_CAPACITY];
    char accelerator[UMI_UI_ACCELERATOR_CAPACITY];
    int enabled;
    int visible;
    int checkable;
    int checked;
    int32_t order;
    /* Optional canonical command argument used by menus, toolbars and keys. */
    char argument[UMI_UI_DESCRIPTION_CAPACITY];
} UmiUiActionSnapshot;

typedef struct UmiUiActionModel UmiUiActionModel;

UmiStatus umi_ui_action_model_create(UmiUiActionModel **out_model);
void umi_ui_action_model_destroy(UmiUiActionModel *model);
UmiStatus umi_ui_action_model_upsert(UmiUiActionModel *model,
                                     const UmiUiActionSnapshot *item);
UmiStatus umi_ui_action_model_remove(UmiUiActionModel *model, const char *item_id);
UmiStatus umi_ui_action_model_find(const UmiUiActionModel *model, const char *item_id,
                                   UmiUiActionSnapshot *out_item);
UmiStatus umi_ui_action_model_at(const UmiUiActionModel *model, size_t index,
                                 UmiUiActionSnapshot *out_item);
size_t umi_ui_action_model_count(const UmiUiActionModel *model);
uint64_t umi_ui_action_model_revision(const UmiUiActionModel *model);

#ifdef __cplusplus
}
#endif

#endif
