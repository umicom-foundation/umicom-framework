/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/context_broker.c
 *
 * PURPOSE:
 *   Implement deterministic context selection with mandatory-source,
 *   priority, token and privacy enforcement.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Mandatory sources are considered first, then priority and stable source ID.
 * The same inputs therefore produce the same explainable context plan.
 */
#include "umicom/ai/context_broker.h"

#include <stdlib.h>
#include <string.h>

struct UmiAiContextBroker {
    UmiAiContextSource sources[UMI_AI_CONTEXT_SOURCE_CAPACITY];
    size_t count;
    uint64_t revision;
};

static size_t find_index(const UmiAiContextBroker *broker,
                         const char *source_id)
{
    size_t index;
    if (broker == NULL || source_id == NULL) return SIZE_MAX;
    for (index = 0U; index < broker->count; ++index) {
        if (strcmp(broker->sources[index].source_id, source_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

static int source_valid(const UmiAiContextSource *source)
{
    return source != NULL && source->source_id[0] != '\0' &&
           source->label[0] != '\0' && source->uri[0] != '\0' &&
           source->kind >= UMI_AI_CONTEXT_WORKSPACE &&
           source->kind <= UMI_AI_CONTEXT_RETRIEVAL &&
           source->classification >= UMI_AI_DATA_PUBLIC &&
           source->classification <= UMI_AI_DATA_RESTRICTED &&
           source->estimated_tokens > 0U;
}

UmiStatus umi_ai_context_broker_create(UmiAiContextBroker **out_broker)
{
    UmiAiContextBroker *broker;
    if (out_broker == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_broker = NULL;
    broker = (UmiAiContextBroker *)calloc(1U, sizeof(*broker));
    if (broker == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    broker->revision = 1U;
    *out_broker = broker;
    return UMI_STATUS_OK;
}

void umi_ai_context_broker_destroy(UmiAiContextBroker *broker)
{
    free(broker);
}

UmiStatus umi_ai_context_broker_upsert(
    UmiAiContextBroker *broker,
    const UmiAiContextSource *source)
{
    size_t index;
    if (broker == NULL || !source_valid(source)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(broker, source->source_id);
    if (index == SIZE_MAX) {
        if (broker->count >= UMI_AI_CONTEXT_SOURCE_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = broker->count++;
    }
    broker->sources[index] = *source;
    ++broker->revision;
    broker->sources[index].revision = broker->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_context_broker_remove(
    UmiAiContextBroker *broker,
    const char *source_id)
{
    size_t index;
    if (broker == NULL || source_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(broker, source_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (index + 1U < broker->count) {
        (void)memmove(&broker->sources[index],
                      &broker->sources[index + 1U],
                      (broker->count - index - 1U) * sizeof(broker->sources[0]));
    }
    --broker->count;
    (void)memset(&broker->sources[broker->count], 0,
                 sizeof(broker->sources[0]));
    ++broker->revision;
    return UMI_STATUS_OK;
}

void umi_ai_context_broker_clear(UmiAiContextBroker *broker)
{
    if (broker == NULL) return;
    (void)memset(broker->sources, 0, sizeof(broker->sources));
    broker->count = 0U;
    ++broker->revision;
}

UmiStatus umi_ai_context_broker_at(
    const UmiAiContextBroker *broker,
    size_t index,
    UmiAiContextSource *out_source)
{
    if (broker == NULL || out_source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= broker->count) return UMI_STATUS_NOT_FOUND;
    *out_source = broker->sources[index];
    return UMI_STATUS_OK;
}

size_t umi_ai_context_broker_count(const UmiAiContextBroker *broker)
{
    return broker != NULL ? broker->count : 0U;
}

static int before(const UmiAiContextSource *left,
                  const UmiAiContextSource *right)
{
    if (left->mandatory != right->mandatory) return left->mandatory > right->mandatory;
    if (left->priority != right->priority) return left->priority > right->priority;
    return strcmp(left->source_id, right->source_id) < 0;
}

UmiStatus umi_ai_context_broker_plan(
    const UmiAiContextBroker *broker,
    uint32_t context_limit,
    uint32_t reserved_output_tokens,
    size_t maximum_sources,
    UmiAiProviderKind provider_kind,
    const UmiAiPrivacyPolicy *privacy,
    int sensitive_approved,
    UmiAiContextPlan *out_plan)
{
    size_t order[UMI_AI_CONTEXT_SOURCE_CAPACITY];
    size_t order_count = 0U;
    size_t index;
    uint32_t available;

    if (broker == NULL || privacy == NULL || out_plan == NULL ||
        context_limit == 0U || reserved_output_tokens >= context_limit ||
        maximum_sources == 0U ||
        maximum_sources > UMI_AI_CONTEXT_PLAN_SOURCE_MAX) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_plan, 0, sizeof(*out_plan));
    out_plan->context_limit = context_limit;
    out_plan->reserved_output_tokens = reserved_output_tokens;
    out_plan->highest_classification = UMI_AI_DATA_PUBLIC;
    out_plan->revision = broker->revision;
    available = context_limit - reserved_output_tokens;

    for (index = 0U; index < broker->count; ++index) {
        size_t position;
        if (!broker->sources[index].enabled) continue;
        position = order_count;
        while (position > 0U &&
               before(&broker->sources[index],
                      &broker->sources[order[position - 1U]])) {
            order[position] = order[position - 1U];
            --position;
        }
        order[position] = index;
        ++order_count;
    }

    for (index = 0U; index < order_count; ++index) {
        const UmiAiContextSource *source = &broker->sources[order[index]];
        UmiStatus privacy_status = umi_ai_privacy_policy_check_share(
            privacy, provider_kind, source->classification,
            sensitive_approved);
        int fits = source->estimated_tokens <=
                   available - out_plan->selected_tokens;
        int has_slot = out_plan->source_count < maximum_sources;

        if (privacy_status != UMI_STATUS_OK || !fits || !has_slot) {
            if (source->mandatory) {
                return privacy_status != UMI_STATUS_OK
                    ? privacy_status : UMI_STATUS_CAPACITY_EXCEEDED;
            }
            ++out_plan->dropped_count;
            out_plan->truncated = 1;
            continue;
        }
        out_plan->sources[out_plan->source_count++] = *source;
        out_plan->selected_tokens += source->estimated_tokens;
        if (source->classification > out_plan->highest_classification) {
            out_plan->highest_classification = source->classification;
        }
    }
    out_plan->remaining_tokens = available - out_plan->selected_tokens;
    return UMI_STATUS_OK;
}

const char *umi_ai_context_source_kind_text(UmiAiContextSourceKind kind)
{
    switch (kind) {
        case UMI_AI_CONTEXT_WORKSPACE: return "workspace";
        case UMI_AI_CONTEXT_PROJECT: return "project";
        case UMI_AI_CONTEXT_DOCUMENT: return "document";
        case UMI_AI_CONTEXT_SELECTION: return "selection";
        case UMI_AI_CONTEXT_DIAGNOSTICS: return "diagnostics";
        case UMI_AI_CONTEXT_SOURCE_CONTROL: return "source-control";
        case UMI_AI_CONTEXT_RETRIEVAL: return "retrieval";
        default: return "unknown";
    }
}
