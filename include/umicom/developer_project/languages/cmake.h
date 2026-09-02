/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/languages/cmake.h
 *
 * PURPOSE:
 *   Publish the built-in CMake language pack for project models, generation,
 *   build/run planning and future language-service adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_LANGUAGE_CMAKE_H
#define UMICOM_DEVELOPER_PROJECT_LANGUAGE_CMAKE_H

#include "umicom/developer_project/language_pack.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer project language cmake operation used by this module and its
 * client applications.
 */
const UmiDeveloperProjectLanguagePack *umi_developer_project_language_cmake(void);

#ifdef __cplusplus
}
#endif

#endif
