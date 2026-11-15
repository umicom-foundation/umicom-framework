/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/local_models/runtime.h
 *
 * PURPOSE:
 *   Describe native model run settings through the shared AI runtime catalogue without executing them.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LOCAL_MODELS_RUNTIME_H
#define UMICOM_LOCAL_MODELS_RUNTIME_H
#include "umicom/local_models/manager.h"
#include "umicom/ai/runtime_catalogue.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Pure conversion: validate the full run plan, then describe the canonical
 * loopback chat provider with the plan's alias, port and context capacity.
 * runtimeId is a stable application-chosen identifier; displayName is a label.
 * maxOutputTokens must be 1..context. The result starts configured, local and
 * unavailable, with revision zero for catalogue assignment. CHAT/STREAMING
 * describe the intended adapter interface, not a tested model capability.
 * No file is inspected, process started, provider registered or endpoint probed.
 * Output is unchanged on failure; inputs must remain stable during the call. */
UmiStatus UmiModelRuntimeDescriptorCreate(const UmiModelRunPlan *plan,
    const char *runtimeId, const char *displayName, uint32_t maxOutputTokens,
    UmiAiRuntimeDescriptor *outDescriptor);
/* Describe the admitted plan captured by a submitted session. This does not
 * read process status or promote health, including after the process exits.
 * Read UmiModelSessionRead separately for process observations. Keep the
 * serialized session owner alive; a retained failed-start owner is refused. */
UmiStatus UmiModelSessionRuntimeDescribe(const UmiModelSession *session,
    const char *runtimeId, const char *displayName, uint32_t maxOutputTokens,
    UmiAiRuntimeDescriptor *outDescriptor);
#ifdef __cplusplus
}
#endif
#endif
