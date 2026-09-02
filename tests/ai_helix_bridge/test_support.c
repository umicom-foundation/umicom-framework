/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_support.c
 *
 * PURPOSE:
 *   Implement a deterministic provider registered through the existing UmiAiRuntime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_support.h"
#include <string.h>

/*
 * Exercise copy text and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus copy_text(char *output, size_t capacity, const char *text)
{
    const size_t length = text != NULL ? strlen(text) : 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (output == NULL || capacity == 0U || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(output, text, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Exercise test generate and return a clear result when the behaviour no longer matches
 * its contract.
 */
static UmiStatus test_generate(void *instance,
                               const UmiAiRequest *request,
                               UmiAiResponse *response)
{
    TestAiProviderState *state = (TestAiProviderState *)instance;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL || request == NULL || response == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    state->last_request = *request;
    ++state->call_count;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (state->generate_status != UMI_STATUS_OK) return state->generate_status;
    memset(response, 0, sizeof(*response));
    status = copy_text(response->request_id, sizeof(response->request_id), request->request_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(response->provider_id, sizeof(response->provider_id), "test.helix");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(response->model_id, sizeof(response->model_id), request->model_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(response->text, sizeof(response->text), state->response_text);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    response->finish_reason = state->finish_reason;
    response->usage.input_tokens = 10U;
    response->usage.output_tokens = 20U;
    response->usage.total_tokens = 30U;
    return UMI_STATUS_OK;
}

/*
 * Exercise test ai provider set response and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus test_ai_provider_set_response(TestAiProviderState *state,
                                        const char *response_text)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return copy_text(state->response_text, sizeof(state->response_text), response_text);
}

/*
 * Exercise test ai runtime prepare and return a clear result when the behaviour no longer
 * matches its contract.
 */
UmiStatus test_ai_runtime_prepare(UmiAiRuntime *runtime,
                                  TestAiProviderState *state,
                                  const char *response_text)
{
    UmiAiProvider provider;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || state == NULL || response_text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    memset(state, 0, sizeof(*state));
    state->generate_status = UMI_STATUS_OK;
    state->finish_reason = UMI_AI_FINISH_STOP;
    status = test_ai_provider_set_response(state, response_text);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    umi_ai_runtime_init(runtime);
    memset(&provider, 0, sizeof(provider));
    provider.structure_size = (uint32_t)sizeof(provider);
    provider.abi_version = 1U;
    provider.provider_id = "test.helix";
    provider.kind = UMI_AI_PROVIDER_TEST;
    provider.instance = state;
    provider.generate = test_generate;
    return umi_ai_provider_registry_add(&runtime->providers, &provider);
}

/*
 * Exercise test bridge request and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_bridge_request(UmiAiHelixAgentRequest *request,
                         UmiHelixAgentRole role)
{
    umi_ai_helix_agent_request_init(request);
    request->role = role;
    (void)copy_text(request->operation_id, sizeof(request->operation_id), "operation-1");
    (void)copy_text(request->objective, sizeof(request->objective), "Improve the bounded implementation safely");
    (void)copy_text(request->context, sizeof(request->context), "build is currently green");
    (void)copy_text(request->target_hint, sizeof(request->target_hint), "src/example.c");
}
