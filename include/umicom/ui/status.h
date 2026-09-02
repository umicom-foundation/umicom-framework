/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/status.h
 *
 * PURPOSE:
 *   Define status-bar items with stable identity, priority and owned presentation text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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


/**
 * Represent the ui status snapshot data shared with callers of this public contract.
 */
typedef struct UmiUiStatusSnapshot {
    char item_id[UMI_UI_ID_CAPACITY];
    char text[UMI_UI_DESCRIPTION_CAPACITY];
    char tooltip[UMI_UI_DESCRIPTION_CAPACITY];
    char command_id[UMI_UI_ID_CAPACITY];
    int32_t priority;
    int visible;
} UmiUiStatusSnapshot;

/**
 * Represent the ui status model data shared with callers of this public contract.
 */
typedef struct UmiUiStatusModel UmiUiStatusModel;

/**
 * Initialise ui status model from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_status_model_create(UmiUiStatusModel **out_model);
/**
 * Release or reset state held by ui status model so the same storage can be reused safely.
 */
void umi_ui_status_model_destroy(UmiUiStatusModel *model);
/**
 * Provide the ui status model upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_status_model_upsert(UmiUiStatusModel *model,
                                     const UmiUiStatusSnapshot *item);
/**
 * Remove ui status model while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_ui_status_model_remove(UmiUiStatusModel *model, const char *item_id);
/**
 * Find ui status model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_status_model_find(const UmiUiStatusModel *model, const char *item_id,
                                   UmiUiStatusSnapshot *out_item);
/**
 * Find ui status model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_status_model_at(const UmiUiStatusModel *model, size_t index,
                                 UmiUiStatusSnapshot *out_item);
/**
 * Return the number of records represented by ui status model without changing their
 * state.
 */
size_t umi_ui_status_model_count(const UmiUiStatusModel *model);
/**
 * Provide the ui status model revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_status_model_revision(const UmiUiStatusModel *model);

#ifdef __cplusplus
}
#endif

#endif
