/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/builtin_languages.h
 *
 * PURPOSE:
 *   Enumerate and register all built-in project/language packs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_BUILTIN_LANGUAGES_H
#define UMICOM_DEVELOPER_PROJECT_BUILTIN_LANGUAGES_H

#include "umicom/developer_project/language_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t umi_developer_project_builtin_language_count(void);

const UmiDeveloperProjectLanguagePack *
umi_developer_project_builtin_language_at(size_t index);

UmiStatus umi_developer_project_builtin_languages_register(
    UmiDeveloperProjectLanguageRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
