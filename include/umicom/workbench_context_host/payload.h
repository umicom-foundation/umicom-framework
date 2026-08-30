/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/payload.h
 *
 * PURPOSE:
 *   Build and enrich canonical typed payloads emitted by workbench and application observations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_PAYLOAD_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_PAYLOAD_H
#include "umicom/workbench_context_host/observation.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_workbench_context_host_payload_from_observation(
    UmiContextPayload *out_payload,
    const char *context_id,const char *application_id,const char *panel_id,
    const UmiWorkbenchContextHostObservation *observation);
UmiStatus umi_workbench_context_host_payload_add_text(
    UmiContextPayload *payload,const char *name,const char *text);
UmiStatus umi_workbench_context_host_payload_add_unsigned(
    UmiContextPayload *payload,const char *name,uint64_t value);
#ifdef __cplusplus
}
#endif
#endif
