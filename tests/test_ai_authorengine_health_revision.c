/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_authorengine_health_revision.c
 *
 * PURPOSE:
 *   Verify health callback publication stays bound to the runtime revision it observed.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "support/assertions.h"
#include <assert.h>
#include <string.h>
#include "umicom/ai/authorengine_service.h"
#include "ai/runtime_catalogue_fixture.h"

typedef struct ProbeFixture {
    UmiAiAuthorEngineService *service;
    const char *mode;
    unsigned calls;
} ProbeFixture;

static UmiStatus never_generate(void *instance, const UmiAiRequest *request, UmiAiResponse *response)
{
    (void)instance; (void)request; (void)response;
    assert(!"catalogue health must not generate text");
    return UMI_STATUS_NOT_IMPLEMENTED;
}

static UmiStatus probe(void *instance, UmiAiHealth *health)
{
    ProbeFixture *fixture = instance;
    UmiAiRuntimeDescriptor value = catalogue_fixture("runtime.local");
    ++fixture->calls;
    assert(fixture->calls == 1U);
    if (strcmp(fixture->mode, "replace") == 0) {
        strcpy(value.model_id, "model.replaced");
        assert(umi_ai_authorengine_service_register_runtime(fixture->service, &value) == UMI_STATUS_OK);
    } else if (strcmp(fixture->mode, "append") == 0) {
        strcpy(value.runtime_id, "runtime.appended");
        assert(umi_ai_authorengine_service_register_runtime(fixture->service, &value) == UMI_STATUS_OK);
    } else if (strcmp(fixture->mode, "provider-mismatch") == 0) {
        strcpy(health->provider_id, "provider.other");
    } else if (strcmp(fixture->mode, "malformed-health") == 0) {
        memset(health->message, 'x', sizeof(health->message));
    } else if (strcmp(fixture->mode, "probe-failure") == 0) {
        return UMI_STATUS_IO_ERROR;
    } else assert(strcmp(fixture->mode, "success") == 0);
    health->available = 1;
    return UMI_STATUS_OK;
}

int main(int argc, char **argv)
{
    UmiAiRuntime runtime;
    UmiAiAuthorEngineService *service = NULL;
    UmiAiAuthorEngineConfig config = {0};
    UmiAiRuntimeDescriptor value = catalogue_fixture("runtime.local"), before, after;
    UmiAiProvider provider = {0};
    ProbeFixture fixture = {0};
    size_t healthy = SIZE_MAX;
    UmiStatus status;
    assert(argc == 2); fixture.mode = argv[1];
    umi_ai_runtime_init(&runtime);
    strcpy(config.executable, "uaengine");
    strcpy(config.workspace, ".");
    strcpy(config.provider, "provider.local");
    assert(umi_ai_authorengine_service_create(&runtime, &config, &service) == UMI_STATUS_OK);
    fixture.service = service;
    provider.structure_size = sizeof(provider); provider.abi_version = 1U;
    provider.provider_id = "provider.local"; provider.kind = UMI_AI_PROVIDER_LOCAL;
    provider.instance = &fixture; provider.generate = never_generate; provider.health = probe;
    /* The fixture remains stack-owned and has no destroy callback. */
    assert(umi_ai_provider_registry_add(&runtime.providers, &provider) == UMI_STATUS_OK);
    assert(umi_ai_authorengine_service_register_runtime(service, &value) == UMI_STATUS_OK);
    assert(umi_ai_runtime_catalogue_find(umi_ai_authorengine_service_catalogue(service),
        value.runtime_id, &before) == UMI_STATUS_OK);
    status = umi_ai_authorengine_service_refresh_health(service, 123U, &healthy);
    assert(fixture.calls == 1U);
    assert(umi_ai_runtime_catalogue_find(umi_ai_authorengine_service_catalogue(service),
        value.runtime_id, &after) == UMI_STATUS_OK);
    if (strcmp(argv[1], "replace") == 0) {
        assert(status == UMI_STATUS_BUSY && healthy == SIZE_MAX);
        assert(after.revision > before.revision);
        assert(strcmp(after.model_id, "model.replaced") == 0);
        assert(!after.health.available && after.health.checked_at_ns == 0U);
    } else if (strcmp(argv[1], "provider-mismatch") == 0 || strcmp(argv[1], "malformed-health") == 0) {
        assert(status == UMI_STATUS_INVALID_ARGUMENT && healthy == SIZE_MAX);
        assert(memcmp(&before, &after, sizeof(before)) == 0);
    } else {
        assert(status == UMI_STATUS_OK);
        assert(after.health.checked_at_ns == 123U);
        assert(after.revision > before.revision);
        assert(healthy == (strcmp(argv[1], "probe-failure") == 0 ? 0U : 1U));
        if (strcmp(argv[1], "append") == 0) {
            assert(umi_ai_runtime_catalogue_count(umi_ai_authorengine_service_catalogue(service)) == 2U);
            assert(umi_ai_runtime_catalogue_find(umi_ai_authorengine_service_catalogue(service),
                "runtime.appended", &after) == UMI_STATUS_OK);
            assert(after.health.checked_at_ns == 0U && !after.health.available);
        }
    }
    umi_ai_authorengine_service_destroy(service);
    umi_ai_runtime_destroy(&runtime);
    return 0;
}
