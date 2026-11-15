/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai/test_checked_generation.c
 *
 * PURPOSE:
 *   Verify bounded provider values, refusal preservation and checked response publication through public APIs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/runtime.h"
#include "umicom/ai_coding_runtime/provider_executor.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); return 1; \
} } while (0)

typedef struct Fixture {
    UmiAiRuntime runtime;
    UmiAiRequest request;
    UmiAiResponse response;
    UmiAiResponse before;
    UmiAiCodingRuntimeConfig config;
    unsigned calls;
    const char *behavior;
} Fixture;

static UmiStatus generate(void *instance, const UmiAiRequest *request,
    UmiAiResponse *response)
{
    Fixture *fixture = instance;
    if (UmiAiRequestValidate(request) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_STATE;
    ++fixture->calls;
    umi_ai_response_init(response);
    memcpy(response->text, "new reply", sizeof("new reply"));
    response->finish_reason = UMI_AI_FINISH_STOP;
    response->usage.input_tokens = 7U;
    response->usage.output_tokens = 3U;
    response->usage.total_tokens = 10U;
    if (strcmp(fixture->behavior, "provider-error") == 0) {
        memset(response->text, 'x', sizeof(response->text));
        return UMI_STATUS_IO_ERROR;
    }
    if (strcmp(fixture->behavior, "invalid-response") == 0)
        memset(response->text, 'x', sizeof(response->text));
    if (strcmp(fixture->behavior, "invalid-finish") == 0)
        response->finish_reason = (UmiAiFinishReason)99;
    if (strcmp(fixture->behavior, "selection-change") == 0) {
        strcpy(fixture->config.provider_id, "later.provider");
        strcpy(fixture->config.model_id, "later.model");
    }
    return UMI_STATUS_OK;
}

static Fixture *fixture_create(const char *behavior)
{
    Fixture *fixture = calloc(1U, sizeof(*fixture));
    if (fixture == NULL) return NULL;
    umi_ai_runtime_init(&fixture->runtime);
    umi_ai_request_init(&fixture->request);
    fixture->request.message_count = 1U;
    fixture->request.messages[0].role = UMI_AI_ROLE_USER;
    strcpy(fixture->request.messages[0].text, "Explain value ownership.");
    strcpy(fixture->request.model_id, "model");
    umi_ai_response_init(&fixture->response);
    strcpy(fixture->response.text, "previous response");
    fixture->response.finish_reason = UMI_AI_FINISH_LENGTH;
    fixture->response.usage.total_tokens = 42U;
    fixture->before = fixture->response;
    umi_ai_coding_runtime_config_init(&fixture->config);
    strcpy(fixture->config.provider_id, "provider");
    strcpy(fixture->config.model_id, "model");
    fixture->behavior = behavior;
    UmiAiProvider provider = {0};
    provider.structure_size = (uint32_t)sizeof(provider);
    provider.abi_version = 1U;
    provider.provider_id = "provider";
    provider.kind = UMI_AI_PROVIDER_LOCAL;
    provider.instance = fixture;
    provider.generate = generate;
    if (umi_ai_provider_registry_add(&fixture->runtime.providers, &provider) != UMI_STATUS_OK) {
        free(fixture);
        return NULL;
    }
    return fixture;
}

static char *request_field(UmiAiRequest *request, const char *name, size_t *capacity)
{
    *capacity = UMI_AI_ID_CAPACITY;
    if (strcmp(name, "request-id") == 0) return request->request_id;
    if (strcmp(name, "model-id") == 0) return request->model_id;
    if (strcmp(name, "message-name") == 0) return request->messages[0].name;
    if (strcmp(name, "message-text") == 0) {
        *capacity = sizeof(request->messages[0].text);
        return request->messages[0].text;
    }
    return NULL;
}

static int request_case(Fixture *fixture, const char *mode, const char *name)
{
    UmiAiRequest *request = &fixture->request;
    UmiStatus expected = UMI_STATUS_OK;
    size_t capacity;
    char *field = request_field(request, name, &capacity);
    if (field != NULL) {
        memset(field, 'x', capacity);
        if (strcmp(mode, "request-boundary") == 0) field[capacity - 1U] = '\0';
        else expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name, "count") == 0) {
        request->message_count = UMI_AI_MAX_MESSAGES + 1U;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(name, "count-max") == 0) {
        request->message_count = SIZE_MAX;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(name, "nan") == 0 || strcmp(name, "infinity") == 0) {
        request->temperature = strcmp(name, "nan") == 0 ? NAN : INFINITY;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name, "role-low") == 0 || strcmp(name, "role-high") == 0) {
        request->messages[0].role = strcmp(name, "role-low") == 0 ? (UmiAiRole)0 : (UmiAiRole)99;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name, "unused") == 0) {
        memset(&request->messages[1], 0xa5, sizeof(request->messages[1]));
    } else if (strcmp(name, "draft") == 0) {
        umi_ai_request_init(request);
    } else if (strcmp(name, "full") == 0 || strcmp(name, "last-slot") == 0) {
        request->message_count = UMI_AI_MAX_MESSAGES;
        for (size_t i = 0U; i < UMI_AI_MAX_MESSAGES; ++i)
            request->messages[i].role = (UmiAiRole)(1U + i % 4U);
        if (strcmp(name, "last-slot") == 0) {
            memset(request->messages[UMI_AI_MAX_MESSAGES - 1U].text, 'x',
                sizeof(request->messages[UMI_AI_MAX_MESSAGES - 1U].text));
            expected = UMI_STATUS_INVALID_ARGUMENT;
        }
    } else if (strcmp(name, "finite-provider-range") == 0) {
        request->temperature = 9.0; /* Structural validity is not provider capability. */
        request->allow_tools = -1;
        request->max_output_tokens = 0U;
    } else return 1;
    UmiAiRequest *before = malloc(sizeof(*before));
    CHECK(before != NULL);
    memcpy(before, request, sizeof(*before));
    UmiStatus actual = UmiAiRequestValidate(request);
    int unchanged = memcmp(before, request, sizeof(*before)) == 0;
    free(before);
    CHECK(actual == expected && unchanged);
    return 0;
}

