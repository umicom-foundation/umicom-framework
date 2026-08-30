/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_ai_runtime_catalogue.c
 *
 * PURPOSE:
 *   Verify provider/model discovery and deterministic runtime selection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/ai/runtime_catalogue.h"

static UmiAiRuntimeDescriptor descriptor(const char *id,
                                         const char *provider,
                                         uint32_t context_tokens,
                                         int local)
{
    UmiAiRuntimeDescriptor value = {0};
    (void)strcpy(value.runtime_id, id);
    (void)strcpy(value.provider_id, provider);
    (void)strcpy(value.display_name, id);
    (void)strcpy(value.model_id, "model.chat");
    value.provider_kind = local ? UMI_AI_PROVIDER_LOCAL : UMI_AI_PROVIDER_REMOTE;
    value.transport = local ? UMI_AI_RUNTIME_LIBRARY : UMI_AI_RUNTIME_HTTP;
    value.capabilities = UMI_AI_RUNTIME_CHAT | UMI_AI_RUNTIME_STREAMING;
    value.context_tokens = context_tokens;
    value.max_output_tokens = 1024U;
    value.configured = 1;
    value.local = local;
    value.health.available = 1;
    return value;
}

int main(void)
{
    UmiAiRuntimeCatalogue *catalogue = NULL;
    UmiAiRuntimeDescriptor selected;
    UmiAiRuntimeDescriptor local = descriptor(
        "runtime.local", "provider.local", 8192U, 1);
    UmiAiRuntimeDescriptor remote = descriptor(
        "runtime.remote", "provider.remote", 4096U, 0);
    assert(umi_ai_runtime_catalogue_create(&catalogue) == UMI_STATUS_OK);
    assert(umi_ai_runtime_catalogue_upsert(catalogue, &local) == UMI_STATUS_OK);
    assert(umi_ai_runtime_catalogue_upsert(catalogue, &remote) == UMI_STATUS_OK);
    assert(umi_ai_runtime_catalogue_provider_count(catalogue) == 2U);
    assert(umi_ai_runtime_catalogue_select(
        catalogue, UMI_AI_RUNTIME_CHAT, 2048U, 0, &selected) == UMI_STATUS_OK);
    assert(strcmp(selected.runtime_id, "runtime.remote") == 0);
    assert(umi_ai_runtime_catalogue_select(
        catalogue, UMI_AI_RUNTIME_CHAT, 2048U, 1, &selected) == UMI_STATUS_OK);
    assert(strcmp(selected.runtime_id, "runtime.local") == 0);
    umi_ai_runtime_catalogue_destroy(catalogue);
    return 0;
}
