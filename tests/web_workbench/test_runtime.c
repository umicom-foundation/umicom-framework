/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web_workbench/test_runtime.c
 * PURPOSE: Verify end-to-end expansion, transient auth, execution and safe history.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/web/workbench/runtime.h"

typedef struct TransportEvidence {
    int calls;
    bool saw_auth;
    bool saw_resolved_url;
} TransportEvidence;

/*
 * Exercise execute stub and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus execute_stub(
    void *context,
    const UmiWebWorkbenchRequest *request,
    const volatile bool *cancel_requested,
    UmiWebWorkbenchResponse *out_response)
{
    TransportEvidence *evidence = (TransportEvidence *)context;
    UmiWebWorkbenchTiming timing = {1U, 2U, 3U, 4U, 10U};
    evidence->calls++;
    evidence->saw_auth = strcmp(umi_web_workbench_request_header(request,
        "Authorization"), "Bearer transient-token") == 0;
    evidence->saw_resolved_url = strcmp(request->url,
        "https://api.example.test/health") == 0;
    assert(cancel_requested != NULL && !*cancel_requested);
    assert(umi_web_workbench_response_set_body(out_response,
        "{\"status\":\"ok\"}", 15U, 15U) == UMI_STATUS_OK);
    umi_web_workbench_response_complete(out_response, UMI_STATUS_OK, 200,
        &timing, true, NULL);
    return UMI_STATUS_OK;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWebWorkbenchRuntime *runtime = NULL;
    UmiWebWorkbenchEnvironment *environment;
    UmiWebWorkbenchAuthCatalogue *auth;
    UmiWebWorkbenchAuthProfile profile;
    UmiWebWorkbenchTransport transport;
    UmiWebWorkbenchRequest request;
    UmiWebWorkbenchResponse response;
    UmiWebWorkbenchRuntimeSnapshot snapshot;
    UmiWebWorkbenchHistory *history;
    TransportEvidence evidence = {0, false, false};
    assert(umi_web_workbench_runtime_create(&runtime) == UMI_STATUS_OK);
    environment = umi_web_workbench_runtime_environment(runtime);
    auth = umi_web_workbench_runtime_auth(runtime);
    assert(environment != NULL && auth != NULL);
    assert(umi_web_workbench_environment_set(environment, "baseUrl",
        "https://api.example.test", true) == UMI_STATUS_OK);
    umi_web_workbench_auth_profile_init(&profile, "oauth", "OAuth",
        UMI_WEB_WORKBENCH_AUTH_BEARER, "secret://oauth/token");
    assert(umi_web_workbench_auth_catalogue_upsert(auth, &profile) == UMI_STATUS_OK);
    umi_web_workbench_request_init(&request, "health", "Health",
        UMI_HTTP_METHOD_GET, "{{baseUrl}}/health");
    assert(umi_web_workbench_copy_text(request.auth_profile_id,
        sizeof(request.auth_profile_id), "oauth") == UMI_STATUS_OK);
    umi_web_workbench_transport_init(&transport, "stub", execute_stub, &evidence);
    assert(umi_web_workbench_runtime_execute(runtime, &transport, &request,
        "transient-token", &response) == UMI_STATUS_OK);
    assert(evidence.calls == 1 && evidence.saw_auth && evidence.saw_resolved_url);
    assert(umi_web_workbench_runtime_snapshot(runtime, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.execution_count == 1U && snapshot.history_count == 1U);
    history = umi_web_workbench_runtime_history(runtime);
    assert(history != NULL);
    assert(strstr(history->entries[0].url, "transient-token") == NULL);
    assert(strstr(request.url, "transient-token") == NULL);
    umi_web_workbench_runtime_destroy(runtime);
    return 0;
}
