/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/local_models/sdk_consumer/provider_ownership.c
 *
 * PURPOSE:
 *   Exercise installed generation and health ownership contracts without a model or network.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "provider_ownership.h"
#include "umicom/ai/authorengine_service.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct SdkOwnership {
    UmiAiRuntime runtime;
    UmiAiAuthorEngineService *service;
    UmiAiRequest request;
    UmiAiResponse response;
    unsigned generated;
    unsigned probed;
    unsigned destroyed;
    unsigned mode;
    int callback_failed;
    int registered;
} SdkOwnership;

/* All callbacks return synchronously. The heap fixture outlives the registry's
 * borrowed instance pointer, and only final runtime release consumes that owner.
 * No callback performs transport, process execution or inference. */
static void SdkDestroy(void *context)
{
    ++((SdkOwnership *)context)->destroyed;
}

static UmiStatus SdkGenerate(void *context, const UmiAiRequest *request,
    UmiAiResponse *response)
{
    SdkOwnership *state = context;
    ++state->generated;
    if (request != &state->request ||
        UmiAiProviderRegistryCheckIdle(&state->runtime.providers) != UMI_STATUS_BUSY ||
        UmiAiRuntimeCheckIdle(&state->runtime) != UMI_STATUS_BUSY ||
        UmiAiRuntimeRelease(&state->runtime) != UMI_STATUS_BUSY ||
        state->destroyed != 0U) state->callback_failed = 1;
    umi_ai_response_init(response);
    strcpy(response->request_id, request->request_id);
    strcpy(response->provider_id, "sdk.ownership");
    strcpy(response->model_id, request->model_id);
    strcpy(response->text, "Synthetic SDK reply; no model executed");
    response->finish_reason = UMI_AI_FINISH_STOP;
    if (state->mode == 2U) memset(response->text, 'x', sizeof response->text);
    return state->mode == 1U ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}

static UmiStatus SdkHealth(void *context, UmiAiHealth *health)
{
    SdkOwnership *state = context;
    ++state->probed;
    if (UmiAiAuthorEngineServiceCheckIdle(state->service) != UMI_STATUS_BUSY ||
        UmiAiAuthorEngineServiceDestroyChecked(state->service) != UMI_STATUS_BUSY ||
        UmiAiRuntimeRelease(&state->runtime) != UMI_STATUS_BUSY ||
        state->destroyed != 0U) state->callback_failed = 1;
    /* The observation deliberately remains unavailable: testing lifetime is
     * not evidence that any actual local engine is healthy. */
    health->available = 0;
    strcpy(health->message, "Offline SDK ownership exercise");
    return UMI_STATUS_OK;
}

static UmiStatus SdkScopedHealth(void *context,
    const UmiAiRuntimeDescriptor *descriptor, UmiAiHealth *health)
{
    if (strcmp(descriptor->runtime_id, "sdk.ownership.runtime") != 0 ||
        strcmp(descriptor->provider_id, "sdk.ownership") != 0)
        return UMI_STATUS_INVALID_STATE;
    return SdkHealth(context, health);
}

static UmiStatus SdkRequire(int condition, const char *phase)
{
    if (condition) return UMI_STATUS_OK;
    (void)fprintf(stderr, "Installed SDK ownership check failed: %s\n", phase);
    return UMI_STATUS_INVALID_STATE;
}

/* The separate operation lets the outer owner perform checked cleanup on every
 * ordinary failure. Output bytes, dispatch flags and cleanup counts are checked
 * together so a provider's error cannot look like a newly published response. */
