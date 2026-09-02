/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/runtime_catalogue.c
 *
 * PURPOSE:
 *   Implement provider/runtime discovery, health updates and deterministic
 *   capability-based selection for AuthorEngine Integration v2.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This catalogue stores descriptions only.  Calls still pass through the
 * provider registry, so discovering a runtime never bypasses AI policy.
 */
#include "umicom/ai/runtime_catalogue.h"

#include <stdlib.h>
#include <string.h>

struct UmiAiRuntimeCatalogue {
    UmiAiRuntimeDescriptor items[UMI_AI_RUNTIME_CATALOGUE_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiAiRuntimeCatalogue *catalogue,
                         const char *runtime_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || runtime_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(catalogue->items[index].runtime_id, runtime_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

/* Check that descriptor satisfies its contract before another service relies on it. */
static int descriptor_valid(const UmiAiRuntimeDescriptor *descriptor)
{
    return descriptor != NULL &&
           descriptor->runtime_id[0] != '\0' &&
           descriptor->provider_id[0] != '\0' &&
           descriptor->display_name[0] != '\0' &&
           descriptor->model_id[0] != '\0' &&
           descriptor->provider_kind >= UMI_AI_PROVIDER_LOCAL &&
           descriptor->provider_kind <= UMI_AI_PROVIDER_TEST &&
           descriptor->transport >= UMI_AI_RUNTIME_IN_PROCESS &&
           descriptor->transport <= UMI_AI_RUNTIME_LIBRARY &&
           descriptor->context_tokens > 0U &&
           descriptor->max_output_tokens > 0U &&
           descriptor->max_output_tokens <= descriptor->context_tokens;
}

/*
 * Initialise ai runtime catalogue from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ai_runtime_catalogue_create(
    UmiAiRuntimeCatalogue **out_catalogue)
{
    UmiAiRuntimeCatalogue *catalogue;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_catalogue = NULL;
    catalogue = (UmiAiRuntimeCatalogue *)calloc(1U, sizeof(*catalogue));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    catalogue->revision = 1U;
    *out_catalogue = catalogue;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by ai runtime catalogue so the same storage can be reused
 * safely.
 */
void umi_ai_runtime_catalogue_destroy(UmiAiRuntimeCatalogue *catalogue)
{
    free(catalogue);
}

/*
 * Provide the ai runtime catalogue upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_runtime_catalogue_upsert(
    UmiAiRuntimeCatalogue *catalogue,
    const UmiAiRuntimeDescriptor *descriptor)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || !descriptor_valid(descriptor)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(catalogue, descriptor->runtime_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (catalogue->count >= UMI_AI_RUNTIME_CATALOGUE_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = catalogue->count++;
    }
    catalogue->items[index] = *descriptor;
    ++catalogue->revision;
    catalogue->items[index].revision = catalogue->revision;
    return UMI_STATUS_OK;
}

/*
 * Find ai runtime catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ai_runtime_catalogue_find(
    const UmiAiRuntimeCatalogue *catalogue,
    const char *runtime_id,
    UmiAiRuntimeDescriptor *out_descriptor)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || runtime_id == NULL || out_descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(catalogue, runtime_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_descriptor = catalogue->items[index];
    return UMI_STATUS_OK;
}

/*
 * Find ai runtime catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ai_runtime_catalogue_at(
    const UmiAiRuntimeCatalogue *catalogue,
    size_t index,
    UmiAiRuntimeDescriptor *out_descriptor)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || out_descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= catalogue->count) return UMI_STATUS_NOT_FOUND;
    *out_descriptor = catalogue->items[index];
    return UMI_STATUS_OK;
}

/*
 * Provide the ai runtime catalogue set health operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_runtime_catalogue_set_health(
    UmiAiRuntimeCatalogue *catalogue,
    const char *runtime_id,
    const UmiAiHealth *health)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || runtime_id == NULL || health == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(catalogue, runtime_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    catalogue->items[index].health = *health;
    ++catalogue->revision;
    catalogue->items[index].revision = catalogue->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the ai runtime catalogue select operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_runtime_catalogue_select(
    const UmiAiRuntimeCatalogue *catalogue,
    uint32_t required_capabilities,
    uint32_t required_context_tokens,
    int local_only,
    UmiAiRuntimeDescriptor *out_descriptor)
{
    size_t index;
    size_t best = SIZE_MAX;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || out_descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) {
        const UmiAiRuntimeDescriptor *candidate = &catalogue->items[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (!candidate->configured || !candidate->health.available ||
            (local_only && !candidate->local) ||
            (candidate->capabilities & required_capabilities) !=
                required_capabilities ||
            candidate->context_tokens < required_context_tokens) {
            continue;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (best == SIZE_MAX ||
            candidate->context_tokens < catalogue->items[best].context_tokens ||
            (candidate->context_tokens == catalogue->items[best].context_tokens &&
             strcmp(candidate->runtime_id,
                    catalogue->items[best].runtime_id) < 0)) {
            best = index;
        }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (best == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_descriptor = catalogue->items[best];
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by ai runtime catalogue without changing their
 * state.
 */
size_t umi_ai_runtime_catalogue_count(
    const UmiAiRuntimeCatalogue *catalogue)
{
    return catalogue != NULL ? catalogue->count : 0U;
}

/*
 * Return the number of records represented by ai runtime catalogue provider without
 * changing their state.
 */
size_t umi_ai_runtime_catalogue_provider_count(
    const UmiAiRuntimeCatalogue *catalogue)
{
    size_t index;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) {
        size_t prior;
        int seen = 0;
        /* Visit each bounded item once so every record receives the same rule. */
        for (prior = 0U; prior < index; ++prior) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (strcmp(catalogue->items[index].provider_id,
                       catalogue->items[prior].provider_id) == 0) {
                seen = 1;
                break;
            }
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!seen) ++count;
    }
    return count;
}

/*
 * Provide the ai runtime transport text operation used by this module and its client
 * applications.
 */
const char *umi_ai_runtime_transport_text(UmiAiRuntimeTransport transport)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (transport) {
        case UMI_AI_RUNTIME_IN_PROCESS: return "in-process";
        case UMI_AI_RUNTIME_PROCESS: return "process";
        case UMI_AI_RUNTIME_HTTP: return "http";
        case UMI_AI_RUNTIME_LIBRARY: return "library";
        default: return "unknown";
    }
}
