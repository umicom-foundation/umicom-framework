/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_runtime_health_routing.c
 *
 * PURPOSE:
 *   Verify descriptor-aware health routing, partial publication and shared refresh reentry ownership.
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

typedef struct HealthFixture {
    UmiAiAuthorEngineService *service;
    const char *mode;
    unsigned calls, nested_calls, legacy_calls;
} HealthFixture;

static UmiStatus nested_probe(void *context, const UmiAiRuntimeDescriptor *descriptor, UmiAiHealth *health)
{
    HealthFixture *fixture = context;
    (void)descriptor; (void)health;
    ++fixture->nested_calls;
    return UMI_STATUS_OK;
}

static UmiStatus scoped_probe(void *context, const UmiAiRuntimeDescriptor *descriptor, UmiAiHealth *health)
{
    HealthFixture *fixture = context;
    ++fixture->calls;
    assert(fixture->calls <= 2U);
    assert(descriptor->revision != 0U);
    assert(strcmp(health->provider_id, descriptor->provider_id) == 0);
    assert(health->checked_at_ns == 0U && !health->available);
    if (strcmp(fixture->mode, "timeout") == 0) return UMI_STATUS_TIMEOUT;
    if (strcmp(fixture->mode, "cancel") == 0) return UMI_STATUS_CANCELLED;
    if (strcmp(fixture->mode, "partial") == 0 && fixture->calls == 2U) return UMI_STATUS_TIMEOUT;
    if (strcmp(fixture->mode, "replace") == 0) {
        UmiAiRuntimeDescriptor replacement = *descriptor;
        strcpy(replacement.model_id, "model.replaced");
        assert(umi_ai_authorengine_service_register_runtime(fixture->service, &replacement) == UMI_STATUS_OK);
    } else if (strcmp(fixture->mode, "append") == 0) {
        UmiAiRuntimeDescriptor added = *descriptor;
        strcpy(added.runtime_id, "runtime.added");
        assert(umi_ai_authorengine_service_register_runtime(fixture->service, &added) == UMI_STATUS_OK);
    } else if (strcmp(fixture->mode, "reenter-runtime") == 0) {
        size_t count = SIZE_MAX;
        assert(UmiAiAuthorEngineRefreshRuntimeHealth(fixture->service, 999U,
            nested_probe, fixture, &count) == UMI_STATUS_BUSY);
        assert(count == SIZE_MAX && fixture->nested_calls == 0U);
    } else if (strcmp(fixture->mode, "reenter-legacy") == 0) {
        size_t count = SIZE_MAX;
        assert(umi_ai_authorengine_service_refresh_health(fixture->service, 999U,
            &count) == UMI_STATUS_BUSY);
        assert(count == SIZE_MAX && fixture->legacy_calls == 0U);
    }
    health->available = 1;
    strcpy(health->message, "Explicit fixture observation");
    if (strcmp(fixture->mode, "routes") == 0 || strcmp(fixture->mode, "same-model") == 0) {
        int first = strcmp(descriptor->runtime_id, "runtime.first") == 0;
        assert(strcmp(descriptor->endpoint, first
            ? "http://127.0.0.1:18080/v1/chat/completions"
            : "http://127.0.0.1:18081/v1/chat/completions") == 0);
        assert(strcmp(descriptor->model_id, first || strcmp(fixture->mode, "same-model") == 0
            ? "model.first" : "model.second") == 0);
        assert(descriptor->configured && descriptor->local);
        health->available = first;
    } else if (strcmp(fixture->mode, "unconfigured") == 0) {
        assert(!descriptor->configured);
        health->available = 0;
    } else if (strcmp(fixture->mode, "unavailable") == 0) {
        health->available = 0;
    } else if (strcmp(fixture->mode, "malformed") == 0) {
        memset(health->message, 'x', sizeof(health->message));
    } else if (strcmp(fixture->mode, "provider-mismatch") == 0) {
        strcpy(health->provider_id, "provider.other");
    }
    /* The service supplies its observation timestamp, not a callback's value. */
    health->checked_at_ns = 777U;
    return UMI_STATUS_OK;
}

static UmiStatus null_context_probe(void *context, const UmiAiRuntimeDescriptor *descriptor, UmiAiHealth *health)
{
    assert(context == NULL && descriptor != NULL && health != NULL);
    health->available = 0;
    return UMI_STATUS_OK;
}