static int response_case(Fixture *fixture, const char *mode, const char *name)
{
    UmiAiResponse *response = &fixture->response;
    char *field = NULL;
    size_t capacity = UMI_AI_ID_CAPACITY;
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(name, "request-id") == 0) field = response->request_id;
    else if (strcmp(name, "provider-id") == 0) field = response->provider_id;
    else if (strcmp(name, "model-id") == 0) field = response->model_id;
    else if (strcmp(name, "text") == 0) { field = response->text; capacity = sizeof(response->text); }
    else if (strcmp(name, "finish-low") == 0 || strcmp(name, "finish-high") == 0) {
        response->finish_reason = strcmp(name, "finish-low") == 0 ? (UmiAiFinishReason)-1 : (UmiAiFinishReason)99;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name, "known-finishes") == 0) {
        for (int finish = (int)UMI_AI_FINISH_NONE; finish <= (int)UMI_AI_FINISH_ERROR; ++finish) {
            response->finish_reason = (UmiAiFinishReason)finish;
            CHECK(UmiAiResponseValidate(response) == UMI_STATUS_OK);
        }
    } else if (strcmp(name, "usage-evidence") == 0) {
        response->usage.input_tokens = UINT64_MAX;
        response->usage.output_tokens = UINT64_MAX;
        response->usage.total_tokens = 0U; /* No invented total or identity. */
        strcpy(response->model_id, "provider.alias");
    } else return 1;
    if (field != NULL) {
        memset(field, 'x', capacity);
        if (strcmp(mode, "response-boundary") == 0) field[capacity - 1U] = '\0';
        else expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiAiResponse before;
    memcpy(&before, response, sizeof(before));
    CHECK(UmiAiResponseValidate(response) == expected);
    CHECK(memcmp(&before, response, sizeof(before)) == 0);
    return 0;
}

