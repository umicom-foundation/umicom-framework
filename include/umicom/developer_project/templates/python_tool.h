/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/templates/python_tool.h
 *
 * PURPOSE:
 *   Publish the built-in Python Tool project template.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_TEMPLATE_PYTHON_TOOL_H
#define UMICOM_DEVELOPER_PROJECT_TEMPLATE_PYTHON_TOOL_H

#include "umicom/developer_project/template.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer project template python tool operation used by this module and its
 * client applications.
 */
const UmiDeveloperProjectTemplate *umi_developer_project_template_python_tool(void);

#ifdef __cplusplus
}
#endif

#endif
