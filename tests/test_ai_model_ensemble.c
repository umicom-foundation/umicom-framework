/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_model_ensemble.c
 *
 * PURPOSE:
 *   Verify multi-model requests preserve policy failures, successful answers,
 *   explicit selection and caller-size safety.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The test providers are deterministic and never use a network. They prove the
 * orchestration contract without requiring credentials or external services.
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/umicom.h"

static size_t generate_calls;

/*
 * Exercise generate answer and return a clear result when the behaviour no longer matches
 * its contract.
 */
static UmiStatus generate_answer(void *instance,
                                 const UmiAiRequest *request,
                                 UmiAiResponse *response)
{
    const char *provider_id = (const char *)instance;
    /* Count invocations to prove validation completes before provider calls. */
    ++generate_calls;
    umi_ai_response_init(response);
    (void)snprintf(response->provider_id, sizeof(response->provider_id),
                   "%s", provider_id);
    (void)snprintf(response->model_id, sizeof(response->model_id),
                   "%s", request->model_id);
    return umi_ai_response_set_text(response, provider_id);
}

/*
 * Exercise add provider and return a clear result when the behaviour no longer matches its
 * contract.
 */
static void add_provider(UmiAiRuntime *runtime,
                         const char *provider_id,
                         UmiAiProviderKind kind)
{
    UmiAiProvider provider = {0};
    provider.structure_size = (uint32_t)sizeof(provider);
    provider.abi_version = 1U;
    provider.provider_id = provider_id;
    provider.kind = kind;
    provider.instance = (void *)provider_id;
    provider.generate = generate_answer;
    assert(umi_ai_provider_registry_add(&runtime->providers, &provider) ==
           UMI_STATUS_OK);
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiAiRuntime runtime;
    UmiAiRequest request;
    UmiAiMessage message;
    UmiAiModelTarget targets[2];
    UmiAiModelEnsembleReport report;
    unsigned char undersized[sizeof(UmiAiModelEnsembleReport)] = {0x5aU};

    umi_ai_runtime_init(&runtime);
    runtime.policy.allow_remote = 1;
    runtime.policy.require_remote_approval = 1;
    add_provider(&runtime, "test.local", UMI_AI_PROVIDER_LOCAL);
    add_provider(&runtime, "test.remote", UMI_AI_PROVIDER_REMOTE);

    umi_ai_request_init(&request);
    (void)snprintf(request.request_id, sizeof(request.request_id),
                   "%s", "ensemble.request.1");
    assert(umi_ai_message_set(&message, UMI_AI_ROLE_USER, "user",
                              "Explain this change") == UMI_STATUS_OK);
    assert(umi_ai_request_add_message(&request, &message) == UMI_STATUS_OK);
    assert(umi_ai_model_target_initialize(
               &targets[0], "test.local", "local-model", "Local", 0) ==
           UMI_STATUS_OK);
    assert(umi_ai_model_target_initialize(
               &targets[1], "test.remote", "remote-model", "Remote", 0) ==
           UMI_STATUS_OK);

    /* A malformed later target must not let an earlier provider run first. */
    (void)memset(targets[1].model_id, 'x', sizeof(targets[1].model_id));
    generate_calls = 0U;
    assert(umi_ai_model_ensemble_query(
               &runtime, &request, targets, 2U, &report) ==
           UMI_STATUS_INVALID_ARGUMENT);
    assert(generate_calls == 0U);
    assert(umi_ai_model_target_initialize(
               &targets[1], "test.remote", "remote-model", "Remote", 0) ==
           UMI_STATUS_OK);

    /* The local answer remains usable when remote approval is absent. */
    assert(umi_ai_model_ensemble_query(
               &runtime, &request, targets, 2U, &report) == UMI_STATUS_OK);
    assert(report.result_count == 2U);
    assert(report.success_count == 1U);
    assert(report.results[1].status == UMI_STATUS_PERMISSION_DENIED);
    assert(umi_ai_model_ensemble_selected(&report) == &report.results[0]);

    /* Approval enables the remote route and either answer can be selected. */
    targets[1].approved = 1;
    assert(umi_ai_model_ensemble_query(
               &runtime, &request, targets, 2U, &report) == UMI_STATUS_OK);
    assert(report.success_count == 2U);
    assert(umi_ai_model_ensemble_select(&report, 1U) == UMI_STATUS_OK);
    assert(strcmp(umi_ai_model_ensemble_selected(&report)->response.text,
                  "test.remote") == 0);

    /* An old caller size is rejected without writing into caller memory. */
    assert(umi_ai_model_ensemble_query_sized(
               &runtime, &request, targets, 2U,
               (UmiAiModelEnsembleReport *)undersized, 8U) ==
           UMI_STATUS_INVALID_ARGUMENT);
    assert(undersized[0] == 0x5aU);
    return 0;
}
