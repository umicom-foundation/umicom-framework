/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/saga_step.c
 *
 * PURPOSE:
 *   Describe a saga action and its compensation operation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/saga_step.h"
#include <string.h>
#include <limits.h>

UmiStatus umi_fabric_saga_step_init(UmiFabricSagaStep *item, const char *step_id, const char *action_operation, const char *compensation_operation, bool compensation_required) {
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->step_id,sizeof(item->step_id),step_id);if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->action_operation,sizeof(item->action_operation),action_operation);if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->compensation_operation,sizeof(item->compensation_operation),compensation_operation);if(s!=UMI_STATUS_OK)return s;item->compensation_required=compensation_required;
    return umi_fabric_saga_step_validate(item);
}
UmiStatus umi_fabric_saga_step_validate(const UmiFabricSagaStep *item) {
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->step_id[0]!='\0' && item->action_operation[0]!='\0' && (!item->compensation_required || item->compensation_operation[0]!='\0'))) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
