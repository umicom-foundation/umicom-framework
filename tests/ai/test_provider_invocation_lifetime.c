/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai/test_provider_invocation_lifetime.c
 *
 * PURPOSE:
 *   Verify serialized provider ownership across generation callbacks, failures and nested runtimes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/runtime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); return 1; } } while (0)

typedef struct Fixture Fixture;
typedef struct Context { Fixture *fixture; unsigned index; } Context;
struct Fixture {
    UmiAiRuntime runtimes[2];
    UmiAiRequest request;
    Context contexts[2];
    const char *action;
    const char *outcome;
    unsigned calls[2], destroyed[2];
    int failed;
};

static void destroy(void *instance)
{
    Context *context = instance;
    ++context->fixture->destroyed[context->index];
}

static UmiStatus generate(void *instance, const UmiAiRequest *request, UmiAiResponse *response)
{
    Context *context = instance;
    Fixture *fixture = context->fixture;
    const unsigned index = context->index;
    UmiAiRuntime *runtime = &fixture->runtimes[index];
    if (++fixture->calls[index] > 1U) {
        fixture->failed = 1; /* Bound recursion even when the guard regresses. */
        return UMI_STATUS_INVALID_STATE;
    }
    UmiStatus status = UMI_STATUS_BUSY;
    if (index == 1U) {
        if (UmiAiRuntimeRelease(&fixture->runtimes[0]) != UMI_STATUS_BUSY ||
            UmiAiRuntimeCheckIdle(&fixture->runtimes[0]) != UMI_STATUS_BUSY ||
            UmiAiRuntimeCheckIdle(runtime) != UMI_STATUS_BUSY)
            fixture->failed = 1;
    } else {
        UmiAiRuntime before;
        memcpy(&before, runtime, sizeof(before));
        if (UmiAiProviderRegistryValidate(&runtime->providers) != UMI_STATUS_OK ||
            umi_ai_provider_registry_find(&runtime->providers, "provider") == NULL)
            fixture->failed = 1;
        if (strcmp(fixture->action, "registry-idle") == 0)
            status = UmiAiProviderRegistryCheckIdle(&runtime->providers);
        else if (strcmp(fixture->action, "runtime-idle") == 0)
            status = UmiAiRuntimeCheckIdle(runtime);
        else if (strcmp(fixture->action, "registry-release") == 0)
            status = UmiAiProviderRegistryRelease(&runtime->providers);
        else if (strcmp(fixture->action, "runtime-release") == 0)
            status = UmiAiRuntimeRelease(runtime);
        else if (strcmp(fixture->action, "registry-destroy") == 0)
            umi_ai_provider_registry_destroy(&runtime->providers);
        else if (strcmp(fixture->action, "runtime-destroy") == 0)
            umi_ai_runtime_destroy(runtime);
        else if (strcmp(fixture->action, "registry-init") == 0)
            umi_ai_provider_registry_init(&runtime->providers);
        else if (strcmp(fixture->action, "runtime-init") == 0)
            umi_ai_runtime_init(runtime);
        else if (strcmp(fixture->action, "registry-add") == 0) {
            UmiAiProvider other = runtime->providers.providers[0];
            other.provider_id = "other";
            other.instance = NULL; other.destroy = NULL;
            status = umi_ai_provider_registry_add(&runtime->providers, &other);
        } else if (strcmp(fixture->action, "legacy-recursion") == 0 ||
                   strcmp(fixture->action, "checked-recursion") == 0) {
            UmiAiResponse nested, saved;
            umi_ai_response_init(&nested);
            strcpy(nested.text, "preserved");
            memcpy(&saved, &nested, sizeof(saved));
            if (strcmp(fixture->action, "legacy-recursion") == 0)
                status = umi_ai_runtime_generate(runtime, "provider", 1, request, &nested);
            else {
                UmiAiGenerationReport report;
                status = UmiAiRuntimeGenerateChecked(runtime, "provider", 1, request, &nested, &report);
                if (report.dispatched || report.response_checked || report.response_published ||
                    report.status != UMI_STATUS_BUSY || report.provider_status != UMI_STATUS_UNAVAILABLE)
                    fixture->failed = 1;
            }
            if (memcmp(&saved, &nested, sizeof(saved)) != 0) fixture->failed = 1;
        } else if (strcmp(fixture->action, "nested-other") == 0) {
            UmiAiResponse nested;
            umi_ai_response_init(&nested);
            if (UmiAiRuntimeGenerateChecked(&fixture->runtimes[1], "provider", 1,
                    request, &nested, NULL) != UMI_STATUS_OK ||
                strcmp(nested.text, "inner") != 0 ||
                UmiAiRuntimeCheckIdle(&fixture->runtimes[1]) != UMI_STATUS_OK ||
                UmiAiRuntimeCheckIdle(runtime) != UMI_STATUS_BUSY ||
                UmiAiRuntimeRelease(&fixture->runtimes[1]) != UMI_STATUS_OK ||
                fixture->destroyed[1] != 1U)
                fixture->failed = 1;
        } else fixture->failed = 1;
        if (status != UMI_STATUS_BUSY || fixture->destroyed[0] != 0U ||
            memcmp(&before, runtime, sizeof(before)) != 0 ||
            UmiAiRuntimeCheckIdle(runtime) != UMI_STATUS_BUSY)
            fixture->failed = 1;
    }
    umi_ai_response_init(response);
    strcpy(response->text, index == 0U ? "outer" : "inner");
    response->finish_reason = UMI_AI_FINISH_STOP;
    if (index == 0U && strcmp(fixture->outcome, "invalid-response") == 0)
        memset(response->text, 'x', sizeof(response->text));
    return index == 0U && strcmp(fixture->outcome, "provider-error") == 0
        ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}

