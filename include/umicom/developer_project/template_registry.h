/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/template_registry.h
 *
 * PURPOSE:
 *   Register built-in and extension-provided project templates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_TEMPLATE_REGISTRY_H
#define UMICOM_DEVELOPER_PROJECT_TEMPLATE_REGISTRY_H

#include "umicom/developer_project/template.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeveloperProjectTemplateRegistry
    UmiDeveloperProjectTemplateRegistry;

UmiStatus umi_developer_project_template_registry_create(
    UmiDeveloperProjectTemplateRegistry **out_registry);

void umi_developer_project_template_registry_destroy(
    UmiDeveloperProjectTemplateRegistry *registry);

UmiStatus umi_developer_project_template_registry_register(
    UmiDeveloperProjectTemplateRegistry *registry,
    const UmiDeveloperProjectTemplate *project_template);

const UmiDeveloperProjectTemplate *
umi_developer_project_template_registry_find(
    const UmiDeveloperProjectTemplateRegistry *registry,
    const char *template_id);

const UmiDeveloperProjectTemplate *
umi_developer_project_template_registry_at(
    const UmiDeveloperProjectTemplateRegistry *registry,
    size_t index);

size_t umi_developer_project_template_registry_count(
    const UmiDeveloperProjectTemplateRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
