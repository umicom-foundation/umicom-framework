/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/execution/helix_bridge.c
 *
 * PURPOSE:
 *   Map completion-plan intent into non-executing Helix action proposals.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/execution/helix_bridge.h"
#include <stdio.h>
UmiHelixActionKind umi_product_execution_helix_action_kind(
    const UmiProductExecutionWorkItem *item)
{
    if (item == NULL) return UMI_HELIX_ACTION_REVIEW;
    if (item->step.stage == UMI_PRODUCTISATION_STAGE_ACCEPTANCE)
        return UMI_HELIX_ACTION_TEST;
    if (item->step.gap_kind == UMI_PRODUCTISATION_GAP_ACCEPTANCE_EVIDENCE)
        return UMI_HELIX_ACTION_TEST;
    if (item->step.gap_kind == UMI_PRODUCTISATION_GAP_EXTERNAL_ADAPTER)
        return UMI_HELIX_ACTION_REVIEW;
    return UMI_HELIX_ACTION_FILESYSTEM;
}
UmiStatus umi_product_execution_helix_action_build(
    const UmiProductExecutionWorkItem *item,
    UmiHelixAction *out_action)
{
    char action_id[UMI_HELIX_ID_CAPACITY];
    const char *target;
    if (item == NULL || out_action == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)snprintf(action_id, sizeof(action_id), "productisation.%zu", item->plan_index);
    target = item->step.asset_id[0] != '\0'
        ? item->step.asset_id : item->step.capability_id;
    return umi_helix_action_init(
        out_action,
        action_id,
        umi_product_execution_helix_action_kind(item),
        item->step.action,
        target,
        item->maximum_attempts);
}