static int dispatch_case(Fixture *fixture, const char *entry, const char *name)
{
    UmiAiGenerationReport report;
    UmiAiCodingProviderExecution execution = {0}, execution_before;
    strcpy(execution.provider_id, "previous");
    execution.revision = 31U;
    execution_before = execution;
    memset(&report, 0x5a, sizeof(report));
    const char *provider_id = "provider";
    UmiStatus expected = UMI_STATUS_OK;
    unsigned expected_calls = 1U;
    int checked = strcmp(entry, "checked") == 0;
    int coding = strcmp(entry, "coding") == 0;
    int legacy = strcmp(entry, "legacy") == 0;
    CHECK(checked || coding || legacy);
    if (strcmp(name, "bad-request") == 0) {
        fixture->request.message_count = SIZE_MAX;
        expected = UMI_STATUS_CAPACITY_EXCEEDED; expected_calls = 0U;
    } else if (strcmp(name, "missing-provider") == 0) {
        provider_id = "missing";
        strcpy(fixture->config.provider_id, provider_id);
        expected = UMI_STATUS_NOT_FOUND; expected_calls = 0U;
    } else if (strcmp(name, "policy") == 0) {
        fixture->runtime.policy.allow_local = 0;
        expected = UMI_STATUS_PERMISSION_DENIED; expected_calls = 0U;
    } else if (strcmp(name, "registry") == 0) {
        fixture->runtime.providers.count = SIZE_MAX;
        expected = UMI_STATUS_INVALID_STATE; expected_calls = 0U;
    } else if (strcmp(name, "provider-error") == 0) {
        expected = UMI_STATUS_IO_ERROR;
    } else if (strcmp(name, "invalid-response") == 0 || strcmp(name, "invalid-finish") == 0) {
        expected = legacy ? UMI_STATUS_OK : UMI_STATUS_INTERNAL_ERROR;
    } else CHECK(strcmp(name, "success") == 0 || strcmp(name, "selection-change") == 0);

    UmiStatus actual;
    if (checked) actual = UmiAiRuntimeGenerateChecked(&fixture->runtime, provider_id, 1,
        &fixture->request, &fixture->response, &report);
    else if (coding) actual = umi_ai_coding_provider_execute(&fixture->runtime, &fixture->config,
        1, &fixture->request, &fixture->response, &execution);
    else actual = umi_ai_runtime_generate(&fixture->runtime, provider_id, 1,
        &fixture->request, &fixture->response);
    CHECK(actual == expected && fixture->calls == expected_calls);
    if (expected_calls == 0U || (!legacy && expected != UMI_STATUS_OK)) {
        CHECK(memcmp(&fixture->before, &fixture->response, sizeof(fixture->response)) == 0);
    } else if (strcmp(name, "provider-error") == 0 || strcmp(name, "invalid-response") == 0) {
        CHECK(memchr(fixture->response.text, '\0', sizeof(fixture->response.text)) == NULL);
    } else CHECK(strcmp(fixture->response.text, "new reply") == 0);
    if (checked) {
        CHECK(report.status == expected && report.dispatched == (int)expected_calls);
        CHECK(report.response_published == (expected == UMI_STATUS_OK));
        CHECK(report.response_checked == (expected_calls != 0U && expected != UMI_STATUS_IO_ERROR));
        CHECK(report.provider_status == (expected_calls == 0U ? UMI_STATUS_UNAVAILABLE :
            expected == UMI_STATUS_IO_ERROR ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK));
        CHECK(report.response_status == (!report.response_checked ? UMI_STATUS_UNAVAILABLE :
            expected == UMI_STATUS_INTERNAL_ERROR ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_OK));
    }
    if (coding) {
        if (strcmp(name, "bad-request") == 0) {
            CHECK(memcmp(&execution_before, &execution, sizeof(execution)) == 0);
        } else {
            CHECK(execution.status == expected && execution.revision == 1U);
            CHECK(strcmp(execution.provider_id, provider_id) == 0);
            CHECK(strcmp(execution.model_id, "model") == 0);
            CHECK(execution.finish_reason == (expected == UMI_STATUS_OK ? UMI_AI_FINISH_STOP : UMI_AI_FINISH_NONE));
            CHECK(execution.usage.total_tokens == (expected == UMI_STATUS_OK ? 10U : 0U));
        }
    }
    return 0;
}

