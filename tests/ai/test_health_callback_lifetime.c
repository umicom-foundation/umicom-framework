/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai/test_health_callback_lifetime.c
 *
 * PURPOSE:
 *   Verify health callbacks retain their service and borrowed runtime through refusal and recovery.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/authorengine_service.h"
#include "runtime_catalogue_fixture.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); return 1; } } while (0)

typedef struct Fixture {
    UmiAiRuntime runtime;
    UmiAiAuthorEngineService *service, *other;
    UmiAiRequest request;
    const char *entry, *action, *outcome;
    unsigned calls, generated, destroyed;
    int failed;
} Fixture;

static UmiStatus scoped_probe(void *context, const UmiAiRuntimeDescriptor *descriptor, UmiAiHealth *health);

static UmiStatus refresh(Fixture *fixture, UmiAiAuthorEngineService *service, size_t *count)
{
    if (strcmp(fixture->entry, "scoped") == 0)
        return UmiAiAuthorEngineRefreshRuntimeHealth(service, 123U, scoped_probe, fixture, count);
    return umi_ai_authorengine_service_refresh_health(service, 123U, count);
}

static UmiStatus generate(void *context, const UmiAiRequest *request, UmiAiResponse *response)
{
    Fixture *fixture = context;
    (void)request;
    ++fixture->generated;
    if (strcmp(fixture->action, "health-from-generation") == 0) {
        size_t count = SIZE_MAX;
        if (refresh(fixture, fixture->service, &count) != UMI_STATUS_BUSY ||
            count != SIZE_MAX || fixture->calls != 0U ||
            UmiAiAuthorEngineServiceCheckIdle(fixture->service) != UMI_STATUS_OK)
            fixture->failed = 1;
    } else fixture->failed = 1;
    umi_ai_response_init(response);
    return UMI_STATUS_OK;
}

static void destroy(void *context) { ++((Fixture *)context)->destroyed; }

static UmiStatus probe(Fixture *fixture, UmiAiHealth *health)
{
    if (++fixture->calls > 1U) { fixture->failed = 1; return UMI_STATUS_INVALID_STATE; }
    UmiAiRuntime before;
    UmiAiAuthorEngineServiceSnapshot before_service = {0}, after_service = {0};
    memcpy(&before, &fixture->runtime, sizeof before);
    if (umi_ai_authorengine_service_snapshot(fixture->service, &before_service) != UMI_STATUS_OK)
        fixture->failed = 1;
    UmiStatus status = UMI_STATUS_BUSY;
    if (strcmp(fixture->action, "service-idle") == 0)
        status = UmiAiAuthorEngineServiceCheckIdle(fixture->service);
    else if (strcmp(fixture->action, "service-destroy") == 0)
        status = UmiAiAuthorEngineServiceDestroyChecked(fixture->service);
    else if (strcmp(fixture->action, "service-legacy") == 0)
        umi_ai_authorengine_service_destroy(fixture->service);
    else if (strcmp(fixture->action, "runtime-idle") == 0)
        status = UmiAiRuntimeCheckIdle(&fixture->runtime);
    else if (strcmp(fixture->action, "runtime-release") == 0)
        status = UmiAiRuntimeRelease(&fixture->runtime);
    else if (strcmp(fixture->action, "runtime-legacy") == 0)
        umi_ai_runtime_destroy(&fixture->runtime);
    else if (strcmp(fixture->action, "registry-add") == 0) {
        UmiAiProvider other = fixture->runtime.providers.providers[0];
        other.provider_id = "other"; other.instance = NULL; other.destroy = NULL;
        status = umi_ai_provider_registry_add(&fixture->runtime.providers, &other);
    } else if (strcmp(fixture->action, "generate") == 0) {
        UmiAiResponse response;
        UmiAiGenerationReport report;
        umi_ai_response_init(&response); strcpy(response.text, "previous");
        status = UmiAiRuntimeGenerateChecked(&fixture->runtime, "provider.local", 1,
            &fixture->request, &response, &report);
        if (report.dispatched || report.response_published || strcmp(response.text, "previous") != 0)
            fixture->failed = 1;
    } else if (strcmp(fixture->action, "other-service") == 0) {
        size_t count = SIZE_MAX;
        status = refresh(fixture, fixture->other, &count);
        if (count != SIZE_MAX || UmiAiAuthorEngineServiceCheckIdle(fixture->other) != UMI_STATUS_OK)
            fixture->failed = 1;
    } else fixture->failed = 1;
    if (status != UMI_STATUS_BUSY || fixture->destroyed != 0U ||
        UmiAiRuntimeCheckIdle(&fixture->runtime) != UMI_STATUS_BUSY ||
        UmiAiAuthorEngineServiceCheckIdle(fixture->service) != UMI_STATUS_BUSY ||
        memcmp(&before, &fixture->runtime, sizeof before) != 0 ||
        umi_ai_authorengine_service_snapshot(fixture->service, &after_service) != UMI_STATUS_OK ||
        memcmp(&before_service, &after_service, sizeof before_service) != 0)
        fixture->failed = 1;
    health->available = 1;
    strcpy(health->message, "Synthetic health observation");
    if (strcmp(fixture->outcome, "malformed") == 0)
        memset(health->message, 'x', sizeof health->message);
    return strcmp(fixture->outcome, "provider-error") == 0 ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}

