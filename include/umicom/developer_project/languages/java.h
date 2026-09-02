/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/languages/java.h
 *
 * PURPOSE:
 *   Publish the built-in Java language pack for project models, generation,
 *   build/run planning and future language-service adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_LANGUAGE_JAVA_H
#define UMICOM_DEVELOPER_PROJECT_LANGUAGE_JAVA_H

#include "umicom/developer_project/language_pack.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer project language java operation used by this module and its client
 * applications.
 */
const UmiDeveloperProjectLanguagePack *umi_developer_project_language_java(void);

#ifdef __cplusplus
}
#endif

#endif
