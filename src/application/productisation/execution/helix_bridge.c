/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/execution/helix_bridge.c
 *
 * PURPOSE:
 *   Map completion-plan intent into non-executing Helix action proposals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/execution/helix_bridge.h"
#include <stdio.h>
/*
 * Provide the product execution helix action kind operation used by this module and its
 * client applications.
 */
UmiHelixActionKind umi_product_execution_helix_action_kind(
    const UmiProductExecutionWorkItem *item)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_HELIX_ACTION_REVIEW;
    /* Apply this branch only when its contract condition is satisfied. */
    if (item->step.stage == UMI_PRODUCTISATION_STAGE_ACCEPTANCE)
        return UMI_HELIX_ACTION_TEST;
    /* Apply this branch only when its contract condition is satisfied. */
    if (item->step.gap_kind == UMI_PRODUCTISATION_GAP_ACCEPTANCE_EVIDENCE)
        return UMI_HELIX_ACTION_TEST;
    /* Apply this branch only when its contract condition is satisfied. */
    if (item->step.gap_kind == UMI_PRODUCTISATION_GAP_EXTERNAL_ADAPTER)
        return UMI_HELIX_ACTION_REVIEW;
    return UMI_HELIX_ACTION_FILESYSTEM;
}
/*
 * Provide the product execution helix action build operation used by this module and its
 * client applications.
 */
UmiStatus umi_product_execution_helix_action_build(
    const UmiProductExecutionWorkItem *item,
    UmiHelixAction *out_action)
{
    char action_id[UMI_HELIX_ID_CAPACITY];
    const char *target;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
