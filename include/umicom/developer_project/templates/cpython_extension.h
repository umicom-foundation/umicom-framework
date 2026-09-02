/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/templates/cpython_extension.h
 *
 * PURPOSE:
 *   Publish the built-in CPython Native Extension project template.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_TEMPLATE_CPYTHON_EXTENSION_H
#define UMICOM_DEVELOPER_PROJECT_TEMPLATE_CPYTHON_EXTENSION_H

#include "umicom/developer_project/template.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer project template cpython extension operation used by this module
 * and its client applications.
 */
const UmiDeveloperProjectTemplate *umi_developer_project_template_cpython_extension(void);

#ifdef __cplusplus
}
#endif

#endif
