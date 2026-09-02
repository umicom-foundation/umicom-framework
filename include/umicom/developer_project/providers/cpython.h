/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/providers/cpython.h
 *
 * PURPOSE:
 *   Publish the built-in CPython Script project provider.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_PROVIDER_CPYTHON_H
#define UMICOM_DEVELOPER_PROJECT_PROVIDER_CPYTHON_H

#include "umicom/developer_project/language_provider.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer project provider cpython operation used by this module and its
 * client applications.
 */
const UmiDeveloperProjectLanguageProvider *umi_developer_project_provider_cpython(void);

#ifdef __cplusplus
}
#endif

#endif
