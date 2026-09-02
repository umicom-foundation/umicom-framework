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

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(
    const UmiDeveloperProjectModelRegistry *registry,
    const char *project_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || project_id == NULL) return (size_t)-1;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->models[index].project_id, project_id) == 0) {
            return index;
        }
    }

    return (size_t)-1;
}

/*
 * Initialise developer project model registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_developer_project_model_registry_create(
    UmiDeveloperProjectModelRegistry **out_registry)
{
    UmiDeveloperProjectModelRegistry *registry;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;

    registry = (UmiDeveloperProjectModelRegistry *)calloc(
        1U, sizeof(*registry));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    *out_registry = registry;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by developer project model registry so the same storage can
 * be reused safely.
 */
void umi_developer_project_model_registry_destroy(
    UmiDeveloperProjectModelRegistry *registry)
{
    free(registry);
}

/*
 * Provide the developer project model registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_project_model_registry_upsert(
    UmiDeveloperProjectModelRegistry *registry,
    const UmiDeveloperProjectModel *model)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || model == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_project_model_validate(model, NULL, 0U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    index = find_index(registry, model->project_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == (size_t)-1) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (registry->count >= UMI_DEVELOPER_PROJECT_MODEL_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = registry->count++;
    }

    registry->models[index] = *model;

    /* Apply this operation only while the related capability or state is available. */
    if (!registry->has_active) {
        registry->active_index = index;
        registry->has_active = 1;
    }

    return UMI_STATUS_OK;
}

/*
 * Remove developer project model registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_developer_project_model_registry_remove(
    UmiDeveloperProjectModelRegistry *registry,
    const char *project_id)
{
    size_t index;
    size_t tail;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || project_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_index(registry, project_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == (size_t)-1) return UMI_STATUS_NOT_FOUND;

    tail = registry->count - index - 1U;
    /* Apply this branch only when its contract condition is satisfied. */
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

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count == 0U) {
        registry->has_active = 0;
        registry->active_index = 0U;
    } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (registry->active_index >= registry->count) {
        registry->active_index = registry->count - 1U;
    }

    return UMI_STATUS_OK;
}

/*
 * Find developer project model registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_developer_project_model_registry_find(
    const UmiDeveloperProjectModelRegistry *registry,
    const char *project_id,
    UmiDeveloperProjectModel *out_model)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || project_id == NULL || out_model == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_index(registry, project_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == (size_t)-1) return UMI_STATUS_NOT_FOUND;

    *out_model = registry->models[index];
    return UMI_STATUS_OK;
}

/*
 * Find developer project model registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_developer_project_model_registry_at(
    const UmiDeveloperProjectModelRegistry *registry,
    size_t index,
    UmiDeveloperProjectModel *out_model)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_model == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;

    *out_model = registry->models[index];
    return UMI_STATUS_OK;
}

/*
 * Provide the developer project model registry activate operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_project_model_registry_activate(
    UmiDeveloperProjectModelRegistry *registry,
    const char *project_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || project_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_index(registry, project_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == (size_t)-1) return UMI_STATUS_NOT_FOUND;

    registry->active_index = index;
    registry->has_active = 1;
    return UMI_STATUS_OK;
}

/*
 * Provide the developer project model registry active operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_project_model_registry_active(
    const UmiDeveloperProjectModelRegistry *registry,
    UmiDeveloperProjectModel *out_model)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_model == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this operation only while the related capability or state is available. */
    if (!registry->has_active ||
        registry->active_index >= registry->count) {
        return UMI_STATUS_NOT_FOUND;
    }

    *out_model = registry->models[registry->active_index];
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by developer project model registry without
 * changing their state.
 */
size_t umi_developer_project_model_registry_count(
    const UmiDeveloperProjectModelRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}
