/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/local_models/runtime.c
 *
 * PURPOSE:
 *   Project admitted native model settings into bounded unavailable runtime metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "umicom/local_models/runtime.h"
#include <stdio.h>
#include <string.h>

UmiStatus UmiModelRuntimeDescriptorCreate(const UmiModelRunPlan *plan,
    const char *runtimeId, const char *displayName, uint32_t maxOutputTokens,
    UmiAiRuntimeDescriptor *outDescriptor)
{
    uint64_t port, context;
    UmiAiRuntimeDescriptor descriptor = {0};
    UmiStatus status;
    if (outDescriptor == NULL ||
        !UmiModelNameValid(runtimeId, sizeof(descriptor.runtime_id), false) ||
        !UmiModelTextFits(displayName, sizeof(descriptor.display_name)) ||
        displayName[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiModelRunPlanValidate(plan);
    if (status != UMI_STATUS_OK) return status;
    if (!UmiModelNumber(plan->port, 1024U, 65535U, &port) ||
        !UmiModelNumber(plan->context, 512U, 32768U, &context) ||
        maxOutputTokens == 0U || maxOutputTokens > context) return UMI_STATUS_INVALID_ARGUMENT;
    memcpy(descriptor.runtime_id, runtimeId, strlen(runtimeId) + 1U);
    memcpy(descriptor.display_name, displayName, strlen(displayName) + 1U);
    memcpy(descriptor.model_id, plan->alias, strlen(plan->alias) + 1U);
    (void)strcpy(descriptor.provider_id, "umicom.local-chat");
    int count = snprintf(descriptor.endpoint, sizeof(descriptor.endpoint),
        "http://127.0.0.1:%u/v1/chat/completions", (unsigned)port);
    if (count < 0 || (size_t)count >= sizeof(descriptor.endpoint)) return UMI_STATUS_CAPACITY_EXCEEDED;
    descriptor.provider_kind = UMI_AI_PROVIDER_LOCAL;
    descriptor.transport = UMI_AI_RUNTIME_HTTP;
    descriptor.capabilities = UMI_AI_RUNTIME_CHAT | UMI_AI_RUNTIME_STREAMING;
    descriptor.context_tokens = (uint32_t)context;
    descriptor.max_output_tokens = maxOutputTokens;
    descriptor.configured = 1;
    descriptor.local = 1;
    /* Configuration alone never supplies a positive health observation.
     * Keep revision zero until the existing catalogue accepts this value. */
    (void)strcpy(descriptor.health.provider_id, descriptor.provider_id);
    (void)strcpy(descriptor.health.message, "Configured local model; endpoint and model identity unverified");
    status = UmiAiRuntimeDescriptorValidate(&descriptor);
    if (status != UMI_STATUS_OK) return status;
    *outDescriptor = descriptor;
    return UMI_STATUS_OK;
}

UmiStatus UmiModelSessionRuntimeDescribe(const UmiModelSession *session,
    const char *runtimeId, const char *displayName, uint32_t maxOutputTokens,
    UmiAiRuntimeDescriptor *outDescriptor)
{
    UmiModelRunPlan captured;
    if (outDescriptor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiModelSessionReadPlan(session, &captured);
    if (status != UMI_STATUS_OK) return status;
    return UmiModelRuntimeDescriptorCreate(&captured, runtimeId, displayName,
        maxOutputTokens, outDescriptor);
}
