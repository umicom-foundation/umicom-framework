/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/workflow_step.c
 *
 * PURPOSE:
 *   Describe an orchestrated integration step with timeout and compensation metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/workflow_step.h"
#include <string.h>
#include <limits.h>

UmiStatus umi_fabric_workflow_step_init(UmiFabricWorkflowStep *item, const char *step_id, const char *operation_id, uint64_t timeout_ms, bool optional, bool compensatable) {
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->step_id,sizeof(item->step_id),step_id);if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->operation_id,sizeof(item->operation_id),operation_id);if(s!=UMI_STATUS_OK)return s;item->timeout_ms=timeout_ms;item->optional=optional;item->compensatable=compensatable;
    return umi_fabric_workflow_step_validate(item);
}
UmiStatus umi_fabric_workflow_step_validate(const UmiFabricWorkflowStep *item) {
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->step_id[0]!='\0' && item->operation_id[0]!='\0' && item->timeout_ms>0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
