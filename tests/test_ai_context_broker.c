/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_ai_context_broker.c
 *
 * PURPOSE:
 *   Verify deterministic context priority, token budgets and privacy gates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/ai/context_broker.h"

static UmiAiContextSource source(const char *id,
                                 uint32_t tokens,
                                 uint32_t priority,
                                 UmiAiDataClassification classification)
{
    UmiAiContextSource value = {0};
    (void)strcpy(value.source_id, id);
    (void)strcpy(value.label, id);
    (void)strcpy(value.uri, "workspace://batch-47");
    value.kind = UMI_AI_CONTEXT_DOCUMENT;
    value.classification = classification;
    value.estimated_tokens = tokens;
    value.priority = priority;
    value.enabled = 1;
    return value;
}

int main(void)
{
    UmiAiContextBroker *broker = NULL;
    UmiAiPrivacyPolicy privacy = umi_ai_privacy_policy_default();
    UmiAiContextPlan plan;
    UmiAiContextSource important = source(
        "context.important", 2500U, 100U, UMI_AI_DATA_INTERNAL);
    UmiAiContextSource optional = source(
        "context.optional", 2000U, 20U, UMI_AI_DATA_PUBLIC);
    assert(umi_ai_context_broker_create(&broker) == UMI_STATUS_OK);
    assert(umi_ai_context_broker_upsert(broker, &optional) == UMI_STATUS_OK);
    assert(umi_ai_context_broker_upsert(broker, &important) == UMI_STATUS_OK);
    assert(umi_ai_context_broker_plan(
        broker, 4096U, 1024U, 8U, UMI_AI_PROVIDER_LOCAL, &privacy, 0,
        &plan) == UMI_STATUS_OK);
    assert(plan.source_count == 1U);
    assert(plan.dropped_count == 1U);
    assert(plan.truncated);
    assert(strcmp(plan.sources[0].source_id, "context.important") == 0);
    assert(umi_ai_context_broker_plan(
        broker, 8192U, 1024U, 8U, UMI_AI_PROVIDER_REMOTE, &privacy, 0,
        &plan) == UMI_STATUS_OK);
    assert(plan.source_count == 1U);
    assert(strcmp(plan.sources[0].source_id, "context.optional") == 0);
    umi_ai_context_broker_destroy(broker);
    return 0;
}
