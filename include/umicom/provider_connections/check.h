/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/provider_connections/check.h
 * PURPOSE: Review a saved destination before checking its model catalogue.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PROVIDER_CONNECTIONS_CHECK_H
#define UMICOM_PROVIDER_CONNECTIONS_CHECK_H
#include "umicom/provider_connections/connections.h"
#include "umicom/platform/cancellation.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiProviderConnectionCheckPlan {
    char application[UMI_PROVIDER_CONNECTION_ID_CAPACITY];
    char profile[UMI_PROVIDER_CONNECTION_ID_CAPACITY];
    uint64_t revision;
    UmiProviderConnection connection;
    char check_endpoint[UMI_PROVIDER_CONNECTION_ENDPOINT_CAPACITY];
    bool requires_credential;
} UmiProviderConnectionCheckPlan;
typedef struct UmiProviderConnectionCheckResult {
    unsigned http_status;
    size_t listed_models;
    bool model_listed;
} UmiProviderConnectionCheckResult;

/* Pure description: no database, credential or network access. Supported
 * adapters are openai at https://api.openai.com/v1/responses or
 * /v1/chat/completions, and local-chat at the exact loopback chat endpoint.
 * Both checks GET the same origin's /v1/models; they never send a prompt.
 * OpenAI requires vault:<alias>. Loopback never uses a credential.
 * Settings must be enabled and name a model. Output is unchanged on failure. */
UmiStatus UmiProviderConnectionCheckDescribe(const UmiProviderConnection *connection,
    char *out_endpoint, size_t capacity, bool *out_requires_credential);
/* Capture the exact saved record and revision for presentation to the user.
 * The supplied application/profile must match the store's immutable scope.
 * No authentication or HTTP request is performed. Keep the resulting plan
 * unchanged while reviewing it and while Run borrows it. */
UmiStatus UmiProviderConnectionCheckPrepare(UmiProviderConnections *store,
    const char *application_id, const char *profile_name, const char *connection_id,
    uint64_t expected_revision, UmiProviderConnectionCheckPlan *out_plan);
bool UmiProviderConnectionCheckAvailable(void);
/* Run only after explicit approval of the displayed plan. Remote checks
 * freshly verify the local profile password, obtain the matching vault key,
 * then recheck the saved revision before contacting the captured endpoint.
 * A later edit does not revoke a request already in flight. Workers must own
 * their Data Server connection. No database lock is held during vault/HTTP I/O.
 * Local checks require no password. The token is optional, borrowed until Run
 * returns; cancellation cannot unsend an HTTP request already transmitted.
 *
 * Result is cleared on entry, then contains only numeric/status evidence.
 * OK means a well-formed catalogue listed the configured model. It does not
 * prove inference permission, account balance, readiness or successful chat.
 * NOT_FOUND may mean a missing model or HTTP 404; inspect http_status.
 * No provider response text or credential is returned. Clear caller-owned
 * password buffers after use. No retry, redirects, proxy, tools or generation.
 * Calls are synchronous: a graphical host must use a worker and join users
 * before destroying borrowed stores and cancellation tokens. */
UmiStatus UmiProviderConnectionCheckRun(const UmiProviderConnectionCheckPlan *plan,
    UmiProviderConnections *store, bool approved, const char *local_password,
    const UmiCancellationToken *cancellation, UmiProviderConnectionCheckResult *out_result);
#ifdef __cplusplus
}
#endif
#endif
