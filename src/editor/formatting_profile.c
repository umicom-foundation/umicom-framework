/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/formatting_profile.c
 *
 * PURPOSE:
 *   Implement validated formatting profiles and deterministic language-policy
 *   resolution without coupling applications to a particular formatter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/formatting_profile.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorFormattingProfileRegistry {
    UmiEditorFormattingProfileItem *profiles;
    size_t count;
    size_t capacity;
    uint64_t revision;
};

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

/* Provide the terminated operation used by this module and its client applications. */
static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

/* Provide the validate profile operation used by this module and its client applications. */
static UmiStatus validate_profile(const UmiEditorFormattingProfileItem *profile)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL ||
        profile->struct_size != (uint32_t)sizeof(*profile) ||
        profile->api_version != UMI_EDITOR_FORMATTING_PROFILE_API_VERSION ||
        !terminated(profile->id, sizeof(profile->id)) ||
        profile->id[0] == '\0' ||
        !terminated(profile->language_id, sizeof(profile->language_id)) ||
        profile->language_id[0] == '\0' ||
        !terminated(profile->provider_id, sizeof(profile->provider_id)) ||
        profile->provider_id[0] == '\0' ||
        !terminated(profile->label, sizeof(profile->label)) ||
        profile->label[0] == '\0' || profile->tab_size == 0U ||
        profile->tab_size > 16U || profile->max_line_length > 4096U ||
        profile->brace_style < UMI_EDITOR_FORMATTING_BRACES_PRESERVE ||
        profile->brace_style > UMI_EDITOR_FORMATTING_BRACES_GNU ||
        profile->line_ending < UMI_EDITOR_FORMATTING_LINE_ENDING_PRESERVE ||
        profile->line_ending > UMI_EDITOR_FORMATTING_LINE_ENDING_CRLF) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* Provide the reserve profiles operation used by this module and its client applications. */
static UmiStatus reserve_profiles(UmiEditorFormattingProfileRegistry *registry,
                                  size_t required)
{
    size_t capacity;
    UmiEditorFormattingProfileItem *replacement;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= registry->capacity) return UMI_STATUS_OK;
    capacity = registry->capacity > 0U ? registry->capacity : 8U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (capacity < required) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorFormattingProfileItem *)realloc(
        registry->profiles, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->profiles = replacement;
    registry->capacity = capacity;
    return UMI_STATUS_OK;
}

/* Provide the find profile operation used by this module and its client applications. */
static size_t find_profile(const UmiEditorFormattingProfileRegistry *registry,
                           const char *profile_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || profile_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->profiles[index].id, profile_id) == 0) return index;
    }
    return SIZE_MAX;
}

/*
 * Initialise editor formatting profile registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_formatting_profile_registry_create(
    UmiEditorFormattingProfileRegistry **out_registry)
{
    UmiEditorFormattingProfileRegistry *registry;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiEditorFormattingProfileRegistry *)calloc(
        1U, sizeof(*registry));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor formatting profile registry so the same storage
 * can be reused safely.
 */
void umi_editor_formatting_profile_registry_destroy(
    UmiEditorFormattingProfileRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return;
    free(registry->profiles);
    registry->profiles = NULL;
    free(registry);
}

/*
 * Release or reset state held by editor formatting profile registry so the same storage
 * can be reused safely.
 */