static int run_case(Fixture *fixture, const char *entry)
{
    for (unsigned i = 0U; i < 2U; ++i) {
        umi_ai_runtime_init(&fixture->runtimes[i]);
        fixture->contexts[i].fixture = fixture; fixture->contexts[i].index = i;
        UmiAiProvider provider = {0};
        provider.structure_size = (uint32_t)sizeof(provider);
        provider.abi_version = 1U; provider.provider_id = "provider";
        provider.kind = UMI_AI_PROVIDER_LOCAL;
        provider.instance = &fixture->contexts[i];
        provider.generate = generate; provider.destroy = destroy;
        CHECK(umi_ai_provider_registry_add(&fixture->runtimes[i].providers, &provider) == UMI_STATUS_OK);
    }
    umi_ai_request_init(&fixture->request);
    UmiAiResponse response;
    umi_ai_response_init(&response);
    strcpy(response.text, "previous");
    UmiStatus expected = strcmp(fixture->outcome, "provider-error") == 0 ? UMI_STATUS_IO_ERROR :
        strcmp(entry, "checked") == 0 && strcmp(fixture->outcome, "invalid-response") == 0 ?
        UMI_STATUS_INTERNAL_ERROR : UMI_STATUS_OK;
    UmiStatus status;
    if (strcmp(entry, "checked") == 0) {
        UmiAiGenerationReport report;
        status = UmiAiRuntimeGenerateChecked(&fixture->runtimes[0], "provider", 1,
            &fixture->request, &response, &report);
        CHECK(report.dispatched && report.response_published == (expected == UMI_STATUS_OK));
        if (expected != UMI_STATUS_OK) CHECK(strcmp(response.text, "previous") == 0);
    } else {
        CHECK(strcmp(entry, "legacy") == 0);
        status = umi_ai_runtime_generate(&fixture->runtimes[0], "provider", 1, &fixture->request, &response);
    }
    CHECK(status == expected && !fixture->failed && fixture->calls[0] == 1U);
    CHECK(fixture->calls[1] == (strcmp(fixture->action, "nested-other") == 0 ? 1U : 0U));
    CHECK(UmiAiRuntimeCheckIdle(&fixture->runtimes[0]) == UMI_STATUS_OK);
    CHECK(UmiAiProviderRegistryCheckIdle(&fixture->runtimes[0].providers) == UMI_STATUS_OK);
    CHECK(UmiAiRuntimeRelease(&fixture->runtimes[0]) == UMI_STATUS_OK && fixture->destroyed[0] == 1U);
    CHECK(UmiAiRuntimeRelease(&fixture->runtimes[1]) == UMI_STATUS_OK && fixture->destroyed[1] == 1U);
    CHECK(UmiAiRuntimeRelease(&fixture->runtimes[0]) == UMI_STATUS_OK && fixture->destroyed[0] == 1U);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "nulls") == 0) {
        CHECK(UmiAiProviderRegistryCheckIdle(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiRuntimeCheckIdle(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    if (argc != 4) return 2;
    Fixture *fixture = calloc(1U, sizeof(*fixture));
    CHECK(fixture != NULL);
    fixture->action = argv[2]; fixture->outcome = argv[3];
    int result = run_case(fixture, argv[1]);
    /* Fixtures deliberately keep instances in this allocation so a broken
     * early-release implementation produces a failure, not a dangling pointer. */
    free(fixture);
    return result;
}
