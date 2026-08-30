/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/request_plan.h
 *
 * PURPOSE:
 *   Publish the public request plan contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_REQUEST_PLAN_H
#define UMICOM_INTEGRATION_REQUEST_PLAN_H
#include "umicom/integration/connection_profile.h"
#include "umicom/integration/api_contract.h"
typedef struct UmiIntegrationRequestPlan {
    char profile_id[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    UmiIntegrationDesignerTransport transport;
    char target[UMI_INTEGRATION_DESIGNER_URI_CAPACITY];
    char method[16U];
    char secret_reference[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    char correlation_id[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    char payload[UMI_INTEGRATION_DESIGNER_PAYLOAD_CAPACITY];
    unsigned timeout_ms;
    bool streaming;
} UmiIntegrationRequestPlan;
UmiStatus umi_integration_request_plan_build(const UmiIntegrationConnectionProfile *profile,const UmiIntegrationApiOperation *operation,const char *payload,const char *correlation_id,UmiIntegrationRequestPlan *out_plan);
#endif
