/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/server_profile.c
 *
 * PURPOSE:
 *   Implement the server profile behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework language-server profiles. Sammy Hegab, Umicom Foundation, MIT. */
#include "umicom/language/server_profile.h"
#include <stdlib.h>
#include <string.h>

struct UmiLanguageServerProfileRegistry {
    UmiLanguageServerProfile items[UMI_LANGUAGE_SERVER_PROFILE_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiLanguageServerProfileRegistry *registry,
                         const char *id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->items[index].id, id) == 0) return index;
    }
    return SIZE_MAX;
}

/*
 * Initialise language server profile registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_language_server_profile_registry_create(
    UmiLanguageServerProfileRegistry **out_registry)
{
    UmiLanguageServerProfileRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiLanguageServerProfileRegistry *)calloc(1U, sizeof(*registry));
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
 * Release or reset state held by language server profile registry so the same storage can
 * be reused safely.
 */
void umi_language_server_profile_registry_destroy(
    UmiLanguageServerProfileRegistry *registry)
{
    free(registry);
}

/*
 * Provide the language server profile registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_server_profile_registry_upsert(
    UmiLanguageServerProfileRegistry *registry,
    const UmiLanguageServerProfile *profile)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || profile == NULL || profile->id[0] == '\0' ||
        profile->executable[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, profile->id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (registry->count >= UMI_LANGUAGE_SERVER_PROFILE_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = registry->count++;
    }
    registry->items[index] = *profile;
    registry->items[index].id[127U] = '\0';
    registry->items[index].display_name[255U] = '\0';
    registry->items[index].executable[1023U] = '\0';
    registry->items[index].arguments[2047U] = '\0';
    registry->items[index].language_ids[511U] = '\0';
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    return UMI_STATUS_OK;
}

/*
 * Find language server profile registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_language_server_profile_registry_find(
    const UmiLanguageServerProfileRegistry *registry, const char *id,
    UmiLanguageServerProfile *out_profile)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || id == NULL || out_profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(registry, id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_profile = registry->items[index];
    return UMI_STATUS_OK;
}

/*
 * Find language server profile registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_language_server_profile_registry_at(
    const UmiLanguageServerProfileRegistry *registry, size_t index,
    UmiLanguageServerProfile *out_profile)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_profile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_profile = registry->items[index];
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by language server profile registry without
 * changing their state.
 */
size_t umi_language_server_profile_registry_count(
    const UmiLanguageServerProfileRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Provide the language server profile supports operation used by this module and its
 * client applications.
 */
int umi_language_server_profile_supports(
    const UmiLanguageServerProfile *profile, uint64_t capability)
{
    return profile != NULL && capability != 0U &&
        (profile->capabilities & capability) == capability;
}
