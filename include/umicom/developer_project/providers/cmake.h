/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/providers/cmake.h
 *
 * PURPOSE:
 *   Publish the built-in CMake/Ninja project provider.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_PROVIDER_CMAKE_H
#define UMICOM_DEVELOPER_PROJECT_PROVIDER_CMAKE_H

#include "umicom/developer_project/language_provider.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer project provider cmake operation used by this module and its
 * client applications.
 */
const UmiDeveloperProjectLanguageProvider *umi_developer_project_provider_cmake(void);

#ifdef __cplusplus
}
#endif

#endif
