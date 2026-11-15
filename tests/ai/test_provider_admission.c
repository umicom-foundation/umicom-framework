/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai/test_provider_admission.c
 *
 * PURPOSE:
 *   Verify current provider policy, dispatch refusal and checked runtime ownership through public APIs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "../support/assertions.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/ai/runtime.h"

typedef struct Calls {
    unsigned generated;
    unsigned destroyed;
    UmiStatus result;
} Calls;
static UmiStatus Generate(void *instance, const UmiAiRequest *request, UmiAiResponse *response)
{
    Calls *calls = instance;
    assert(request != NULL && response != NULL);
    ++calls->generated;
    if (calls->result != UMI_STATUS_OK) return calls->result;
    umi_ai_response_init(response);
    return umi_ai_response_set_text(response, "reply");
}
static void Destroy(void *instance) { ++((Calls *)instance)->destroyed; }

int main(void)
{
    UmiAiRuntime runtime, before;
    Calls calls = {0U, 0U, UMI_STATUS_OK};
    UmiAiProvider provider = {0}, sentinel = {0}, *selected = &sentinel;
    UmiAiRequest *request = malloc(sizeof(*request));
    UmiAiResponse *response = malloc(sizeof(*response)), *untouched = malloc(sizeof(*untouched));
    assert(request != NULL && response != NULL && untouched != NULL);
    umi_ai_request_init(request);
    memset(response, 0x5a, sizeof(*response));
    memcpy(untouched, response, sizeof(*untouched));
    umi_ai_runtime_init(&runtime);
    provider.structure_size = (uint32_t)sizeof(provider);
    provider.abi_version = 1U; provider.provider_id = "provider";
    provider.kind = UMI_AI_PROVIDER_LOCAL;
    provider.instance = &calls; provider.generate = Generate; provider.destroy = Destroy;
    assert(umi_ai_provider_registry_add(&runtime.providers, &provider) == UMI_STATUS_OK);
    assert(UmiAiRuntimeAdmitProvider(NULL, "provider", 1, &selected) == UMI_STATUS_INVALID_ARGUMENT);
    assert(UmiAiRuntimeAdmitProvider(&runtime, NULL, 1, &selected) == UMI_STATUS_INVALID_ARGUMENT);
    assert(UmiAiRuntimeAdmitProvider(&runtime, "provider", 1, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    assert(UmiAiRuntimeAdmitProvider(&runtime, "missing", 1, &selected) == UMI_STATUS_NOT_FOUND);
    assert(selected == &sentinel && calls.generated == 0U);
    assert(UmiAiRuntimeAdmitProvider(&runtime, "provider", 0, &selected) == UMI_STATUS_OK);
    assert(selected == &runtime.providers.providers[0] && calls.generated == 0U);
    /* Admission is not a cached authorization. Revocation blocks the next
     * dispatch even though the borrowed table was obtained successfully. */
    runtime.policy.allow_local = 0;
    selected = &sentinel;
    assert(UmiAiRuntimeAdmitProvider(&runtime, "provider", 1, &selected) == UMI_STATUS_PERMISSION_DENIED);
    assert(selected == &sentinel);
    assert(umi_ai_runtime_generate(&runtime, "provider", 1, request, response) == UMI_STATUS_PERMISSION_DENIED);
    assert(calls.generated == 0U && memcmp(response, untouched, sizeof(*response)) == 0);
    runtime.providers.providers[0].kind = UMI_AI_PROVIDER_REMOTE;
    runtime.policy.allow_remote = 1;
    assert(umi_ai_runtime_generate(&runtime, "provider", 0, request, response) == UMI_STATUS_PERMISSION_DENIED);
    assert(calls.generated == 0U && memcmp(response, untouched, sizeof(*response)) == 0);
    assert(umi_ai_runtime_generate(&runtime, "provider", 1, request, response) == UMI_STATUS_OK);
    assert(calls.generated == 1U && strcmp(response->text, "reply") == 0);
    calls.result = UMI_STATUS_IO_ERROR;
    assert(umi_ai_runtime_generate(&runtime, "provider", 1, request, response) == UMI_STATUS_IO_ERROR);
    assert(calls.generated == 2U);
    runtime.providers.providers[0].generate = NULL;
    selected = &sentinel;
    assert(UmiAiRuntimeAdmitProvider(&runtime, "provider", 1, &selected) == UMI_STATUS_INVALID_STATE);
    assert(selected == &sentinel);
    assert(umi_ai_runtime_generate(&runtime, "provider", 1, request, response) == UMI_STATUS_INVALID_STATE);
    assert(calls.generated == 2U);
    runtime.providers.providers[0].generate = Generate;
    runtime.providers.count = SIZE_MAX;
    memcpy(&before, &runtime, sizeof(before));
    assert(UmiAiRuntimeAdmitProvider(&runtime, "provider", 1, &selected) == UMI_STATUS_INVALID_STATE);
    assert(UmiAiRuntimeRelease(&runtime) == UMI_STATUS_INVALID_STATE);
    umi_ai_runtime_destroy(&runtime);
    assert(calls.destroyed == 0U && memcmp(&before, &runtime, sizeof(before)) == 0);
    runtime.providers.count = 1U; /* Restore only the deliberately injected fixture fault. */
    assert(UmiAiRuntimeRelease(&runtime) == UMI_STATUS_OK);
    assert(calls.destroyed == 1U && runtime.providers.count == 0U && runtime.tools.count == 0U);
    assert(UmiAiRuntimeRelease(&runtime) == UMI_STATUS_OK && calls.destroyed == 1U);
    assert(UmiAiRuntimeRelease(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    free(untouched); free(response); free(request);
    return 0;
}
