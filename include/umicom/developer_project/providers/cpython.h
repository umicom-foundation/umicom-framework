/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/providers/cpython.h
 *
 * PURPOSE:
 *   Publish the built-in CPython Script project provider.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_PROVIDER_CPYTHON_H
#define UMICOM_DEVELOPER_PROJECT_PROVIDER_CPYTHON_H

#include "umicom/developer_project/language_provider.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiDeveloperProjectLanguageProvider *umi_developer_project_provider_cpython(void);

#ifdef __cplusplus
}
#endif

#endif
