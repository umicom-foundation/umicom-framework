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


/**
 * Represent the ui toolbar snapshot data shared with callers of this public contract.
 */
typedef struct UmiUiToolbarSnapshot {
    char item_id[UMI_UI_ID_CAPACITY];
    char toolbar_id[UMI_UI_ID_CAPACITY];
    char action_id[UMI_UI_ID_CAPACITY];
    char group_id[UMI_UI_ID_CAPACITY];
    int separator;
    int32_t order;
} UmiUiToolbarSnapshot;

/**
 * Represent the ui toolbar model data shared with callers of this public contract.
 */
typedef struct UmiUiToolbarModel UmiUiToolbarModel;

/**
 * Initialise ui toolbar model from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_toolbar_model_create(UmiUiToolbarModel **out_model);
/**
 * Release or reset state held by ui toolbar model so the same storage can be reused
 * safely.
 */
void umi_ui_toolbar_model_destroy(UmiUiToolbarModel *model);
/**
 * Provide the ui toolbar model upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_toolbar_model_upsert(UmiUiToolbarModel *model,
                                     const UmiUiToolbarSnapshot *item);
/**
 * Remove ui toolbar model while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_ui_toolbar_model_remove(UmiUiToolbarModel *model, const char *item_id);
/**
 * Find ui toolbar model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_toolbar_model_find(const UmiUiToolbarModel *model, const char *item_id,
                                   UmiUiToolbarSnapshot *out_item);
/**
 * Find ui toolbar model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_toolbar_model_at(const UmiUiToolbarModel *model, size_t index,
                                 UmiUiToolbarSnapshot *out_item);
/**
 * Return the number of records represented by ui toolbar model without changing their
 * state.
 */
size_t umi_ui_toolbar_model_count(const UmiUiToolbarModel *model);
/**
 * Provide the ui toolbar model revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_toolbar_model_revision(const UmiUiToolbarModel *model);

#ifdef __cplusplus
}
#endif

#endif
