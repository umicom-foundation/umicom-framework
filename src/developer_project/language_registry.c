/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/language_registry.c
 *
 * PURPOSE:
 *   Implement bounded language-pack registration and lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/language_registry.h"

#include <stdlib.h>
#include <string.h>

struct UmiDeveloperProjectLanguageRegistry {
    const UmiDeveloperProjectLanguagePack *
        packs[UMI_DEVELOPER_PROJECT_LANGUAGE_CAPACITY];
    size_t count;
};

UmiStatus umi_developer_project_language_registry_create(
    UmiDeveloperProjectLanguageRegistry **out_registry)
{
    UmiDeveloperProjectLanguageRegistry *registry;

    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;

    registry = (UmiDeveloperProjectLanguageRegistry *)calloc(
        1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_developer_project_language_registry_destroy(
    UmiDeveloperProjectLanguageRegistry *registry)
{
    free(registry);
}

UmiStatus umi_developer_project_language_registry_register(
    UmiDeveloperProjectLanguageRegistry *registry,
    const UmiDeveloperProjectLanguagePack *pack)
{
    size_t index;
    UmiStatus status;

    if (registry == NULL || pack == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_project_language_pack_validate(pack);
    if (status != UMI_STATUS_OK) return status;

    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->packs[index]->language_id,
                   pack->language_id) == 0) {
            registry->packs[index] = pack;
            return UMI_STATUS_OK;
        }
    }

    if (registry->count >= UMI_DEVELOPER_PROJECT_LANGUAGE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    registry->packs[registry->count++] = pack;
    return UMI_STATUS_OK;
}

const UmiDeveloperProjectLanguagePack *
umi_developer_project_language_registry_find(
    const UmiDeveloperProjectLanguageRegistry *registry,
    const char *language_id)
{
    size_t index;

    if (registry == NULL || language_id == NULL) return NULL;

    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->packs[index]->language_id,
                   language_id) == 0) {
            return registry->packs[index];
        }
    }

    return NULL;
}

const UmiDeveloperProjectLanguagePack *
umi_developer_project_language_registry_for_extension(
    const UmiDeveloperProjectLanguageRegistry *registry,
    const char *extension)
{
    size_t index;

    if (registry == NULL || extension == NULL) return NULL;

    for (index = 0U; index < registry->count; ++index) {
        if (umi_developer_project_language_pack_supports_extension(
                registry->packs[index], extension)) {
            return registry->packs[index];
        }
    }

    return NULL;
}

const UmiDeveloperProjectLanguagePack *
umi_developer_project_language_registry_at(
    const UmiDeveloperProjectLanguageRegistry *registry,
    size_t index)
{
    return registry != NULL && index < registry->count
        ? registry->packs[index]
        : NULL;
}

size_t umi_developer_project_language_registry_count(
    const UmiDeveloperProjectLanguageRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}
