/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/test_application_federation.c
 *
 * PURPOSE:
 *   Verify capability routing, declaration enforcement and policy mediation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/application/application.h"

static UmiStatus handle_ai(void *context,
                           const UmiFederationRequest *request,
                           UmiFederationResponse *response)
{
    const char *answer = "framework-routed";
    int *calls = (int *)context;
    *calls += 1;
    assert(strcmp(request->operation_id, "chat.complete") == 0);
    if (response != NULL && response->payload != NULL &&
        response->capacity >= strlen(answer) + 1U) {
        (void)memcpy(response->payload, answer, strlen(answer) + 1U);
        response->payload_size = strlen(answer) + 1U;
    }
    return UMI_STATUS_OK;
}

static int deny_all(void *context, const UmiFederationRequest *request,
                    const char *provider_application_id)
{
    (void)context;
    (void)request;
    (void)provider_application_id;
    return 0;
}

int main(void)
{
    UmiFederationRouter *router = NULL;
    const UmiApplicationDefinition *studio =
        umi_application_portfolio_find("org.umicom.studio");
    const UmiApplicationDefinition *llm =
        umi_application_portfolio_find("org.umicom.llm");
    UmiFederationRequest request = {0};
    UmiFederationResponse response = {0};
    UmiFederationStats stats;
    char payload[64] = {0};
    int calls = 0;

    assert(umi_federation_router_create(&router) == UMI_STATUS_OK);
    assert(umi_federation_router_register_application(router, studio) ==
           UMI_STATUS_OK);
    assert(umi_federation_router_register_application(router, llm) ==
           UMI_STATUS_OK);
    assert(umi_federation_router_find_application(
        router, "org.umicom.llm") == llm);
    assert(umi_federation_router_find_application(
        router, "org.umicom.unknown") == NULL);
    assert(umi_federation_router_register_provider(
        router, llm->application_id, "umicom.ai", handle_ai, &calls) ==
        UMI_STATUS_OK);
    assert(umi_federation_router_register_provider(
        router, llm->application_id, "umicom.trading", handle_ai, &calls) ==
        UMI_STATUS_PERMISSION_DENIED);

    request.structure_size = (uint32_t)sizeof(request);
    request.message_id = "message-1";
    request.source_application_id = studio->application_id;
    request.capability_id = "umicom.ai";
    request.operation_id = "chat.complete";
    request.schema_id = "umicom.ai.chat.v1";
    request.correlation_id = "correlation-1";
    request.kind = UMI_FEDERATION_QUERY;
    response.payload = payload;
    response.capacity = sizeof(payload);
    assert(umi_federation_router_dispatch(router, &request, &response) ==
           UMI_STATUS_OK);
    assert(calls == 1);
    assert(strcmp(payload, "framework-routed") == 0);
    assert(strcmp(response.provider_application_id, llm->application_id) == 0);

    umi_federation_router_set_policy(router, deny_all, NULL);
    assert(umi_federation_router_dispatch(router, &request, &response) ==
           UMI_STATUS_UNAVAILABLE);
    stats = umi_federation_router_stats(router);
    assert(stats.application_count == 2U);
    assert(stats.provider_count == 1U);
    assert(stats.dispatch_count == 1U);
    assert(stats.denied_count == 1U);
    umi_federation_router_destroy(router);
    return 0;
}
