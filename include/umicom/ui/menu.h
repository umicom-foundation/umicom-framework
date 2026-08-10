/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/menu.h
 *
 * PURPOSE:
 *   Define a toolkit-neutral menu contribution model whose items reference registered
 *   actions instead of toolkit callbacks.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MENU_H
#define UMICOM_UI_MENU_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_MENU_MAX 256U


typedef struct UmiUiMenuSnapshot {
    char item_id[UMI_UI_ID_CAPACITY];
    char menu_id[UMI_UI_ID_CAPACITY];
    char section_id[UMI_UI_ID_CAPACITY];
    char action_id[UMI_UI_ID_CAPACITY];
    char label_override[UMI_UI_TEXT_CAPACITY];
    int separator;
    int32_t order;
} UmiUiMenuSnapshot;

typedef struct UmiUiMenuModel UmiUiMenuModel;

UmiStatus umi_ui_menu_model_create(UmiUiMenuModel **out_model);
void umi_ui_menu_model_destroy(UmiUiMenuModel *model);
UmiStatus umi_ui_menu_model_upsert(UmiUiMenuModel *model,
                                     const UmiUiMenuSnapshot *item);
UmiStatus umi_ui_menu_model_remove(UmiUiMenuModel *model, const char *item_id);
UmiStatus umi_ui_menu_model_find(const UmiUiMenuModel *model, const char *item_id,
                                   UmiUiMenuSnapshot *out_item);
UmiStatus umi_ui_menu_model_at(const UmiUiMenuModel *model, size_t index,
                                 UmiUiMenuSnapshot *out_item);
size_t umi_ui_menu_model_count(const UmiUiMenuModel *model);
uint64_t umi_ui_menu_model_revision(const UmiUiMenuModel *model);

#ifdef __cplusplus
}
#endif

#endif
