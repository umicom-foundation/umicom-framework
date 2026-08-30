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

UmiStatus umi_developer_project_template_registry_create(
    UmiDeveloperProjectTemplateRegistry **out_registry)
{
    UmiDeveloperProjectTemplateRegistry *registry;

    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;

    registry = (UmiDeveloperProjectTemplateRegistry *)calloc(
        1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_developer_project_template_registry_destroy(
    UmiDeveloperProjectTemplateRegistry *registry)
{
    free(registry);
}

UmiStatus umi_developer_project_template_registry_register(
    UmiDeveloperProjectTemplateRegistry *registry,
    const UmiDeveloperProjectTemplate *project_template)
{
    size_t index;
    UmiStatus status;

    if (registry == NULL || project_template == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_project_template_validate(project_template);
    if (status != UMI_STATUS_OK) return status;

    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->templates[index]->template_id,
                   project_template->template_id) == 0) {
            registry->templates[index] = project_template;
            return UMI_STATUS_OK;
        }
    }

    if (registry->count >= UMI_DEVELOPER_PROJECT_TEMPLATE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    registry->templates[registry->count++] = project_template;
    return UMI_STATUS_OK;
}

const UmiDeveloperProjectTemplate *
umi_developer_project_template_registry_find(
    const UmiDeveloperProjectTemplateRegistry *registry,
    const char *template_id)
{
    size_t index;

    if (registry == NULL || template_id == NULL) return NULL;

    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->templates[index]->template_id,
                   template_id) == 0) {
            return registry->templates[index];
        }
    }

    return NULL;
}

const UmiDeveloperProjectTemplate *
umi_developer_project_template_registry_at(
    const UmiDeveloperProjectTemplateRegistry *registry,
    size_t index)
{
    return registry != NULL && index < registry->count
        ? registry->templates[index]
        : NULL;
}

size_t umi_developer_project_template_registry_count(
    const UmiDeveloperProjectTemplateRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}
