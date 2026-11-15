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
/* The generate callback retains its full-response contract. The separate
 * reviewed_stream.h owner can stream an approved job using this canonical
 * provider configuration and the same durable workspace policy. */
UmiStatus UmiAiWorkspaceLocalProviderCreate(uint16_t port, uint32_t timeoutMillis,
    const UmiAiWorkspaceCancellation *cancellation, UmiAiProvider *outProvider);
bool UmiAiWorkspaceLocalProviderAvailable(void);

/* One bounded observation from a trusted loopback endpoint, independent of
 * any process supervisor or model-file identity. HTTP 200/status ok reports
 * ready; the documented HTTP 503 error reports unavailable. Neither proves
 * endpoint ownership, the selected model, inference quality or future health. */
typedef struct UmiAiLocalHealth {
    uint32_t http_status;
    bool endpoint_reports_ready;
} UmiAiLocalHealth;
/* GET http://127.0.0.1:<port>/health; no prompts, credentials, redirects,
 * proxies, retries or polling. Port 1024..65535, timeout 100..60000 ms, body
 * fewer than 4096 bytes. Output remains unchanged on failure/cancellation.
 * The token is borrowed only during this blocking call. Run it on a joined
 * worker in GUI clients; obtain explicit network consent before calling. */
UmiStatus UmiAiWorkspaceLocalHealthRead(uint16_t port, uint32_t timeoutMillis,
    const UmiAiWorkspaceCancellation *cancellation, UmiAiLocalHealth *outHealth);

/* Register the harmless read-only capacities tool in a caller-owned runtime.
 * Policy permission and the workspace allowlist must still be enabled by the
 * host, and each invocation must still be reviewed. Arguments are exactly {}. */
UmiStatus UmiAiWorkspaceRegisterCapacityTool(UmiAiRuntime *runtime);
#ifdef __cplusplus
}
#endif
#endif
