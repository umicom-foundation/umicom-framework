/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/model_registry.c
 *
 * PURPOSE:
 *   Implement bounded project-model storage and active selection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/model_registry.h"

#include <stdlib.h>
#include <string.h>

struct UmiDeveloperProjectModelRegistry {
    UmiDeveloperProjectModel models[UMI_DEVELOPER_PROJECT_MODEL_CAPACITY];
    size_t count;
    size_t active_index;
    int has_active;
};

static size_t find_index(
    const UmiDeveloperProjectModelRegistry *registry,
    const char *project_id)
{
    size_t index;

    if (registry == NULL || project_id == NULL) return (size_t)-1;

    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->models[index].project_id, project_id) == 0) {
            return index;
        }
    }

    return (size_t)-1;
}

UmiStatus umi_developer_project_model_registry_create(
    UmiDeveloperProjectModelRegistry **out_registry)
{
    UmiDeveloperProjectModelRegistry *registry;

    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;

    registry = (UmiDeveloperProjectModelRegistry *)calloc(
        1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_developer_project_model_registry_destroy(
    UmiDeveloperProjectModelRegistry *registry)
{
    free(registry);
}

UmiStatus umi_developer_project_model_registry_upsert(
    UmiDeveloperProjectModelRegistry *registry,
    const UmiDeveloperProjectModel *model)
{
    size_t index;
    UmiStatus status;

    if (registry == NULL || model == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_project_model_validate(model, NULL, 0U);
    if (status != UMI_STATUS_OK) return status;

    index = find_index(registry, model->project_id);
    if (index == (size_t)-1) {
        if (registry->count >= UMI_DEVELOPER_PROJECT_MODEL_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = registry->count++;
    }

    registry->models[index] = *model;

    if (!registry->has_active) {
        registry->active_index = index;
        registry->has_active = 1;
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_developer_project_model_registry_remove(
    UmiDeveloperProjectModelRegistry *registry,
    const char *project_id)
{
    size_t index;
    size_t tail;

    if (registry == NULL || project_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_index(registry, project_id);
    if (index == (size_t)-1) return UMI_STATUS_NOT_FOUND;

    tail = registry->count - index - 1U;
    if (tail > 0U) {
        (void)memmove(
            &registry->models[index],
            &registry->models[index + 1U],
            tail * sizeof(registry->models[0]));
    }

    registry->count -= 1U;
    (void)memset(
        &registry->models[registry->count],
        0,
        sizeof(registry->models[0]));

    if (registry->count == 0U) {
        registry->has_active = 0;
        registry->active_index = 0U;
    } else if (registry->active_index >= registry->count) {
        registry->active_index = registry->count - 1U;
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_developer_project_model_registry_find(
    const UmiDeveloperProjectModelRegistry *registry,
    const char *project_id,
    UmiDeveloperProjectModel *out_model)
{
    size_t index;

    if (registry == NULL || project_id == NULL || out_model == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_index(registry, project_id);
    if (index == (size_t)-1) return UMI_STATUS_NOT_FOUND;

    *out_model = registry->models[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_project_model_registry_at(
    const UmiDeveloperProjectModelRegistry *registry,
    size_t index,
    UmiDeveloperProjectModel *out_model)
{
    if (registry == NULL || out_model == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;

    *out_model = registry->models[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_project_model_registry_activate(
    UmiDeveloperProjectModelRegistry *registry,
    const char *project_id)
{
    size_t index;

    if (registry == NULL || project_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_index(registry, project_id);
    if (index == (size_t)-1) return UMI_STATUS_NOT_FOUND;

    registry->active_index = index;
    registry->has_active = 1;
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_project_model_registry_active(
    const UmiDeveloperProjectModelRegistry *registry,
    UmiDeveloperProjectModel *out_model)
{
    if (registry == NULL || out_model == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (!registry->has_active ||
        registry->active_index >= registry->count) {
        return UMI_STATUS_NOT_FOUND;
    }

    *out_model = registry->models[registry->active_index];
    return UMI_STATUS_OK;
}

size_t umi_developer_project_model_registry_count(
    const UmiDeveloperProjectModelRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}