static UmiStatus scoped_probe(void *context, const UmiAiRuntimeDescriptor *descriptor, UmiAiHealth *health)
{
    if (descriptor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return probe(context, health);
}
static UmiStatus legacy_probe(void *context, UmiAiHealth *health) { return probe(context, health); }

static int initialize(Fixture *fixture)
{
    umi_ai_runtime_init(&fixture->runtime);
    umi_ai_request_init(&fixture->request);
    UmiAiAuthorEngineConfig config = {0};
    strcpy(config.executable, "uaengine"); strcpy(config.workspace, ".");
    strcpy(config.provider, "provider.local");
    CHECK(umi_ai_authorengine_service_create(&fixture->runtime, &config, &fixture->service) == UMI_STATUS_OK);
    CHECK(umi_ai_authorengine_service_create(&fixture->runtime, &config, &fixture->other) == UMI_STATUS_OK);
    UmiAiProvider provider = {0};
    provider.structure_size = (uint32_t)sizeof provider; provider.abi_version = 1U;
    provider.provider_id = "provider.local"; provider.kind = UMI_AI_PROVIDER_LOCAL;
    provider.instance = fixture; provider.generate = generate; provider.health = legacy_probe; provider.destroy = destroy;
    CHECK(umi_ai_provider_registry_add(&fixture->runtime.providers, &provider) == UMI_STATUS_OK);
    UmiAiRuntimeDescriptor descriptor = catalogue_fixture("runtime.local");
    CHECK(umi_ai_authorengine_service_register_runtime(fixture->service, &descriptor) == UMI_STATUS_OK);
    CHECK(umi_ai_authorengine_service_register_runtime(fixture->other, &descriptor) == UMI_STATUS_OK);
    return 0;
}

static int exercise(Fixture *fixture)
{
    size_t count = SIZE_MAX;
    if (strcmp(fixture->action, "registry-count") == 0) {
        fixture->runtime.providers.count = SIZE_MAX;
        UmiStatus status = refresh(fixture, fixture->service, &count);
        fixture->runtime.providers.count = 1U; /* Restore the deliberately injected fault. */
        CHECK(status == UMI_STATUS_INVALID_STATE && count == SIZE_MAX && fixture->calls == 0U);
    } else if (strcmp(fixture->action, "health-from-generation") == 0) {
        UmiAiResponse response;
        CHECK(UmiAiRuntimeGenerateChecked(&fixture->runtime, "provider.local", 1,
            &fixture->request, &response, NULL) == UMI_STATUS_OK);
        CHECK(fixture->generated == 1U && fixture->calls == 0U && !fixture->failed);
    } else {
        UmiStatus expected = strcmp(fixture->outcome, "malformed") == 0 ? UMI_STATUS_INVALID_ARGUMENT :
            strcmp(fixture->entry, "scoped") == 0 && strcmp(fixture->outcome, "provider-error") == 0
            ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
        CHECK(refresh(fixture, fixture->service, &count) == expected);
        CHECK(!fixture->failed && fixture->calls == 1U && fixture->generated == 0U && fixture->destroyed == 0U);
        CHECK(count == (expected != UMI_STATUS_OK ? SIZE_MAX :
            strcmp(fixture->outcome, "provider-error") == 0 ? 0U : 1U));
    }
    CHECK(UmiAiAuthorEngineServiceCheckIdle(fixture->service) == UMI_STATUS_OK);
    CHECK(UmiAiRuntimeCheckIdle(&fixture->runtime) == UMI_STATUS_OK);
    /* A subsequent independent refresh proves that every prior return path
     * retired both activity guards without discarding the service. */
    fixture->action = "service-idle"; fixture->outcome = "success";
    fixture->calls = 0U; fixture->generated = 0U; count = SIZE_MAX;
    CHECK(refresh(fixture, fixture->service, &count) == UMI_STATUS_OK);
    CHECK(fixture->calls == 1U && !fixture->failed && count == 1U);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "nulls") == 0) {
        CHECK(UmiAiAuthorEngineServiceCheckIdle(NULL) == UMI_STATUS_OK);
        CHECK(UmiAiAuthorEngineServiceDestroyChecked(NULL) == UMI_STATUS_OK);
        umi_ai_authorengine_service_destroy(NULL);
        return 0;
    }
    if (argc != 4) return 2;
    Fixture *fixture = calloc(1U, sizeof *fixture);
    CHECK(fixture != NULL);
    fixture->entry = argv[1]; fixture->action = argv[2]; fixture->outcome = argv[3];
    int result = initialize(fixture);
    if (result == 0) result = exercise(fixture);
    UmiStatus release = UmiAiAuthorEngineServiceDestroyChecked(fixture->service);
    UmiStatus other_release = UmiAiAuthorEngineServiceDestroyChecked(fixture->other);
    UmiStatus runtime_release = UmiAiRuntimeRelease(&fixture->runtime);
    if (release != UMI_STATUS_OK || other_release != UMI_STATUS_OK ||
        runtime_release != UMI_STATUS_OK || fixture->destroyed != 1U) result = 1;
    free(fixture);
    return result;
}
