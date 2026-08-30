/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/code_action_provider.c
 *
 * PURPOSE:
 *   Implement validated, dynamically sized code-action provider discovery and
 *   language/capability matching for every Umicom application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/code_action_provider.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorCodeActionProviderRegistry {
    UmiEditorCodeActionProviderItem *providers;
    size_t count;
    size_t capacity;
    uint64_t revision;
};

static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

static UmiStatus validate_provider(
    const UmiEditorCodeActionProviderItem *provider)
{
    if (provider == NULL ||
        provider->struct_size != (uint32_t)sizeof(*provider) ||
        provider->api_version != UMI_EDITOR_CODE_ACTION_PROVIDER_API_VERSION ||
        !terminated(provider->id, sizeof(provider->id)) ||
        provider->id[0] == '\0' ||
        !terminated(provider->label, sizeof(provider->label)) ||
        provider->label[0] == '\0' ||
        !terminated(provider->language_id, sizeof(provider->language_id)) ||
        provider->language_id[0] == '\0' || provider->kind_mask == 0U ||
        (provider->kind_mask &
         ~(UmiEditorCodeActionKindMask)UMI_EDITOR_CODE_ACTION_KIND_ALL) != 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

static UmiStatus reserve_providers(UmiEditorCodeActionProviderRegistry *registry,
                                   size_t required)
{
    size_t capacity;
    UmiEditorCodeActionProviderItem *replacement;

    if (required <= registry->capacity) return UMI_STATUS_OK;
    capacity = registry->capacity > 0U ? registry->capacity : 8U;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorCodeActionProviderItem *)realloc(
        registry->providers, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->providers = replacement;
    registry->capacity = capacity;
    return UMI_STATUS_OK;
}

static size_t find_provider(
    const UmiEditorCodeActionProviderRegistry *registry,
    const char *provider_id)
{
    size_t index;

    if (registry == NULL || provider_id == NULL) return SIZE_MAX;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->providers[index].id, provider_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

UmiStatus umi_editor_code_action_provider_registry_create(
    UmiEditorCodeActionProviderRegistry **out_registry)
{
    UmiEditorCodeActionProviderRegistry *registry;

    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiEditorCodeActionProviderRegistry *)calloc(
        1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_editor_code_action_provider_registry_destroy(
    UmiEditorCodeActionProviderRegistry *registry)
{
    if (registry == NULL) return;
    free(registry->providers);
    registry->providers = NULL;
    free(registry);
}

UmiStatus umi_editor_code_action_provider_registry_clear(
    UmiEditorCodeActionProviderRegistry *registry)
{
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    registry->count = 0U;
    registry->revision = next_revision(registry->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_code_action_provider_registry_upsert(
    UmiEditorCodeActionProviderRegistry *registry,
    const UmiEditorCodeActionProviderItem *provider)
{
    UmiEditorCodeActionProviderItem stored;
    size_t index;
    UmiStatus status;

    if (registry == NULL || validate_provider(provider) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_provider(registry, provider->id);
    if (index == SIZE_MAX) {
        status = reserve_providers(registry, registry->count + 1U);
        if (status != UMI_STATUS_OK) return status;
        index = registry->count++;
    }
    stored = *provider;
    stored.supports_diagnostics = provider->supports_diagnostics != 0;
    stored.supports_selection = provider->supports_selection != 0;
    stored.supports_resolution = provider->supports_resolution != 0;
    stored.supports_workspace_edits =
        provider->supports_workspace_edits != 0;
    stored.supports_preview = provider->supports_preview != 0;
    stored.enabled = provider->enabled != 0;
    registry->providers[index] = stored;
    registry->revision = next_revision(registry->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_code_action_provider_registry_remove(
    UmiEditorCodeActionProviderRegistry *registry,
    const char *provider_id)
{
    size_t index;

    if (registry == NULL || provider_id == NULL || provider_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_provider(registry, provider_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (index + 1U < registry->count) {
        (void)memmove(&registry->providers[index],
                      &registry->providers[index + 1U],
                      (registry->count - index - 1U) *
                          sizeof(*registry->providers));
    }
    --registry->count;
    registry->revision = next_revision(registry->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_code_action_provider_registry_find(
    const UmiEditorCodeActionProviderRegistry *registry,
    const char *provider_id,
    UmiEditorCodeActionProviderItem *out_provider)
{
    size_t index;

    if (registry == NULL || provider_id == NULL || out_provider == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_provider(registry, provider_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_provider = registry->providers[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_code_action_provider_registry_at(
    const UmiEditorCodeActionProviderRegistry *registry,
    size_t index,
    UmiEditorCodeActionProviderItem *out_provider)
{
    if (registry == NULL || out_provider == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_provider = registry->providers[index];
    return UMI_STATUS_OK;
}

int umi_editor_code_action_provider_supports(
    const UmiEditorCodeActionProviderItem *provider,
    const char *language_id,
    UmiEditorCodeActionKindMask kind_mask)
{
    if (validate_provider(provider) != UMI_STATUS_OK ||
        language_id == NULL || language_id[0] == '\0' || kind_mask == 0U ||
        !provider->enabled) {
        return 0;
    }
    return (strcmp(provider->language_id, language_id) == 0 ||
            strcmp(provider->language_id, "*") == 0) &&
           (provider->kind_mask & kind_mask) != 0U;
}

UmiStatus umi_editor_code_action_provider_registry_snapshot(
    const UmiEditorCodeActionProviderRegistry *registry,
    UmiEditorCodeActionProviderSnapshot *out_snapshot)
{
    size_t index;
    size_t comparison;

    if (registry == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_CODE_ACTION_PROVIDER_API_VERSION;
    out_snapshot->provider_count = registry->count;
    out_snapshot->revision = registry->revision;
    for (index = 0U; index < registry->count; ++index) {
        int first_language = 1;
        if (registry->providers[index].enabled) {
            ++out_snapshot->enabled_provider_count;
            out_snapshot->provided_kind_mask |=
                registry->providers[index].kind_mask;
        }
        for (comparison = 0U; comparison < index; ++comparison) {
            if (strcmp(registry->providers[index].language_id,
                       registry->providers[comparison].language_id) == 0) {
                first_language = 0;
                break;
            }
        }
        if (first_language) ++out_snapshot->language_count;
    }
    return UMI_STATUS_OK;
}

size_t umi_editor_code_action_provider_registry_count(
    const UmiEditorCodeActionProviderRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

uint64_t umi_editor_code_action_provider_registry_revision(
    const UmiEditorCodeActionProviderRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}
