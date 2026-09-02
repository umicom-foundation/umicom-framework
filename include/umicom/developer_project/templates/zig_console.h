/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/templates/zig_console.h
 *
 * PURPOSE:
 *   Publish the built-in Zig Console Application project template.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_TEMPLATE_ZIG_CONSOLE_H
#define UMICOM_DEVELOPER_PROJECT_TEMPLATE_ZIG_CONSOLE_H

#include "umicom/developer_project/template.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer project template zig console operation used by this module and its
 * client applications.
 */
const UmiDeveloperProjectTemplate *umi_developer_project_template_zig_console(void);

#ifdef __cplusplus
}
#endif

#endif
