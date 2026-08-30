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

size_t umi_developer_project_builtin_template_count(void);

const UmiDeveloperProjectTemplate *
umi_developer_project_builtin_template_at(size_t index);

UmiStatus umi_developer_project_builtin_templates_register(
    UmiDeveloperProjectTemplateRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
