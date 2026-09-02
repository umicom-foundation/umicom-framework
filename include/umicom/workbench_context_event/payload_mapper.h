/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/payload_mapper.h
 *
 * PURPOSE:
 *   Convert normalised workbench interactions into canonical typed context payloads.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_PAYLOAD_MAPPER_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_PAYLOAD_MAPPER_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the workbench context event map payload operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_event_map_payload(
    const UmiWorkbenchContextEvent *event,
    UmiContextPayload *out_payload);

#ifdef __cplusplus
}
#endif
#endif
