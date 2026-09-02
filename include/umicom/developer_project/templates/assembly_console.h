/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/templates/assembly_console.h
 *
 * PURPOSE:
 *   Publish the built-in Assembly Console Project project template.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_TEMPLATE_ASSEMBLY_CONSOLE_H
#define UMICOM_DEVELOPER_PROJECT_TEMPLATE_ASSEMBLY_CONSOLE_H

#include "umicom/developer_project/template.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer project template assembly console operation used by this module
 * and its client applications.
 */
const UmiDeveloperProjectTemplate *umi_developer_project_template_assembly_console(void);

#ifdef __cplusplus
}
#endif

#endif
