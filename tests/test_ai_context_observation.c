/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_context_observation.c
 *
 * PURPOSE:
 *   Verify context previews remain bound to their policy, broker and selected runtime owners without activating a model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/ai/authorengine_service.h"
#include "ai/runtime_catalogue_fixture.h"
/* Include the real implementation only to reach read-only revision boundary
 * fixtures. All metadata edits and ordinary observations use public APIs. */
#include "../src/ai/authorengine_service.c"

#define REQUIRE(condition) do { if (!(condition)) { \
    fprintf(stderr, "Context observation check failed at line %d\n", __LINE__); \
    result = 1; goto cleanup; } } while (0)
#define CASE(name) (strcmp(argv[1], name) == 0)

int main(int argc, char **argv)
{
    static const char *const modes[] = {"valid",
        "borrowed-id",
        "policy-stale",
        "context-upsert",
        "context-remove",
        "context-clear",
        "runtime-replace",
        "runtime-health",
        "runtime-missing",
        "unrelated-runtime",
        "failed-policy-update",
        "failed-context-update",
        "failed-runtime-update",
        "readonly-exhausted",
        "invalid-owner-zero",
        "refusal-budget",
        "refusal-privacy",
        "refusal-provider",
        "missing-runtime",
        "null-service",
        "null-id",
        "null-plan",
        "null-observation",
        "check-null-service",
        "check-null-observation",
        "check-empty-id",
        "check-id-bound",
        "check-zero-revisions",
        "refresh"};
    UmiAiRuntime runtime;
    UmiAiAuthorEngineConfig config = {0};
    UmiAiAuthorEngineService *service = NULL;
    UmiAiRuntimeDescriptor descriptor = catalogue_fixture("runtime.preview");
    UmiAiContextSource source = {0};
    UmiAiContextPlan *plan = malloc(sizeof *plan), *prior_plan = malloc(sizeof *prior_plan);
    UmiAiContextPlanObservation observation, prior_observation;
    UmiAiAuthorEngineServiceSnapshot before_snapshot, after_snapshot;
    UmiStatus status, expected = UMI_STATUS_OK;
    int result = 0, known = 0, initial_refusal;
    umi_ai_runtime_init(&runtime);
    REQUIRE(argc == 2 && plan != NULL && prior_plan != NULL);
    for (size_t i = 0U; i < sizeof modes / sizeof modes[0]; ++i)
        if (CASE(modes[i])) known = 1;
    REQUIRE(known);
    strcpy(config.executable, "uaengine"); strcpy(config.workspace, ".");
    strcpy(config.provider, "provider.local");
    status = umi_ai_authorengine_service_create(&runtime, &config, &service);
    REQUIRE(status == UMI_STATUS_OK);
    if (CASE("refusal-provider")) descriptor.provider_kind = UMI_AI_PROVIDER_REMOTE;
    status = umi_ai_authorengine_service_register_runtime(service, &descriptor);
    REQUIRE(status == UMI_STATUS_OK);
    strcpy(source.source_id, "source.preview"); strcpy(source.label, "Context preview");
    strcpy(source.uri, "workspace://preview"); source.kind = UMI_AI_CONTEXT_DOCUMENT;
    source.classification = UMI_AI_DATA_PUBLIC; source.estimated_tokens = 80U;
    source.mandatory = 1; source.enabled = 1;
    if (CASE("refusal-budget")) source.estimated_tokens = 8000U;
    if (CASE("refusal-privacy")) source.classification = UMI_AI_DATA_RESTRICTED;
    status = umi_ai_context_broker_upsert(service->context, &source);
    REQUIRE(status == UMI_STATUS_OK);
    if (CASE("readonly-exhausted")) service->revision = UINT64_MAX;
    if (CASE("invalid-owner-zero")) service->revision = 0U;
    memset(plan, 0xA5, sizeof *plan); memcpy(prior_plan, plan, sizeof *plan);
    memset(&observation, 0x5A, sizeof observation);
    memcpy(&prior_observation, &observation, sizeof observation);
    initial_refusal = CASE("refusal-budget") || CASE("refusal-privacy") || CASE("refusal-provider") ||
        CASE("missing-runtime") || CASE("null-service") || CASE("null-id") || CASE("null-plan") ||
        CASE("null-observation") || CASE("invalid-owner-zero");
    status = UmiAiAuthorEnginePlanContextObserved(CASE("null-service") ? NULL : service,
        CASE("null-id") ? NULL : CASE("missing-runtime") ? "runtime.missing" : descriptor.runtime_id,
        1, CASE("null-plan") ? NULL : plan, CASE("null-observation") ? NULL : &observation);
    if (initial_refusal) {
        expected = CASE("refusal-budget") ? UMI_STATUS_CAPACITY_EXCEEDED :
            CASE("refusal-privacy") || CASE("refusal-provider") ? UMI_STATUS_PERMISSION_DENIED :
            CASE("missing-runtime") ? UMI_STATUS_NOT_FOUND :
            CASE("invalid-owner-zero") ? UMI_STATUS_INVALID_STATE : UMI_STATUS_INVALID_ARGUMENT;
        REQUIRE(status == expected);
        REQUIRE(memcmp(prior_plan, plan, sizeof *plan) == 0);
        REQUIRE(memcmp(&prior_observation, &observation, sizeof observation) == 0);
        goto cleanup;
    }
    REQUIRE(status == UMI_STATUS_OK && plan->source_count == 1U && plan->selected_tokens == 80U);
    REQUIRE(plan->revision == observation.context_revision);
    REQUIRE(strcmp(observation.runtime_id, descriptor.runtime_id) == 0);
    status = UmiAiAuthorEngineCheckContextObservation(service, &observation);
    REQUIRE(status == UMI_STATUS_OK);
    if (CASE("borrowed-id")) {
        status = UmiAiAuthorEnginePlanContextObserved(service, observation.runtime_id, 1, plan, &observation);
        REQUIRE(status == UMI_STATUS_OK);
    }
    if (CASE("policy-stale") || CASE("refresh")) {
        UmiAiPrivacyPolicy privacy = umi_ai_privacy_policy_default();
        status = umi_ai_authorengine_service_set_policy(service, &runtime.policy, &privacy, 4096U, 512U);
        REQUIRE(status == UMI_STATUS_OK); expected = UMI_STATUS_BUSY;
    }
    if (CASE("context-upsert")) {
        strcpy(source.label, "Changed after preview");
        status = umi_ai_context_broker_upsert(service->context, &source);
        REQUIRE(status == UMI_STATUS_OK); expected = UMI_STATUS_BUSY;
    }
    if (CASE("context-remove")) {
        status = umi_ai_context_broker_remove(service->context, source.source_id);
        REQUIRE(status == UMI_STATUS_OK); expected = UMI_STATUS_BUSY;
    }
    if (CASE("context-clear")) {
        status = UmiAiContextBrokerClearChecked(service->context);
        REQUIRE(status == UMI_STATUS_OK); expected = UMI_STATUS_BUSY;
    }
    if (CASE("runtime-replace") || CASE("unrelated-runtime")) {
        if (CASE("unrelated-runtime")) strcpy(descriptor.runtime_id, "runtime.unrelated");
        strcpy(descriptor.model_id, "model.changed");
        status = umi_ai_runtime_catalogue_upsert(service->catalogue, &descriptor);
        REQUIRE(status == UMI_STATUS_OK);
        expected = CASE("unrelated-runtime") ? UMI_STATUS_OK : UMI_STATUS_BUSY;
    }
    if (CASE("runtime-health")) {
        UmiAiHealth health = {0}; strcpy(health.provider_id, descriptor.provider_id);
        health.available = 1;
        status = umi_ai_runtime_catalogue_set_health(service->catalogue, descriptor.runtime_id, &health);
        REQUIRE(status == UMI_STATUS_OK); expected = UMI_STATUS_BUSY;
    }
    if (CASE("runtime-missing")) {
/* Model disappearance with a fresh valid empty catalogue; no catalogue clear API exists. Allocate before releasing the fixture owner. The former implementation is retained for owner review. */
#if 0
        umi_ai_runtime_catalogue_clear(service->catalogue); expected = UMI_STATUS_BUSY;
#endif
        UmiAiRuntimeCatalogue *empty = NULL;
        status = umi_ai_runtime_catalogue_create(&empty);
        REQUIRE(status == UMI_STATUS_OK);
        umi_ai_runtime_catalogue_destroy(service->catalogue);
        service->catalogue = empty;
        expected = UMI_STATUS_BUSY;
    }
    if (CASE("failed-policy-update")) {
        UmiAiPrivacyPolicy privacy = umi_ai_privacy_policy_default();
        privacy.maximum_remote_classification = (UmiAiDataClassification)99;
        status = umi_ai_authorengine_service_set_policy(service, &runtime.policy, &privacy, 4096U, 512U);
        REQUIRE(status == UMI_STATUS_INVALID_ARGUMENT);
    }
    if (CASE("failed-context-update")) {
        source.estimated_tokens = 0U;
        status = umi_ai_context_broker_upsert(service->context, &source);
        REQUIRE(status == UMI_STATUS_INVALID_ARGUMENT);
    }
    if (CASE("failed-runtime-update")) {
        descriptor.context_tokens = 0U;
        status = umi_ai_runtime_catalogue_upsert(service->catalogue, &descriptor);
        REQUIRE(status == UMI_STATUS_INVALID_ARGUMENT);
    }
    if (CASE("check-empty-id")) observation.runtime_id[0] = '\0';
    if (CASE("check-id-bound")) memset(observation.runtime_id, 'x', sizeof observation.runtime_id);
    if (CASE("check-null-service") || CASE("check-null-observation") ||
        CASE("check-empty-id") || CASE("check-id-bound")) expected = UMI_STATUS_INVALID_ARGUMENT;
    if (CASE("check-zero-revisions")) {
        UmiAiContextPlanObservation invalid = observation;
        invalid.service_revision = 0U;
        REQUIRE(UmiAiAuthorEngineCheckContextObservation(service, &invalid) == UMI_STATUS_INVALID_ARGUMENT);
        invalid = observation; invalid.context_revision = 0U;
        REQUIRE(UmiAiAuthorEngineCheckContextObservation(service, &invalid) == UMI_STATUS_INVALID_ARGUMENT);
        invalid = observation; invalid.runtime_revision = 0U;
        REQUIRE(UmiAiAuthorEngineCheckContextObservation(service, &invalid) == UMI_STATUS_INVALID_ARGUMENT);
    }
    memcpy(prior_plan, plan, sizeof *plan);
    memcpy(&prior_observation, &observation, sizeof observation);
    status = umi_ai_authorengine_service_snapshot(service, &before_snapshot);
    REQUIRE(status == UMI_STATUS_OK);
    status = UmiAiAuthorEngineCheckContextObservation(CASE("check-null-service") ? NULL : service,
        CASE("check-null-observation") ? NULL : &observation);
    REQUIRE(status == expected);
    status = umi_ai_authorengine_service_snapshot(service, &after_snapshot);
    REQUIRE(status == UMI_STATUS_OK && memcmp(&before_snapshot, &after_snapshot, sizeof before_snapshot) == 0);
    REQUIRE(memcmp(prior_plan, plan, sizeof *plan) == 0);
    REQUIRE(memcmp(&prior_observation, &observation, sizeof observation) == 0);
    if (CASE("refresh")) {
        status = UmiAiAuthorEnginePlanContextObserved(service, descriptor.runtime_id, 1, plan, &observation);
        REQUIRE(status == UMI_STATUS_OK && plan->context_limit == 4096U);
        REQUIRE(observation.service_revision != prior_observation.service_revision);
        REQUIRE(UmiAiAuthorEngineCheckContextObservation(service, &observation) == UMI_STATUS_OK);
    }
cleanup:
    umi_ai_authorengine_service_destroy(service); umi_ai_runtime_destroy(&runtime);
    free(plan); free(prior_plan);
    return result;
}
#undef CASE
#undef REQUIRE
