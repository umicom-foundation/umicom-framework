/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/contribution.h
 *
 * PURPOSE:
 *   Define contributions from Framework modules and plug-ins to panes, actions,
 *   menus, toolbars, status items and perspectives.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_CONTRIBUTION_H
#define UMICOM_UI_CONTRIBUTION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_CONTRIBUTION_MAX 256U


/**
 * Represent the ui contribution snapshot data shared with callers of this public contract.
 */
typedef struct UmiUiContributionSnapshot {
    char contribution_id[UMI_UI_ID_CAPACITY];
    char provider_id[UMI_UI_ID_CAPACITY];
    char extension_point[UMI_UI_ID_CAPACITY];
    char target_id[UMI_UI_ID_CAPACITY];
    int32_t order;
    int enabled;
} UmiUiContributionSnapshot;

/**
 * Represent the ui contribution model data shared with callers of this public contract.
 */
typedef struct UmiUiContributionModel UmiUiContributionModel;

/**
 * Initialise ui contribution model from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ui_contribution_model_create(UmiUiContributionModel **out_model);
/**
 * Release or reset state held by ui contribution model so the same storage can be reused
 * safely.
 */
void umi_ui_contribution_model_destroy(UmiUiContributionModel *model);
/**
 * Provide the ui contribution model upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_contribution_model_upsert(UmiUiContributionModel *model,
                                     const UmiUiContributionSnapshot *item);
/**
 * Remove ui contribution model while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_contribution_model_remove(UmiUiContributionModel *model, const char *item_id);
/**
 * Find ui contribution model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_contribution_model_find(const UmiUiContributionModel *model, const char *item_id,
                                   UmiUiContributionSnapshot *out_item);
/**
 * Find ui contribution model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_contribution_model_at(const UmiUiContributionModel *model, size_t index,
                                 UmiUiContributionSnapshot *out_item);
/**
 * Return the number of records represented by ui contribution model without changing their
 * state.
 */
size_t umi_ui_contribution_model_count(const UmiUiContributionModel *model);
/**
 * Provide the ui contribution model revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_contribution_model_revision(const UmiUiContributionModel *model);

#ifdef __cplusplus
}
#endif

#endif
