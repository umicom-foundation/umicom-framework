/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/template_registry.h
 *
 * PURPOSE:
 *   Register built-in and extension-provided project templates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_TEMPLATE_REGISTRY_H
#define UMICOM_DEVELOPER_PROJECT_TEMPLATE_REGISTRY_H

#include "umicom/developer_project/template.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the developer project template registry data shared with callers of this
 * public contract.
 */
typedef struct UmiDeveloperProjectTemplateRegistry
    UmiDeveloperProjectTemplateRegistry;

/**
 * Initialise developer project template registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_developer_project_template_registry_create(
    UmiDeveloperProjectTemplateRegistry **out_registry);

/**
 * Release or reset state held by developer project template registry so the same storage
 * can be reused safely.
 */
void umi_developer_project_template_registry_destroy(
    UmiDeveloperProjectTemplateRegistry *registry);

/**
 * Add developer project template registry only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_developer_project_template_registry_register(
    UmiDeveloperProjectTemplateRegistry *registry,
    const UmiDeveloperProjectTemplate *project_template);

/**
 * Find developer project template registry while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiDeveloperProjectTemplate *
umi_developer_project_template_registry_find(
    const UmiDeveloperProjectTemplateRegistry *registry,
    const char *template_id);

/**
 * Find developer project template registry while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiDeveloperProjectTemplate *
umi_developer_project_template_registry_at(
    const UmiDeveloperProjectTemplateRegistry *registry,
    size_t index);

/**
 * Return the number of records represented by developer project template registry without
 * changing their state.
 */
size_t umi_developer_project_template_registry_count(
    const UmiDeveloperProjectTemplateRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