static int config_case(Fixture *fixture, const char *name)
{
    if (strcmp(name, "provider-id") == 0)
        memset(fixture->config.provider_id, 'x', sizeof(fixture->config.provider_id));
    else if (strcmp(name, "model-id") == 0)
        memset(fixture->config.model_id, 'x', sizeof(fixture->config.model_id));
    else if (strcmp(name, "temperature") == 0) fixture->config.temperature = NAN;
    else if (strcmp(name, "kind") == 0) fixture->config.provider_kind = (UmiAiProviderKind)99;
    else if (strcmp(name, "budget") == 0) fixture->config.context_token_budget = 0U;
    else return 1;
    UmiAiCodingProviderExecution execution = {0}, before;
    execution.revision = 12U;
    memcpy(&before, &execution, sizeof(before));
    CHECK(umi_ai_coding_provider_execute(&fixture->runtime, &fixture->config, 1,
        &fixture->request, &fixture->response, &execution) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(fixture->calls == 0U && memcmp(&execution, &before, sizeof(before)) == 0);
    CHECK(memcmp(&fixture->before, &fixture->response, sizeof(fixture->response)) == 0);
    return 0;
}

static int optional_case(Fixture *fixture, const char *name)
{
    UmiAiGenerationReport report;
    if (strcmp(name, "null-report") == 0) {
        CHECK(UmiAiRuntimeGenerateChecked(&fixture->runtime, "provider", 1,
            &fixture->request, &fixture->response, NULL) == UMI_STATUS_OK);
        CHECK(fixture->calls == 1U && strcmp(fixture->response.text, "new reply") == 0);
        return 0;
    }
    if (strcmp(name, "value-nulls") == 0) {
        CHECK(UmiAiRequestValidate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiResponseValidate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    UmiAiRuntime *runtime = &fixture->runtime;
    const char *id = "provider";
    const UmiAiRequest *request = &fixture->request;
    UmiAiResponse *response = &fixture->response;
    if (strcmp(name, "runtime") == 0) runtime = NULL;
    else if (strcmp(name, "id") == 0) id = NULL;
    else if (strcmp(name, "request") == 0) request = NULL;
    else if (strcmp(name, "response") == 0) response = NULL;
    else return 1;
    memset(&report, 0x5a, sizeof(report));
    CHECK(UmiAiRuntimeGenerateChecked(runtime, id, 1, request, response, &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(report.status == UMI_STATUS_INVALID_ARGUMENT && !report.dispatched &&
        !report.response_checked && !report.response_published);
    CHECK(report.provider_status == UMI_STATUS_UNAVAILABLE && report.response_status == UMI_STATUS_UNAVAILABLE);
    CHECK(fixture->calls == 0U && memcmp(&fixture->before, &fixture->response, sizeof(fixture->response)) == 0);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 3) return 2;
    const char *name = argv[argc - 1];
    Fixture *fixture = fixture_create(name);
    if (fixture == NULL) return 3;
    int result = 2;
    if (strcmp(argv[1], "request") == 0 || strcmp(argv[1], "request-boundary") == 0)
        result = request_case(fixture, argv[1], name);
    else if (strcmp(argv[1], "response") == 0 || strcmp(argv[1], "response-boundary") == 0)
        result = response_case(fixture, argv[1], name);
    else if (strcmp(argv[1], "dispatch") == 0 && argc == 4)
        result = dispatch_case(fixture, argv[2], name);
    else if (strcmp(argv[1], "config") == 0)
        result = config_case(fixture, name);
    else if (strcmp(argv[1], "optional") == 0)
        result = optional_case(fixture, name);
    /* No owned provider destructor is installed; corrupted-count fixtures have
     * no external handles. Release valid owners through their public API. */
    if (fixture->runtime.providers.count <= UMI_AI_MAX_PROVIDERS)
        (void)UmiAiRuntimeRelease(&fixture->runtime);
    free(fixture);
    return result;
}
