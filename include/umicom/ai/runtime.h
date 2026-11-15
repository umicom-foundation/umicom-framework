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
/** Checked cleanup retains storage on invalid providers or BUSY generation/health work. */
UmiStatus UmiAiRuntimeRelease(UmiAiRuntime *runtime);
/**
 * Resolve and authorize a provider without invoking it. On failure out_provider
 * is unchanged. The successful pointer is borrowed, not a cached permission or
 * a lifetime lease. Serialize registry, policy and provider access, and repeat
 * admission immediately before external work after any intervening changes.
 * Approval is a caller-supplied decision; this function does not collect it.
 */
UmiStatus UmiAiRuntimeAdmitProvider(UmiAiRuntime *runtime, const char *provider_id,
    int approved, UmiAiProvider **out_provider);
/**
 * Legacy generation validates request bounds, then dispatches directly into
 * response. Provider error output remains available for compatibility. Use
 * UmiAiRuntimeGenerateChecked when failures must preserve the previous response.
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

/** Observe one checked generation attempt. status is the returned result.
 * provider_status is meaningful only when dispatched is nonzero.
 * response_status is meaningful only when response_checked is nonzero.
 * Unattempted stages use UNAVAILABLE; a provider's error is never replaced by
 * validation of its unfinished output. These fields do not imply model quality. */
typedef struct UmiAiGenerationReport {
    UmiStatus status;
    UmiStatus provider_status;
    UmiStatus response_status;
    int dispatched;
    int response_checked;
    int response_published;
} UmiAiGenerationReport;

/** Validate request, admit the current provider/policy, invoke it into private
 * staging storage, and publish only a structurally valid successful response.
 * Every failure preserves response. A successful malformed provider reply
 * returns INTERNAL_ERROR, with its value-validation result in out_report.
 * out_report may be NULL. Otherwise it records the attempt even on refusal.
 * Caller supplies complete stable request storage and a terminated provider_id.
 * Writable outputs must not overlap each other, request or runtime storage.
 * Serialize the entire call. This does not isolate malicious native callbacks,
 * synchronize threads, guard arbitrary tool callbacks, or validate response
 * identity/usage claims. Supported same-registry mutation and nested generation
 * are refused with BUSY while generation or AuthorEngine health refresh owns
 * the registry. */
UmiStatus UmiAiRuntimeGenerateChecked(UmiAiRuntime *runtime,
    const char *provider_id, int approved, const UmiAiRequest *request,
    UmiAiResponse *response, UmiAiGenerationReport *out_report);

/** Read-only teardown preflight for composition owners. BUSY means provider
 * generation or AuthorEngine health refresh is active on this thread;
 * otherwise validates provider ownership.
 * NULL is INVALID_ARGUMENT. It is not a lease; serialize the complete operation.
 * Direct tool calls and callbacks outside runtime generation or AuthorEngine
 * health refresh are not covered. Callbacks must return normally without freeing the runtime. */
UmiStatus UmiAiRuntimeCheckIdle(const UmiAiRuntime *runtime);

#ifdef __cplusplus
}
#endif

#endif
