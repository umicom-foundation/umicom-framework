/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/templates/bash_tool.h
 *
 * PURPOSE:
 *   Publish the built-in Bash Tool project template.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_TEMPLATE_BASH_TOOL_H
#define UMICOM_DEVELOPER_PROJECT_TEMPLATE_BASH_TOOL_H

#include "umicom/developer_project/template.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer project template bash tool operation used by this module and its
 * client applications.
 */
const UmiDeveloperProjectTemplate *umi_developer_project_template_bash_tool(void);

#ifdef __cplusplus
}
#endif

#endif
