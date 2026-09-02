/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/templates/c23_shared_library.h
 *
 * PURPOSE:
 *   Publish the built-in C23 Shared Library project template.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_TEMPLATE_C23_SHARED_LIBRARY_H
#define UMICOM_DEVELOPER_PROJECT_TEMPLATE_C23_SHARED_LIBRARY_H

#include "umicom/developer_project/template.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer project template c23 shared library operation used by this module
 * and its client applications.
 */
const UmiDeveloperProjectTemplate *umi_developer_project_template_c23_shared_library(void);

#ifdef __cplusplus
}
#endif

#endif
