/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/builtin_templates.h
 *
 * PURPOSE:
 *   Enumerate and register all built-in project templates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_BUILTIN_TEMPLATES_H
#define UMICOM_DEVELOPER_PROJECT_BUILTIN_TEMPLATES_H

#include "umicom/developer_project/template_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Return the number of records represented by developer project builtin template without
 * changing their state.
 */
size_t umi_developer_project_builtin_template_count(void);

/**
 * Find developer project builtin template while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiDeveloperProjectTemplate *
umi_developer_project_builtin_template_at(size_t index);

/**
 * Add developer project builtin templates only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_developer_project_builtin_templates_register(
    UmiDeveloperProjectTemplateRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