UmiStatus umi_editor_formatting_profile_registry_clear(
    UmiEditorFormattingProfileRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    registry->count = 0U;
    registry->revision = next_revision(registry->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor formatting profile registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_formatting_profile_registry_upsert(
    UmiEditorFormattingProfileRegistry *registry,
    const UmiEditorFormattingProfileItem *profile)
{
    UmiEditorFormattingProfileItem stored;
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || validate_profile(profile) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_profile(registry, profile->id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        status = reserve_profiles(registry, registry->count + 1U);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        index = registry->count++;
    }
    stored = *profile;
    stored.insert_spaces = profile->insert_spaces != 0;
    stored.trim_trailing_whitespace =
        profile->trim_trailing_whitespace != 0;
    stored.insert_final_newline = profile->insert_final_newline != 0;
    stored.format_on_save = profile->format_on_save != 0;
    stored.format_on_type = profile->format_on_type != 0;
    stored.format_on_paste = profile->format_on_paste != 0;
    stored.organise_imports = profile->organise_imports != 0;
    stored.enabled = profile->enabled != 0;
    registry->profiles[index] = stored;
    registry->revision = next_revision(registry->revision);
    return UMI_STATUS_OK;
}

/*
 * Remove editor formatting profile registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_editor_formatting_profile_registry_remove(
    UmiEditorFormattingProfileRegistry *registry,
    const char *profile_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || profile_id == NULL || profile_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_profile(registry, profile_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < registry->count) {
        (void)memmove(&registry->profiles[index],
                      &registry->profiles[index + 1U],
                      (registry->count - index - 1U) *
                          sizeof(*registry->profiles));
    }
    --registry->count;
    registry->revision = next_revision(registry->revision);
    return UMI_STATUS_OK;
}

/*
 * Find editor formatting profile registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_formatting_profile_registry_find(
    const UmiEditorFormattingProfileRegistry *registry,
    const char *profile_id,
    UmiEditorFormattingProfileItem *out_profile)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || profile_id == NULL || out_profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_profile(registry, profile_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_profile = registry->profiles[index];
    return UMI_STATUS_OK;
}

/*
 * Provide the editor formatting profile registry resolve operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_formatting_profile_registry_resolve(
    const UmiEditorFormattingProfileRegistry *registry,
    const char *language_id,
    UmiEditorFormattingProfileItem *out_profile)
{
    size_t index;
    size_t best = SIZE_MAX;
    int exact_found = 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || language_id == NULL || language_id[0] == '\0' ||
        out_profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        const UmiEditorFormattingProfileItem *candidate =
            &registry->profiles[index];
        int exact = strcmp(candidate->language_id, language_id) == 0;
        int fallback = strcmp(candidate->language_id, "*") == 0;

        /* Apply this operation only while the related capability or state is available. */
        if (!candidate->enabled || (!exact && !fallback)) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (best == SIZE_MAX || (exact && !exact_found) ||
            (exact == exact_found &&
             candidate->priority > registry->profiles[best].priority) ||
            (exact == exact_found &&
             candidate->priority == registry->profiles[best].priority &&
             strcmp(candidate->id, registry->profiles[best].id) < 0)) {
            best = index;
            exact_found = exact;
        }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (best == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_profile = registry->profiles[best];
    return UMI_STATUS_OK;
}

/*
 * Find editor formatting profile registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_formatting_profile_registry_at(
    const UmiEditorFormattingProfileRegistry *registry,
    size_t index,
    UmiEditorFormattingProfileItem *out_profile)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_profile = registry->profiles[index];
    return UMI_STATUS_OK;
}

/*
 * Provide the editor formatting profile registry snapshot operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_formatting_profile_registry_snapshot(
    const UmiEditorFormattingProfileRegistry *registry,
    UmiEditorFormattingProfileSnapshot *out_snapshot)
{
    size_t index;
    size_t comparison;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_FORMATTING_PROFILE_API_VERSION;
    out_snapshot->profile_count = registry->count;
    out_snapshot->revision = registry->revision;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        int first_language = 1;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (registry->profiles[index].enabled) {
            ++out_snapshot->enabled_profile_count;
        }
        /* Visit each bounded item once so every record receives the same rule. */
        for (comparison = 0U; comparison < index; ++comparison) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (strcmp(registry->profiles[index].language_id,
                       registry->profiles[comparison].language_id) == 0) {
                first_language = 0;
                break;
            }
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (first_language) ++out_snapshot->language_count;
    }
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by editor formatting profile registry without
 * changing their state.
 */
size_t umi_editor_formatting_profile_registry_count(
    const UmiEditorFormattingProfileRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Provide the editor formatting profile registry revision operation used by this module
 * and its client applications.
 */
uint64_t umi_editor_formatting_profile_registry_revision(
    const UmiEditorFormattingProfileRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}
