/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/languages/zig.h
 *
 * PURPOSE:
 *   Publish the built-in Zig language pack for project models, generation,
 *   build/run planning and future language-service adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_LANGUAGE_ZIG_H
#define UMICOM_DEVELOPER_PROJECT_LANGUAGE_ZIG_H

#include "umicom/developer_project/language_pack.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiDeveloperProjectLanguagePack *umi_developer_project_language_zig(void);

#ifdef __cplusplus
}
#endif

#endif
