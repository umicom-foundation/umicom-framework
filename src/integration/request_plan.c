/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/request_plan.c
 *
 * PURPOSE:
 *   Implement the request plan behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Provider-neutral request plan | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/integration/request_plan.h"
#include <stdio.h>
#include <string.h>

UmiStatus umi_integration_request_plan_build(const UmiIntegrationConnectionProfile *profile,const UmiIntegrationApiOperation *operation,const char *payload,const char *correlation_id,UmiIntegrationRequestPlan *out_plan)
{
    int length;
    UmiStatus status;
    UmiIntegrationDesignerValidation validation = {0};
    if (profile == NULL || operation == NULL || payload == NULL || correlation_id == NULL || out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!profile->enabled) return UMI_STATUS_UNAVAILABLE;
    status = umi_integration_connection_profile_validate(profile,&validation);
    if (status != UMI_STATUS_OK) return status;
    (void)memset(out_plan,0,sizeof(*out_plan));
    status = umi_integration_designer_copy(out_plan->profile_id,sizeof(out_plan->profile_id),profile->id);
    if (status != UMI_STATUS_OK) return status;
    out_plan->transport = profile->transport;
    length = snprintf(out_plan->target,sizeof(out_plan->target),"%s%s",profile->base_uri,operation->path);
    if (length < 0 || (size_t)length >= sizeof(out_plan->target)) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_integration_designer_copy(out_plan->method,sizeof(out_plan->method),operation->method);
    if (status == UMI_STATUS_OK) status = umi_integration_designer_copy(out_plan->secret_reference,sizeof(out_plan->secret_reference),profile->secret_reference);
    if (status == UMI_STATUS_OK) status = umi_integration_designer_copy(out_plan->correlation_id,sizeof(out_plan->correlation_id),correlation_id);
    if (status == UMI_STATUS_OK) status = umi_integration_designer_copy(out_plan->payload,sizeof(out_plan->payload),payload);
    out_plan->timeout_ms = profile->timeout_ms;
    out_plan->streaming = operation->streaming;
    return status;
}
