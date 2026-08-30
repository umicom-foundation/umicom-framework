/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/languages/cpp.h
 *
 * PURPOSE:
 *   Publish the built-in C++ language pack for project models, generation,
 *   build/run planning and future language-service adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_LANGUAGE_CPP_H
#define UMICOM_DEVELOPER_PROJECT_LANGUAGE_CPP_H

#include "umicom/developer_project/language_pack.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiDeveloperProjectLanguagePack *umi_developer_project_language_cpp(void);

#ifdef __cplusplus
}
#endif

#endif
