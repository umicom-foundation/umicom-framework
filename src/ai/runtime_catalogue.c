/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/runtime_catalogue.c
 *
 * PURPOSE:
 *   Implement provider/runtime discovery, health updates and deterministic
 *   capability-based selection for AuthorEngine Integration v2.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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

static size_t find_index(const UmiAiRuntimeCatalogue *catalogue,
                         const char *runtime_id)
{
    size_t index;
    if (catalogue == NULL || runtime_id == NULL) return SIZE_MAX;
    for (index = 0U; index < catalogue->count; ++index) {
        if (strcmp(catalogue->items[index].runtime_id, runtime_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

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

UmiStatus umi_ai_runtime_catalogue_create(
    UmiAiRuntimeCatalogue **out_catalogue)
{
    UmiAiRuntimeCatalogue *catalogue;
    if (out_catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_catalogue = NULL;
    catalogue = (UmiAiRuntimeCatalogue *)calloc(1U, sizeof(*catalogue));
    if (catalogue == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    catalogue->revision = 1U;
    *out_catalogue = catalogue;
    return UMI_STATUS_OK;
}

void umi_ai_runtime_catalogue_destroy(UmiAiRuntimeCatalogue *catalogue)
{
    free(catalogue);
}

UmiStatus umi_ai_runtime_catalogue_upsert(
    UmiAiRuntimeCatalogue *catalogue,
    const UmiAiRuntimeDescriptor *descriptor)
{
    size_t index;
    if (catalogue == NULL || !descriptor_valid(descriptor)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(catalogue, descriptor->runtime_id);
    if (index == SIZE_MAX) {
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

UmiStatus umi_ai_runtime_catalogue_find(
    const UmiAiRuntimeCatalogue *catalogue,
    const char *runtime_id,
    UmiAiRuntimeDescriptor *out_descriptor)
{
    size_t index;
    if (catalogue == NULL || runtime_id == NULL || out_descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(catalogue, runtime_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_descriptor = catalogue->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_runtime_catalogue_at(
    const UmiAiRuntimeCatalogue *catalogue,
    size_t index,
    UmiAiRuntimeDescriptor *out_descriptor)
{
    if (catalogue == NULL || out_descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= catalogue->count) return UMI_STATUS_NOT_FOUND;
    *out_descriptor = catalogue->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_runtime_catalogue_set_health(
    UmiAiRuntimeCatalogue *catalogue,
    const char *runtime_id,
    const UmiAiHealth *health)
{
    size_t index;
    if (catalogue == NULL || runtime_id == NULL || health == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(catalogue, runtime_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    catalogue->items[index].health = *health;
    ++catalogue->revision;
    catalogue->items[index].revision = catalogue->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_runtime_catalogue_select(
    const UmiAiRuntimeCatalogue *catalogue,
    uint32_t required_capabilities,
    uint32_t required_context_tokens,
    int local_only,
    UmiAiRuntimeDescriptor *out_descriptor)
{
    size_t index;
    size_t best = SIZE_MAX;
    if (catalogue == NULL || out_descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < catalogue->count; ++index) {
        const UmiAiRuntimeDescriptor *candidate = &catalogue->items[index];
        if (!candidate->configured || !candidate->health.available ||
            (local_only && !candidate->local) ||
            (candidate->capabilities & required_capabilities) !=
                required_capabilities ||
            candidate->context_tokens < required_context_tokens) {
            continue;
        }
        if (best == SIZE_MAX ||
            candidate->context_tokens < catalogue->items[best].context_tokens ||
            (candidate->context_tokens == catalogue->items[best].context_tokens &&
             strcmp(candidate->runtime_id,
                    catalogue->items[best].runtime_id) < 0)) {
            best = index;
        }
    }
    if (best == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_descriptor = catalogue->items[best];
    return UMI_STATUS_OK;
}

size_t umi_ai_runtime_catalogue_count(
    const UmiAiRuntimeCatalogue *catalogue)
{
    return catalogue != NULL ? catalogue->count : 0U;
}

size_t umi_ai_runtime_catalogue_provider_count(
    const UmiAiRuntimeCatalogue *catalogue)
{
    size_t index;
    size_t count = 0U;
    if (catalogue == NULL) return 0U;
    for (index = 0U; index < catalogue->count; ++index) {
        size_t prior;
        int seen = 0;
        for (prior = 0U; prior < index; ++prior) {
            if (strcmp(catalogue->items[index].provider_id,
                       catalogue->items[prior].provider_id) == 0) {
                seen = 1;
                break;
            }
        }
        if (!seen) ++count;
    }
    return count;
}

const char *umi_ai_runtime_transport_text(UmiAiRuntimeTransport transport)
{
    switch (transport) {
        case UMI_AI_RUNTIME_IN_PROCESS: return "in-process";
        case UMI_AI_RUNTIME_PROCESS: return "process";
        case UMI_AI_RUNTIME_HTTP: return "http";
        case UMI_AI_RUNTIME_LIBRARY: return "library";
        default: return "unknown";
    }
}
