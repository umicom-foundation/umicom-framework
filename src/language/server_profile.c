/* Umicom Framework language-server profiles. Sammy Hegab, Umicom Foundation, MIT. */
#include "umicom/language/server_profile.h"
#include <stdlib.h>
#include <string.h>

struct UmiLanguageServerProfileRegistry {
    UmiLanguageServerProfile items[UMI_LANGUAGE_SERVER_PROFILE_CAPACITY];
    size_t count;
    uint64_t revision;
};

static size_t find_index(const UmiLanguageServerProfileRegistry *registry,
                         const char *id)
{
    size_t index;
    if (registry == NULL || id == NULL) return SIZE_MAX;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].id, id) == 0) return index;
    }
    return SIZE_MAX;
}

UmiStatus umi_language_server_profile_registry_create(
    UmiLanguageServerProfileRegistry **out_registry)
{
    UmiLanguageServerProfileRegistry *registry;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiLanguageServerProfileRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_language_server_profile_registry_destroy(
    UmiLanguageServerProfileRegistry *registry)
{
    free(registry);
}

UmiStatus umi_language_server_profile_registry_upsert(
    UmiLanguageServerProfileRegistry *registry,
    const UmiLanguageServerProfile *profile)
{
    size_t index;
    if (registry == NULL || profile == NULL || profile->id[0] == '\0' ||
        profile->executable[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, profile->id);
    if (index == SIZE_MAX) {
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

UmiStatus umi_language_server_profile_registry_find(
    const UmiLanguageServerProfileRegistry *registry, const char *id,
    UmiLanguageServerProfile *out_profile)
{
    size_t index;
    if (registry == NULL || id == NULL || out_profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(registry, id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_profile = registry->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_language_server_profile_registry_at(
    const UmiLanguageServerProfileRegistry *registry, size_t index,
    UmiLanguageServerProfile *out_profile)
{
    if (registry == NULL || out_profile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_profile = registry->items[index];
    return UMI_STATUS_OK;
}

size_t umi_language_server_profile_registry_count(
    const UmiLanguageServerProfileRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

int umi_language_server_profile_supports(
    const UmiLanguageServerProfile *profile, uint64_t capability)
{
    return profile != NULL && capability != 0U &&
        (profile->capabilities & capability) == capability;
}
