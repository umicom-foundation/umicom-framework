/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/language_registry.h
 *
 * PURPOSE:
 *   Register reusable built-in and extension-provided language packs.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_LANGUAGE_REGISTRY_H
#define UMICOM_DEVELOPER_PROJECT_LANGUAGE_REGISTRY_H

#include "umicom/developer_project/language_pack.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeveloperProjectLanguageRegistry
    UmiDeveloperProjectLanguageRegistry;

UmiStatus umi_developer_project_language_registry_create(
    UmiDeveloperProjectLanguageRegistry **out_registry);

void umi_developer_project_language_registry_destroy(
    UmiDeveloperProjectLanguageRegistry *registry);

UmiStatus umi_developer_project_language_registry_register(
    UmiDeveloperProjectLanguageRegistry *registry,
    const UmiDeveloperProjectLanguagePack *pack);

const UmiDeveloperProjectLanguagePack *
umi_developer_project_language_registry_find(
    const UmiDeveloperProjectLanguageRegistry *registry,
    const char *language_id);

const UmiDeveloperProjectLanguagePack *
umi_developer_project_language_registry_for_extension(
    const UmiDeveloperProjectLanguageRegistry *registry,
    const char *extension);

const UmiDeveloperProjectLanguagePack *
umi_developer_project_language_registry_at(
    const UmiDeveloperProjectLanguageRegistry *registry,
    size_t index);

size_t umi_developer_project_language_registry_count(
    const UmiDeveloperProjectLanguageRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
