/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/contribution.h
 *
 * PURPOSE:
 *   Define contributions from Framework modules and plug-ins to panes, actions,
 *   menus, toolbars, status items and perspectives.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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


typedef struct UmiUiContributionSnapshot {
    char contribution_id[UMI_UI_ID_CAPACITY];
    char provider_id[UMI_UI_ID_CAPACITY];
    char extension_point[UMI_UI_ID_CAPACITY];
    char target_id[UMI_UI_ID_CAPACITY];
    int32_t order;
    int enabled;
} UmiUiContributionSnapshot;

typedef struct UmiUiContributionModel UmiUiContributionModel;

UmiStatus umi_ui_contribution_model_create(UmiUiContributionModel **out_model);
void umi_ui_contribution_model_destroy(UmiUiContributionModel *model);
UmiStatus umi_ui_contribution_model_upsert(UmiUiContributionModel *model,
                                     const UmiUiContributionSnapshot *item);
UmiStatus umi_ui_contribution_model_remove(UmiUiContributionModel *model, const char *item_id);
UmiStatus umi_ui_contribution_model_find(const UmiUiContributionModel *model, const char *item_id,
                                   UmiUiContributionSnapshot *out_item);
UmiStatus umi_ui_contribution_model_at(const UmiUiContributionModel *model, size_t index,
                                 UmiUiContributionSnapshot *out_item);
size_t umi_ui_contribution_model_count(const UmiUiContributionModel *model);
uint64_t umi_ui_contribution_model_revision(const UmiUiContributionModel *model);

#ifdef __cplusplus
}
#endif

#endif
