/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/menu.h
 *
 * PURPOSE:
 *   Define a toolkit-neutral menu contribution model whose items reference registered
 *   actions instead of toolkit callbacks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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


/**
 * Represent the ui menu snapshot data shared with callers of this public contract.
 */
typedef struct UmiUiMenuSnapshot {
    char item_id[UMI_UI_ID_CAPACITY];
    char menu_id[UMI_UI_ID_CAPACITY];
    char section_id[UMI_UI_ID_CAPACITY];
    char action_id[UMI_UI_ID_CAPACITY];
    char label_override[UMI_UI_TEXT_CAPACITY];
    int separator;
    int32_t order;
} UmiUiMenuSnapshot;

/**
 * Represent the ui menu model data shared with callers of this public contract.
 */
typedef struct UmiUiMenuModel UmiUiMenuModel;

/**
 * Initialise ui menu model from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_ui_menu_model_create(UmiUiMenuModel **out_model);
/**
 * Release or reset state held by ui menu model so the same storage can be reused safely.
 */
void umi_ui_menu_model_destroy(UmiUiMenuModel *model);
/**
 * Provide the ui menu model upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_menu_model_upsert(UmiUiMenuModel *model,
                                     const UmiUiMenuSnapshot *item);
/**
 * Remove ui menu model while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_ui_menu_model_remove(UmiUiMenuModel *model, const char *item_id);
/**
 * Find ui menu model while leaving the underlying catalogue or model owned by this module.
 */
UmiStatus umi_ui_menu_model_find(const UmiUiMenuModel *model, const char *item_id,
                                   UmiUiMenuSnapshot *out_item);
/**
 * Find ui menu model while leaving the underlying catalogue or model owned by this module.
 */
UmiStatus umi_ui_menu_model_at(const UmiUiMenuModel *model, size_t index,
                                 UmiUiMenuSnapshot *out_item);
/**
 * Return the number of records represented by ui menu model without changing their state.
 */
size_t umi_ui_menu_model_count(const UmiUiMenuModel *model);
/**
 * Provide the ui menu model revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_menu_model_revision(const UmiUiMenuModel *model);

#ifdef __cplusplus
}
#endif

#endif
