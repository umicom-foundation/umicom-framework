/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/template_registry.c
 *
 * PURPOSE:
 *   Implement bounded project-template registration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/template_registry.h"

#include <stdlib.h>
#include <string.h>

struct UmiDeveloperProjectTemplateRegistry {
    const UmiDeveloperProjectTemplate *
        templates[UMI_DEVELOPER_PROJECT_TEMPLATE_CAPACITY];
    size_t count;
};

/*
 * Initialise developer project template registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_developer_project_template_registry_create(
    UmiDeveloperProjectTemplateRegistry **out_registry)
{
    UmiDeveloperProjectTemplateRegistry *registry;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;

    registry = (UmiDeveloperProjectTemplateRegistry *)calloc(
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
 * Release or reset state held by developer project template registry so the same storage
 * can be reused safely.
 */
void umi_developer_project_template_registry_destroy(
    UmiDeveloperProjectTemplateRegistry *registry)
{
    free(registry);
}

/*
 * Add developer project template registry only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_developer_project_template_registry_register(
    UmiDeveloperProjectTemplateRegistry *registry,
    const UmiDeveloperProjectTemplate *project_template)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || project_template == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_project_template_validate(project_template);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->templates[index]->template_id,
                   project_template->template_id) == 0) {
            registry->templates[index] = project_template;
            return UMI_STATUS_OK;
        }
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_DEVELOPER_PROJECT_TEMPLATE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    registry->templates[registry->count++] = project_template;
    return UMI_STATUS_OK;
}

/*
 * Find developer project template registry while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiDeveloperProjectTemplate *
umi_developer_project_template_registry_find(
    const UmiDeveloperProjectTemplateRegistry *registry,
    const char *template_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || template_id == NULL) return NULL;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->templates[index]->template_id,
                   template_id) == 0) {
            return registry->templates[index];
        }
    }

    return NULL;
}

/*
 * Find developer project template registry while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiDeveloperProjectTemplate *
umi_developer_project_template_registry_at(
    const UmiDeveloperProjectTemplateRegistry *registry,
    size_t index)
{
    return registry != NULL && index < registry->count
        ? registry->templates[index]
        : NULL;
}

/*
 * Return the number of records represented by developer project template registry without
 * changing their state.
 */
size_t umi_developer_project_template_registry_count(
    const UmiDeveloperProjectTemplateRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}
