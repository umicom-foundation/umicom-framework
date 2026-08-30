/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_provider.c
 *
 * PURPOSE:
 *   Validate the AI and Helix foundation introduced by Batch 10.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This focused executable keeps one contract easy to diagnose when the larger test suite reports a failure.
 */



#include <assert.h>
#include <string.h>
#include "umicom/umicom.h"

static UmiStatus generate(void *instance, const UmiAiRequest *request, UmiAiResponse *response)
{
    (void)instance;
    (void)request;
    umi_ai_response_init(response);
    return umi_ai_response_set_text(response, "ok");
}

int main(void)
{
    UmiAiRuntime runtime;
    UmiAiProvider provider = {0};
    UmiAiRequest request;
    UmiAiResponse response;
    umi_ai_runtime_init(&runtime);
    provider.structure_size = (uint32_t)sizeof(provider);
    provider.abi_version = 1U;
    provider.provider_id = "test";
    provider.kind = UMI_AI_PROVIDER_TEST;
    provider.generate = generate;
    assert(umi_ai_provider_registry_add(&runtime.providers, &provider) == UMI_STATUS_OK);
    umi_ai_request_init(&request);
    assert(umi_ai_runtime_generate(&runtime, "test", 1, &request, &response) == UMI_STATUS_OK);
    assert(strcmp(response.text, "ok") == 0);
    return 0;
}
