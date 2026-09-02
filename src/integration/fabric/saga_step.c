/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/saga_step.c
 *
 * PURPOSE:
 *   Describe a saga action and its compensation operation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/saga_step.h"
#include <string.h>
#include <limits.h>

/*
 * Initialise fabric saga step from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_fabric_saga_step_init(UmiFabricSagaStep *item, const char *step_id, const char *action_operation, const char *compensation_operation, bool compensation_required) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->step_id,sizeof(item->step_id),step_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->action_operation,sizeof(item->action_operation),action_operation);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->compensation_operation,sizeof(item->compensation_operation),compensation_operation);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->compensation_required=compensation_required;
    return umi_fabric_saga_step_validate(item);
}
/* Check that fabric saga step satisfies its contract before another service relies on it. */
UmiStatus umi_fabric_saga_step_validate(const UmiFabricSagaStep *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->step_id[0]!='\0' && item->action_operation[0]!='\0' && (!item->compensation_required || item->compensation_operation[0]!='\0'))) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
