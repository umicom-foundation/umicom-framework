/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_workspace/providers.h
 *
 * PURPOSE:
 *   Expose an offline extractive provider and optional loopback-only model transport through the canonical AI ABI.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_WORKSPACE_PROVIDERS_H
#define UMICOM_AI_WORKSPACE_PROVIDERS_H
#include "umicom/ai_workspace/workspace.h"
#ifdef __cplusplus
extern "C" {
#endif
/* No neural model is bundled. This provider copies bounded reference excerpts
 * so the complete review/persistence workflow can be tried without a network.
 * Its source framing is produced by UmiAiWorkspaceBuildRequest; arbitrary chat
 * messages are not inferred to be verified source passages. */
UmiStatus UmiAiWorkspaceExtractiveProviderCreate(UmiAiProvider *outProvider);
/* A real HTTP client for an already-running, trusted local model server.
 * Only http://127.0.0.1:<port>/v1/chat/completions is supported. No redirects,
 * proxies, credentials, external hosts, tools, streaming or model downloads.
 * Port: 1024..65535. Timeout: 100..60000 ms. The cancellation token is borrowed
 * until the provider is destroyed. Create/destroy on the owner thread and join
 * generation work first. Successful registry add transfers instance ownership.
 * Missing optional libcurl/json-c support returns UNAVAILABLE, never a mock. */
UmiStatus UmiAiWorkspaceLocalProviderCreate(uint16_t port, uint32_t timeoutMillis,
    const UmiAiWorkspaceCancellation *cancellation, UmiAiProvider *outProvider);
bool UmiAiWorkspaceLocalProviderAvailable(void);
/* Register the harmless read-only capacities tool in a caller-owned runtime.
 * Policy permission and the workspace allowlist must still be enabled by the
 * host, and each invocation must still be reviewed. Arguments are exactly {}. */
UmiStatus UmiAiWorkspaceRegisterCapacityTool(UmiAiRuntime *runtime);
#ifdef __cplusplus
}
#endif
#endif
