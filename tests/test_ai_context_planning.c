/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_context_planning.c
 *
 * PURPOSE:
 *   Verify complete privacy-aware context selection, deterministic flag ordering and unchanged plans after mandatory or domain refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "ai/context_broker_fixture.h"

int main(int argc, char **argv)
{
    static const char *const modes[] = {"ordering",
        "mandatory-negative",
        "enabled-negative",
        "mandatory-budget",
        "mandatory-slots",
        "mandatory-private",
        "mandatory-approval",
        "optional-budget",
        "optional-private",
        "disabled",
        "exact-budget",
        "maximum-tokens",
        "provider-low",
        "provider-high",
        "invalid-policy",
        "empty-provider",
        "empty-policy",
        "null-output",
        "zero-context",
        "reserved-equal",
        "zero-sources",
        "too-many-sources",
        "source-copy"};
    UmiAiContextBroker *broker = NULL;
    UmiAiContextPlan *plan = malloc(sizeof *plan), *prior_plan = malloc(sizeof *prior_plan);
    UmiAiPrivacyPolicy privacy = umi_ai_privacy_policy_default();
    UmiAiProviderKind provider = UMI_AI_PROVIDER_LOCAL;
    UmiAiContextSource a = context_fixture("context.a"), b = context_fixture("context.b"),
        c = context_fixture("context.c");
    uint32_t limit = 200U, reserved = 20U;
    size_t maximum_sources = 8U;
    int known = 0, approved = 1;
    UmiStatus expected = UMI_STATUS_OK;
    assert(argc == 2 && plan != NULL && prior_plan != NULL);
    for (size_t i = 0U; i < sizeof modes / sizeof modes[0]; ++i)
        if (strcmp(argv[1], modes[i]) == 0) known = 1;
    assert(known);
#define CASE(name) (strcmp(argv[1], name) == 0)
    assert(umi_ai_context_broker_create(&broker) == UMI_STATUS_OK);
    a.estimated_tokens = 60U; a.priority = 10U; a.classification = UMI_AI_DATA_PUBLIC;
    b.estimated_tokens = 50U; b.priority = 20U; b.classification = UMI_AI_DATA_PUBLIC;
    c.estimated_tokens = 30U; c.priority = 20U; c.classification = UMI_AI_DATA_PUBLIC;
    if (CASE("mandatory-negative")) a.mandatory = -7;
    if (CASE("enabled-negative")) b.enabled = -1;
    if (CASE("mandatory-budget") || CASE("mandatory-slots") ||
        CASE("mandatory-private") || CASE("mandatory-approval")) {
        a.mandatory = 1; b.mandatory = 1;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (CASE("mandatory-budget")) { limit = 80U; reserved = 0U; }
    if (CASE("mandatory-slots")) maximum_sources = 1U;
    if (CASE("mandatory-private") || CASE("mandatory-approval")) {
        a.classification = UMI_AI_DATA_CONFIDENTIAL;
        provider = CASE("mandatory-private") ? UMI_AI_PROVIDER_REMOTE : UMI_AI_PROVIDER_LOCAL;
        approved = CASE("mandatory-private") ? 1 : 0;
        expected = UMI_STATUS_PERMISSION_DENIED;
    }
    if (CASE("optional-budget")) { limit = 90U; reserved = 0U; }
    if (CASE("optional-private")) { b.classification = UMI_AI_DATA_CONFIDENTIAL; provider = UMI_AI_PROVIDER_REMOTE; }
    if (CASE("disabled")) a.enabled = b.enabled = c.enabled = 0;
    if (CASE("exact-budget")) { limit = 140U; reserved = 0U; }
    if (CASE("maximum-tokens")) {
        a.estimated_tokens = UINT32_MAX; b.enabled = c.enabled = 0;
        limit = UINT32_MAX; reserved = 0U;
    }
    if (CASE("provider-low") || CASE("empty-provider")) provider = (UmiAiProviderKind)0;
    if (CASE("provider-high")) provider = (UmiAiProviderKind)99;
    if (CASE("invalid-policy") || CASE("empty-policy"))
        privacy.maximum_local_classification = (UmiAiDataClassification)99;
    if (CASE("zero-context")) limit = 0U;
    if (CASE("reserved-equal")) reserved = limit;
    if (CASE("zero-sources")) maximum_sources = 0U;
    if (CASE("too-many-sources")) maximum_sources = UMI_AI_CONTEXT_PLAN_SOURCE_MAX + 1U;
    if (CASE("provider-low") || CASE("provider-high") || CASE("empty-provider") ||
        CASE("invalid-policy") || CASE("empty-policy") || CASE("zero-context") ||
        CASE("reserved-equal") || CASE("zero-sources") || CASE("too-many-sources") || CASE("null-output"))
        expected = UMI_STATUS_INVALID_ARGUMENT;
    /* Register out of ranking order so stable ID/priority rules are exercised. */
    assert(umi_ai_context_broker_upsert(broker, &c) == UMI_STATUS_OK);
    assert(umi_ai_context_broker_upsert(broker, &a) == UMI_STATUS_OK);
    assert(umi_ai_context_broker_upsert(broker, &b) == UMI_STATUS_OK);
    if (CASE("empty-provider") || CASE("empty-policy"))
        assert(UmiAiContextBrokerClearChecked(broker) == UMI_STATUS_OK);
    UmiSnapshotCapture prior_capture, after_capture;
    assert(UmiAiContextBrokerCapture(broker, &prior_capture) == UMI_STATUS_OK);
    memset(plan, 0xA5, sizeof *plan); memcpy(prior_plan, plan, sizeof *plan);
    UmiStatus status = umi_ai_context_broker_plan(broker, limit, reserved, maximum_sources,
        provider, &privacy, approved, CASE("null-output") ? NULL : plan);
    assert(status == expected);
    assert(UmiAiContextBrokerCapture(broker, &after_capture) == UMI_STATUS_OK);
    assert(prior_capture.count == after_capture.count && prior_capture.revision == after_capture.revision);
    if (status != UMI_STATUS_OK) {
        assert(memcmp(prior_plan, plan, sizeof *plan) == 0);
    } else {
        assert(plan->revision == prior_capture.revision);
        assert(plan->context_limit == limit && plan->reserved_output_tokens == reserved);
        assert(plan->selected_tokens <= limit - reserved);
        assert(plan->remaining_tokens == limit - reserved - plan->selected_tokens);
        if (CASE("mandatory-negative")) {
            assert(plan->source_count == 3U);
            assert(strcmp(plan->sources[0].source_id, "context.a") == 0);
        } else if (CASE("optional-budget")) {
            assert(plan->source_count == 2U && plan->dropped_count == 1U && plan->truncated);
            assert(plan->selected_tokens == 80U && plan->remaining_tokens == 10U);
        } else if (CASE("optional-private")) {
            assert(plan->source_count == 2U && plan->dropped_count == 1U && plan->truncated);
            assert(plan->selected_tokens == 90U);
            assert(strcmp(plan->sources[0].source_id, "context.c") == 0);
        } else if (CASE("disabled")) {
            assert(plan->source_count == 0U && plan->dropped_count == 0U && !plan->truncated);
        } else if (CASE("maximum-tokens")) {
            assert(plan->source_count == 1U && plan->selected_tokens == UINT32_MAX && plan->remaining_tokens == 0U);
        } else {
            assert(plan->source_count == 3U && plan->dropped_count == 0U && !plan->truncated);
            assert(strcmp(plan->sources[0].source_id, "context.b") == 0);
            assert(strcmp(plan->sources[1].source_id, "context.c") == 0);
            assert(strcmp(plan->sources[2].source_id, "context.a") == 0);
            if (CASE("exact-budget")) assert(plan->remaining_tokens == 0U);
        }
        if (CASE("source-copy")) {
            strcpy(b.label, "Later source label");
            assert(umi_ai_context_broker_upsert(broker, &b) == UMI_STATUS_OK);
            assert(strcmp(plan->sources[0].label, "Example document") == 0);
            assert(UmiAiContextBrokerCapture(broker, &after_capture) == UMI_STATUS_OK);
            assert(after_capture.revision != plan->revision);
        }
    }
    umi_ai_context_broker_destroy(broker); free(plan); free(prior_plan);
    return 0;
#undef CASE
}
