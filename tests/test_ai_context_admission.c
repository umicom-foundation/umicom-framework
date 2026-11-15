/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_context_admission.c
 *
 * PURPOSE:
 *   Verify context source bounds, registry invariants, nonwrapping revisions and checked clear behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "ai/context_broker_fixture.h"
/* Private representation access is limited to invalid-state/exhaustion fixtures;
 * mutations and reads exercise the real exported implementation. */
#include "../src/ai/context_broker.c"

int main(int argc, char **argv)
{
    static const char *const modes[] = {"valid",
        "source-id",
        "label",
        "uri",
        "empty-id",
        "empty-label",
        "empty-uri",
        "kind-low",
        "kind-high",
        "class-low",
        "class-high",
        "zero-tokens",
        "capacity",
        "replace-full",
        "revision-max-upsert",
        "revision-max-remove",
        "revision-max-clear",
        "revision-last",
        "empty-clear",
        "legacy-clear-max",
        "corrupt-count",
        "corrupt-revision",
        "corrupt-record-revision",
        "corrupt-record-ahead",
        "corrupt-duplicate",
        "corrupt-text",
        "capture-null",
        "remove-query",
        "missing-remove",
        "at-missing"};
    UmiAiContextBroker *broker = NULL, *prior_owner = malloc(sizeof *prior_owner);
    UmiAiContextSource item = context_fixture("context.new"), prior_item, output, prior_output;
    UmiSnapshotCapture capture = {9U, 77U}, prior_capture = capture;
    UmiStatus status = UMI_STATUS_OK, expected = UMI_STATUS_INVALID_ARGUMENT;
    int known = 0, success;
    assert(argc == 2 && prior_owner != NULL);
    for (size_t i = 0U; i < sizeof modes / sizeof modes[0]; ++i)
        if (strcmp(argv[1], modes[i]) == 0) known = 1;
    assert(known);
#define CASE(name) (strcmp(argv[1], name) == 0)
    assert(umi_ai_context_broker_create(&broker) == UMI_STATUS_OK);
    UmiAiContextSource initial = context_fixture("context.base");
    assert(umi_ai_context_broker_upsert(broker, &initial) == UMI_STATUS_OK);
    if (CASE("source-id")) memset(item.source_id, 'x', sizeof item.source_id);
    if (CASE("label")) memset(item.label, 'x', sizeof item.label);
    if (CASE("uri")) memset(item.uri, 'x', sizeof item.uri);
    if (CASE("empty-id")) item.source_id[0] = '\0';
    if (CASE("empty-label")) item.label[0] = '\0';
    if (CASE("empty-uri")) item.uri[0] = '\0';
    if (CASE("kind-low")) item.kind = (UmiAiContextSourceKind)0;
    if (CASE("kind-high")) item.kind = (UmiAiContextSourceKind)99;
    if (CASE("class-low")) item.classification = (UmiAiDataClassification)0;
    if (CASE("class-high")) item.classification = (UmiAiDataClassification)99;
    if (CASE("zero-tokens")) item.estimated_tokens = 0U;
    if (CASE("capacity") || CASE("replace-full")) {
        for (size_t i = 1U; i < UMI_AI_CONTEXT_SOURCE_CAPACITY; ++i) {
            UmiAiContextSource extra = context_fixture("context.fill");
            (void)snprintf(extra.source_id, sizeof extra.source_id, "context.fill.%zu", i);
            assert(umi_ai_context_broker_upsert(broker, &extra) == UMI_STATUS_OK);
        }
        if (CASE("replace-full")) strcpy(item.source_id, "context.base");
    }
    if (strncmp(argv[1], "revision-max-", 13U) == 0 || CASE("legacy-clear-max"))
        broker->revision = UINT64_MAX;
    if (CASE("revision-last")) broker->revision = UINT64_MAX - 1U;
    if (CASE("empty-clear")) assert(UmiAiContextBrokerClearChecked(broker) == UMI_STATUS_OK);
    if (CASE("corrupt-count")) broker->count = UMI_AI_CONTEXT_SOURCE_CAPACITY + 1U;
    if (CASE("corrupt-revision")) broker->revision = 0U;
    if (CASE("corrupt-record-revision")) broker->sources[0].revision = 0U;
    if (CASE("corrupt-record-ahead")) broker->sources[0].revision = broker->revision + 1U;
    if (CASE("corrupt-text")) memset(broker->sources[0].uri, 'x', sizeof broker->sources[0].uri);
    if (CASE("corrupt-duplicate")) {
        UmiAiContextSource extra = context_fixture("context.second");
        assert(umi_ai_context_broker_upsert(broker, &extra) == UMI_STATUS_OK);
        strcpy(broker->sources[1].source_id, broker->sources[0].source_id);
    }
    memcpy(prior_owner, broker, sizeof *broker);
    memcpy(&prior_item, &item, sizeof item);
    memset(&output, 0xA5, sizeof output); memcpy(&prior_output, &output, sizeof output);
    success = CASE("valid") || CASE("replace-full") || CASE("revision-last") || CASE("empty-clear");
    if (CASE("revision-max-remove"))
        status = umi_ai_context_broker_remove(broker, "context.base");
    else if (CASE("revision-max-clear") || CASE("empty-clear"))
        status = UmiAiContextBrokerClearChecked(broker);
    else if (CASE("legacy-clear-max")) {
        umi_ai_context_broker_clear(broker); status = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (CASE("capture-null"))
        status = UmiAiContextBrokerCapture(broker, NULL);
    else if (CASE("remove-query")) {
        char query[UMI_AI_ID_CAPACITY]; memset(query, 'x', sizeof query);
        status = umi_ai_context_broker_remove(broker, query);
    } else if (CASE("missing-remove"))
        status = umi_ai_context_broker_remove(broker, "context.missing");
    else if (CASE("at-missing"))
        status = umi_ai_context_broker_at(broker, UMI_AI_CONTEXT_SOURCE_CAPACITY, &output);
    else
        status = umi_ai_context_broker_upsert(broker, &item);
    assert(memcmp(&prior_item, &item, sizeof item) == 0);
    if (success) {
        assert(status == UMI_STATUS_OK);
        assert(UmiAiContextBrokerCapture(broker, &capture) == UMI_STATUS_OK);
        assert(capture.revision == prior_owner->revision + 1U);
        assert(capture.count == (CASE("empty-clear") ? 0U :
            prior_owner->count + (CASE("replace-full") ? 0U : 1U)));
        if (!CASE("empty-clear")) {
            size_t slot = CASE("replace-full") ? 0U : broker->count - 1U;
            assert(umi_ai_context_broker_at(broker, slot, &output) == UMI_STATUS_OK);
            assert(output.revision == capture.revision && strcmp(output.source_id, item.source_id) == 0);
        }
        if (CASE("revision-last")) {
            assert(capture.revision == UINT64_MAX);
            memcpy(prior_owner, broker, sizeof *broker);
            assert(UmiAiContextBrokerClearChecked(broker) == UMI_STATUS_CAPACITY_EXCEEDED);
            assert(memcmp(prior_owner, broker, sizeof *broker) == 0);
        }
    } else {
        if (CASE("capacity") || strncmp(argv[1], "revision-max-", 13U) == 0 || CASE("legacy-clear-max"))
            expected = UMI_STATUS_CAPACITY_EXCEEDED;
        if (strncmp(argv[1], "corrupt-", 8U) == 0) expected = UMI_STATUS_INVALID_STATE;
        if (CASE("missing-remove") || CASE("at-missing")) expected = UMI_STATUS_NOT_FOUND;
        assert(status == expected);
        assert(memcmp(prior_owner, broker, sizeof *broker) == 0);
        assert(memcmp(&prior_output, &output, sizeof output) == 0);
        if (expected == UMI_STATUS_INVALID_STATE) {
            assert(UmiAiContextBrokerCapture(broker, &capture) == UMI_STATUS_INVALID_STATE);
            assert(memcmp(&capture, &prior_capture, sizeof capture) == 0);
            assert(umi_ai_context_broker_at(broker, 0U, &output) == UMI_STATUS_INVALID_STATE);
            assert(memcmp(&prior_output, &output, sizeof output) == 0);
            assert(umi_ai_context_broker_count(broker) == 0U);
            assert(UmiAiContextBrokerClearChecked(broker) == UMI_STATUS_INVALID_STATE);
            assert(memcmp(prior_owner, broker, sizeof *broker) == 0);
        }
    }
    umi_ai_context_broker_destroy(broker); free(prior_owner);
    return 0;
#undef CASE
}