static UmiStatus never_generate(void *context, const UmiAiRequest *request, UmiAiResponse *response)
{
    (void)context; (void)request; (void)response;
    assert(!"health must not generate text");
    return UMI_STATUS_NOT_IMPLEMENTED;
}

static UmiStatus legacy_probe(void *context, UmiAiHealth *health)
{
    HealthFixture *fixture = context;
    size_t count = SIZE_MAX;
    ++fixture->legacy_calls;
    assert(UmiAiAuthorEngineRefreshRuntimeHealth(fixture->service, 999U,
        nested_probe, fixture, &count) == UMI_STATUS_BUSY);
    assert(count == SIZE_MAX && fixture->nested_calls == 0U);
    health->available = 1;
    return UMI_STATUS_OK;
}

int main(int argc, char **argv)
{
    static const char *const modes[] = {"null-service", "null-callback", "null-output",
        "empty", "routes", "same-model", "unconfigured", "unavailable", "timeout",
        "cancel", "malformed", "provider-mismatch", "replace", "append",
        "reenter-runtime", "reenter-legacy", "legacy-reenter", "partial", "null-context"};
    UmiAiRuntime runtime;
    UmiAiAuthorEngineConfig config = {0};
    UmiAiAuthorEngineService *service = NULL;
    UmiAiRuntimeDescriptor first = catalogue_fixture("runtime.first"), second;
    UmiAiRuntimeDescriptor before, after, second_before, second_after;
    HealthFixture fixture = {0};
    UmiAiProvider provider = {0};
    size_t healthy = SIZE_MAX, index;
    UmiStatus status;
    int known = 0, two, invalid, failed;
    assert(argc == 2);
    for (index = 0U; index < sizeof modes / sizeof modes[0]; ++index)
        if (strcmp(argv[1], modes[index]) == 0) known = 1;
    assert(known);
    fixture.mode = argv[1];
    umi_ai_runtime_init(&runtime);
    strcpy(config.executable, "uaengine"); strcpy(config.workspace, ".");
    strcpy(config.provider, "provider.local");
    assert(umi_ai_authorengine_service_create(&runtime, &config, &service) == UMI_STATUS_OK);
    fixture.service = service;
    strcpy(first.model_id, "model.first");
    strcpy(first.endpoint, "http://127.0.0.1:18080/v1/chat/completions");
    if (strcmp(fixture.mode, "unconfigured") == 0) first.configured = 0;
    two = strcmp(fixture.mode, "routes") == 0 || strcmp(fixture.mode, "same-model") == 0 ||
        strcmp(fixture.mode, "partial") == 0;
    if (strcmp(fixture.mode, "empty") != 0) {
        assert(umi_ai_authorengine_service_register_runtime(service, &first) == UMI_STATUS_OK);
        assert(umi_ai_runtime_catalogue_find(umi_ai_authorengine_service_catalogue(service),
            first.runtime_id, &before) == UMI_STATUS_OK);
    }
    if (two) {
        second = first;
        strcpy(second.runtime_id, "runtime.second");
        strcpy(second.endpoint, "http://127.0.0.1:18081/v1/chat/completions");
        if (strcmp(fixture.mode, "same-model") != 0) strcpy(second.model_id, "model.second");
        assert(umi_ai_authorengine_service_register_runtime(service, &second) == UMI_STATUS_OK);
        assert(umi_ai_runtime_catalogue_find(umi_ai_authorengine_service_catalogue(service),
            second.runtime_id, &second_before) == UMI_STATUS_OK);
    }
    /* Install a provider only for the reentry cases. Scoped routing otherwise
     * proves that no provider-wide instance is required or implicitly called. */
    if (strcmp(fixture.mode, "reenter-legacy") == 0 || strcmp(fixture.mode, "legacy-reenter") == 0) {
        provider.structure_size = sizeof provider; provider.abi_version = 1U;
        provider.provider_id = "provider.local"; provider.kind = UMI_AI_PROVIDER_LOCAL;
        provider.instance = &fixture; provider.generate = never_generate; provider.health = legacy_probe;
        assert(umi_ai_provider_registry_add(&runtime.providers, &provider) == UMI_STATUS_OK);
    }
    invalid = strncmp(fixture.mode, "null-", 5U) == 0 && strcmp(fixture.mode, "null-context") != 0;
    if (strcmp(fixture.mode, "legacy-reenter") == 0)
        status = umi_ai_authorengine_service_refresh_health(service, 123U, &healthy);
    else
        status = UmiAiAuthorEngineRefreshRuntimeHealth(
            strcmp(fixture.mode, "null-service") == 0 ? NULL : service, 123U,
            strcmp(fixture.mode, "null-callback") == 0 ? NULL :
                strcmp(fixture.mode, "null-context") == 0 ? null_context_probe : scoped_probe,
            strcmp(fixture.mode, "null-context") == 0 ? NULL : &fixture,
            strcmp(fixture.mode, "null-output") == 0 ? NULL : &healthy);
    failed = strcmp(fixture.mode, "timeout") == 0 || strcmp(fixture.mode, "cancel") == 0 ||
        strcmp(fixture.mode, "malformed") == 0 || strcmp(fixture.mode, "provider-mismatch") == 0 ||
        strcmp(fixture.mode, "replace") == 0 || strcmp(fixture.mode, "partial") == 0;
    if (invalid) {
        assert(status == UMI_STATUS_INVALID_ARGUMENT && healthy == SIZE_MAX && fixture.calls == 0U);
    } else if (failed) {
        UmiStatus expected = strcmp(fixture.mode, "replace") == 0 ? UMI_STATUS_BUSY :
            strcmp(fixture.mode, "cancel") == 0 ? UMI_STATUS_CANCELLED :
            strcmp(fixture.mode, "timeout") == 0 || strcmp(fixture.mode, "partial") == 0
                ? UMI_STATUS_TIMEOUT : UMI_STATUS_INVALID_ARGUMENT;
        assert(status == expected && healthy == SIZE_MAX);
    } else {
        size_t expected = strcmp(fixture.mode, "empty") == 0 || strcmp(fixture.mode, "unavailable") == 0 ||
            strcmp(fixture.mode, "unconfigured") == 0 || strcmp(fixture.mode, "null-context") == 0 ? 0U : 1U;
        assert(status == UMI_STATUS_OK && healthy == expected);
    }
    if (strcmp(fixture.mode, "empty") != 0) {
        assert(umi_ai_runtime_catalogue_find(umi_ai_authorengine_service_catalogue(service),
            first.runtime_id, &after) == UMI_STATUS_OK);
        if (invalid || (failed && strcmp(fixture.mode, "replace") != 0 && strcmp(fixture.mode, "partial") != 0))
            assert(memcmp(&before, &after, sizeof before) == 0);
        else if (strcmp(fixture.mode, "replace") == 0)
            assert(strcmp(after.model_id, "model.replaced") == 0 && !after.health.available &&
                after.health.checked_at_ns == 0U && after.revision > before.revision);
        else
            assert(after.health.checked_at_ns == 123U && after.revision > before.revision);
    }
    if (two) {
        assert(umi_ai_runtime_catalogue_find(umi_ai_authorengine_service_catalogue(service),
            second.runtime_id, &second_after) == UMI_STATUS_OK);
        if (strcmp(fixture.mode, "partial") == 0)
            assert(memcmp(&second_before, &second_after, sizeof second_before) == 0 && after.health.available);
        else
            assert(second_after.health.checked_at_ns == 123U && !second_after.health.available);
    }
    if (strcmp(fixture.mode, "append") == 0) {
        assert(fixture.calls == 1U);
        assert(umi_ai_runtime_catalogue_find(umi_ai_authorengine_service_catalogue(service),
            "runtime.added", &after) == UMI_STATUS_OK);
        assert(after.health.checked_at_ns == 0U && !after.health.available);
    }
    if (strcmp(fixture.mode, "legacy-reenter") == 0)
        assert(fixture.legacy_calls == 1U && fixture.calls == 0U);
    if (failed || invalid) {
        /* Every return releases the refresh guard; retry uses fresh observations,
         * not a stale result copied onto the newer configuration. */
        fixture.mode = "retry"; fixture.calls = 0U;
        assert(UmiAiAuthorEngineRefreshRuntimeHealth(service, 456U, scoped_probe,
            &fixture, &healthy) == UMI_STATUS_OK);
        assert(healthy == (two ? 2U : 1U));
    }
    assert(fixture.nested_calls == 0U);
    umi_ai_authorengine_service_destroy(service);
    umi_ai_runtime_destroy(&runtime);
    return 0;
}
