/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/runtime.h
 *
 * PURPOSE:
 *   Own the provider registry, tool registry and AI policy used by one application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The runtime centralises provider and tool selection so individual Studio panes cannot bypass policy or create private provider registries.
 */

#ifndef INCLUDE_UMICOM_AI_RUNTIME_H
#define INCLUDE_UMICOM_AI_RUNTIME_H

#include "umicom/ai/provider_registry.h"
#include "umicom/ai/tool_registry.h"
#include "umicom/ai/policy.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai runtime data shared with callers of this public contract.
 */
typedef struct UmiAiRuntime {
    UmiAiProviderRegistry providers;
    UmiAiToolRegistry tools;
    UmiAiPolicy policy;
} UmiAiRuntime;

/**
 * Initialise ai runtime from caller-provided values so later operations receive a known
 * state.
 */
void umi_ai_runtime_init(UmiAiRuntime *runtime);
/**
 * Release or reset state held by ai runtime so the same storage can be reused safely.
 */
void umi_ai_runtime_destroy(UmiAiRuntime *runtime);
/**
 * Provide the ai runtime generate operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_runtime_generate(UmiAiRuntime *runtime,
                                  const char *provider_id,
                                  int approved,
                                  const UmiAiRequest *request,
                                  UmiAiResponse *response);
/**
 * Provide the ai runtime invoke tool operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_runtime_invoke_tool(UmiAiRuntime *runtime,
                                     const char *tool_id,
                                     int approved,
                                     const char *arguments_json,
                                     char *output,
                                     size_t output_capacity);

#ifdef __cplusplus
}
#endif

#endif
