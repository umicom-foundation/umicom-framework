/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/context_broker.c
 *
 * PURPOSE:
 *   Implement deterministic context selection with mandatory-source,
 *   priority, token and privacy enforcement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
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

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiAiContextBroker *broker,
                         const char *source_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (broker == NULL || source_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < broker->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(broker->sources[index].source_id, source_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

/* Check that source satisfies its contract before another service relies on it. */
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

/*
 * Initialise ai context broker from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ai_context_broker_create(UmiAiContextBroker **out_broker)
{
    UmiAiContextBroker *broker;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_broker == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_broker = NULL;
    broker = (UmiAiContextBroker *)calloc(1U, sizeof(*broker));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (broker == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    broker->revision = 1U;
    *out_broker = broker;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by ai context broker so the same storage can be reused
 * safely.
 */
void umi_ai_context_broker_destroy(UmiAiContextBroker *broker)
{
    free(broker);
}

/*
 * Provide the ai context broker upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_context_broker_upsert(
    UmiAiContextBroker *broker,
    const UmiAiContextSource *source)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (broker == NULL || !source_valid(source)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(broker, source->source_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Remove ai context broker while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_ai_context_broker_remove(
    UmiAiContextBroker *broker,
    const char *source_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (broker == NULL || source_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(broker, source_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Release or reset state held by ai context broker so the same storage can be reused
 * safely.
 */
void umi_ai_context_broker_clear(UmiAiContextBroker *broker)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (broker == NULL) return;
    (void)memset(broker->sources, 0, sizeof(broker->sources));
    broker->count = 0U;
    ++broker->revision;
}

/*
 * Find ai context broker while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ai_context_broker_at(
    const UmiAiContextBroker *broker,
    size_t index,
    UmiAiContextSource *out_source)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (broker == NULL || out_source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= broker->count) return UMI_STATUS_NOT_FOUND;
    *out_source = broker->sources[index];
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by ai context broker without changing their
 * state.
 */
size_t umi_ai_context_broker_count(const UmiAiContextBroker *broker)
{
    return broker != NULL ? broker->count : 0U;
}

/* Provide the before operation used by this module and its client applications. */
static int before(const UmiAiContextSource *left,
                  const UmiAiContextSource *right)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->mandatory != right->mandatory) return left->mandatory > right->mandatory;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->priority != right->priority) return left->priority > right->priority;
    return strcmp(left->source_id, right->source_id) < 0;
}

/*
 * Provide the ai context broker plan operation used by this module and its client
 * applications.
 */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < broker->count; ++index) {
        size_t position;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!broker->sources[index].enabled) continue;
        position = order_count;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (position > 0U &&
               before(&broker->sources[index],
                      &broker->sources[order[position - 1U]])) {
            order[position] = order[position - 1U];
            --position;
        }
        order[position] = index;
        ++order_count;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < order_count; ++index) {
        const UmiAiContextSource *source = &broker->sources[order[index]];
        UmiStatus privacy_status = umi_ai_privacy_policy_check_share(
            privacy, provider_kind, source->classification,
            sensitive_approved);
        int fits = source->estimated_tokens <=
                   available - out_plan->selected_tokens;
        int has_slot = out_plan->source_count < maximum_sources;

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (privacy_status != UMI_STATUS_OK || !fits || !has_slot) {
            /* Apply this branch only when its contract condition is satisfied. */
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
        /* Apply this branch only when its contract condition is satisfied. */
        if (source->classification > out_plan->highest_classification) {
            out_plan->highest_classification = source->classification;
        }
    }
    out_plan->remaining_tokens = available - out_plan->selected_tokens;
    return UMI_STATUS_OK;
}

/*
 * Provide the ai context source kind text operation used by this module and its client
 * applications.
 */
const char *umi_ai_context_source_kind_text(UmiAiContextSourceKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
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