static UmiStatus SdkExercise(SdkOwnership *state)
{
    UmiAiProvider provider = {0};
    provider.structure_size = (uint32_t)sizeof provider;
    provider.abi_version = 1U;
    provider.provider_id = "sdk.ownership";
    provider.kind = UMI_AI_PROVIDER_LOCAL;
    provider.instance = state;
    provider.generate = SdkGenerate;
    provider.health = SdkHealth;
    provider.destroy = SdkDestroy;
    UmiStatus status = umi_ai_provider_registry_add(&state->runtime.providers, &provider);
    if (status != UMI_STATUS_OK) return status;
    state->registered = 1;

    umi_ai_request_init(&state->request);
    strcpy(state->request.request_id, "sdk.ownership.request");
    strcpy(state->request.model_id, "synthetic");
    status = UmiAiRequestValidate(&state->request);
    if (status != UMI_STATUS_OK) return status;
    umi_ai_response_init(&state->response);
    strcpy(state->response.text, "Previously displayed reply");
    UmiAiResponse previous;
    memcpy(&previous, &state->response, sizeof previous);
    UmiAiGenerationReport report;

    state->request.message_count = UMI_AI_MAX_MESSAGES + 1U;
    status = UmiAiRuntimeGenerateChecked(&state->runtime, provider.provider_id, 1,
        &state->request, &state->response, &report);
    state->request.message_count = 0U;
    if (SdkRequire(status == UMI_STATUS_CAPACITY_EXCEEDED &&
        report.status == status && !report.dispatched && !report.response_checked &&
        !report.response_published && state->generated == 0U &&
        memcmp(&previous, &state->response, sizeof previous) == 0,
        "bounded request refuses before dispatch") != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;

    for (unsigned mode = 0U; mode < 3U; ++mode) {
        state->mode = mode;
        memcpy(&previous, &state->response, sizeof previous);
        status = UmiAiRuntimeGenerateChecked(&state->runtime, provider.provider_id, 1,
            &state->request, &state->response, &report);
        UmiStatus expected = mode == 0U ? UMI_STATUS_OK :
            mode == 1U ? UMI_STATUS_IO_ERROR : UMI_STATUS_INTERNAL_ERROR;
        if (SdkRequire(status == expected && report.status == expected &&
            report.dispatched && !state->callback_failed &&
            state->generated == mode + 1U &&
            UmiAiRuntimeCheckIdle(&state->runtime) == UMI_STATUS_OK,
            "generation result and owner recovery") != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;
        if (mode == 0U) {
            if (SdkRequire(report.provider_status == UMI_STATUS_OK &&
                report.response_checked && report.response_published &&
                report.response_status == UMI_STATUS_OK &&
                UmiAiResponseValidate(&state->response) == UMI_STATUS_OK &&
                strcmp(state->response.text, "Synthetic SDK reply; no model executed") == 0,
                "valid staged reply published") != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;
        } else {
            if (SdkRequire(!report.response_published &&
                memcmp(&previous, &state->response, sizeof previous) == 0 &&
                report.provider_status == (mode == 1U ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK) &&
                report.response_checked == (mode == 2U) &&
                report.response_status == (mode == 1U ? UMI_STATUS_UNAVAILABLE : UMI_STATUS_INVALID_ARGUMENT),
                "failed reply retains previous output") != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;
        }
    }

    UmiAiAuthorEngineConfig config = {0};
    strcpy(config.executable, "uaengine");
    strcpy(config.workspace, ".");
    strcpy(config.provider, provider.provider_id);
    status = umi_ai_authorengine_service_create(&state->runtime, &config, &state->service);
    if (status != UMI_STATUS_OK) return status;
    UmiAiRuntimeDescriptor descriptor = {0};
    strcpy(descriptor.runtime_id, "sdk.ownership.runtime");
    strcpy(descriptor.provider_id, provider.provider_id);
    strcpy(descriptor.display_name, "Offline SDK ownership example");
    strcpy(descriptor.model_id, "synthetic");
    descriptor.provider_kind = UMI_AI_PROVIDER_LOCAL;
    descriptor.transport = UMI_AI_RUNTIME_IN_PROCESS;
    descriptor.capabilities = UMI_AI_RUNTIME_CHAT;
    descriptor.context_tokens = 2048U;
    descriptor.max_output_tokens = 128U;
    descriptor.configured = 1;
    descriptor.local = 1;
    status = umi_ai_authorengine_service_register_runtime(state->service, &descriptor);
    if (status != UMI_STATUS_OK) return status;
    size_t healthy = SIZE_MAX;
    status = umi_ai_authorengine_service_refresh_health(state->service, 1U, &healthy);
    if (status != UMI_STATUS_OK) return status;
    if (SdkRequire(healthy == 0U && state->probed == 1U && !state->callback_failed &&
        UmiAiAuthorEngineServiceCheckIdle(state->service) == UMI_STATUS_OK,
        "provider health retains service during callback") != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;
    healthy = SIZE_MAX;
    status = UmiAiAuthorEngineRefreshRuntimeHealth(state->service, 2U,
        SdkScopedHealth, state, &healthy);
    if (status != UMI_STATUS_OK) return status;
    if (SdkRequire(healthy == 0U && state->probed == 2U && !state->callback_failed &&
        UmiAiAuthorEngineServiceCheckIdle(state->service) == UMI_STATUS_OK &&
        UmiAiRuntimeCheckIdle(&state->runtime) == UMI_STATUS_OK,
        "descriptor health retires both activity guards") != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;

    state->mode = 0U;
    status = UmiAiRuntimeGenerateChecked(&state->runtime, provider.provider_id, 1,
        &state->request, &state->response, &report);
    if (status != UMI_STATUS_OK) return status;
    return SdkRequire(report.response_published && state->generated == 4U &&
        !state->callback_failed && state->destroyed == 0U,
        "generation remains usable after health refresh");
}

UmiStatus UmiModelSdkExerciseOwnership(void)
{
    /* Requests have large bounded message storage; keep this fixture off the
     * application's limited stack. Only the example owns this allocation. */
    SdkOwnership *state = calloc(1U, sizeof *state);
    if (state == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_ai_runtime_init(&state->runtime);
    UmiStatus result = SdkExercise(state);
    UmiStatus service_release = UmiAiAuthorEngineServiceDestroyChecked(state->service);
    if (service_release != UMI_STATUS_OK) {
        /* A refused release retains the complete owner. Do not free storage
         * borrowed by it, even when this diagnostic application will exit. */
        (void)fprintf(stderr, "Installed SDK retained its service: %d\n", (int)service_release);
        return service_release;
    }
    state->service = NULL;
    UmiStatus runtime_release = UmiAiRuntimeRelease(&state->runtime);
    if (runtime_release != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Installed SDK retained its runtime: %d\n", (int)runtime_release);
        return runtime_release;
    }
    if (SdkRequire(state->destroyed == (state->registered ? 1U : 0U),
        "provider destroyed exactly once") != UMI_STATUS_OK) result = UMI_STATUS_INVALID_STATE;
    free(state);
    return result;
}
