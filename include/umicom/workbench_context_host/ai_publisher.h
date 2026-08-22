/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/ai_publisher.h
 *
 * PURPOSE:
 *   Publish AI conversation selections while preserving provider, conversation and evidence identifiers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_AI_PUBLISHER_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_AI_PUBLISHER_H
#include "umicom/workbench_context_host/host.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_workbench_context_host_publish_ai(
    UmiWorkbenchContextHost *host,
    const char *group_id,
    const char *source_panel_id,
    const char *context_id,
    const char * conversation_id,
    const char * message_id,
    const char * provider_id,
    const char * model_id,
    const char * evidence_id,
    uint64_t now_ms);
#ifdef __cplusplus
}
#endif
#endif
