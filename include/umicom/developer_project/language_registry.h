/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/language_registry.h
 *
 * PURPOSE:
 *   Register reusable built-in and extension-provided language packs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_LANGUAGE_REGISTRY_H
#define UMICOM_DEVELOPER_PROJECT_LANGUAGE_REGISTRY_H

#include "umicom/developer_project/language_pack.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the developer project language registry data shared with callers of this
 * public contract.
 */
typedef struct UmiDeveloperProjectLanguageRegistry
    UmiDeveloperProjectLanguageRegistry;

/**
 * Initialise developer project language registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_developer_project_language_registry_create(
    UmiDeveloperProjectLanguageRegistry **out_registry);

/**
 * Release or reset state held by developer project language registry so the same storage
 * can be reused safely.
 */
void umi_developer_project_language_registry_destroy(
    UmiDeveloperProjectLanguageRegistry *registry);

/**
 * Add developer project language registry only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_developer_project_language_registry_register(
    UmiDeveloperProjectLanguageRegistry *registry,
    const UmiDeveloperProjectLanguagePack *pack);

/**
 * Find developer project language registry while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiDeveloperProjectLanguagePack *
umi_developer_project_language_registry_find(
    const UmiDeveloperProjectLanguageRegistry *registry,
    const char *language_id);

/**
 * Provide the developer project language registry for extension operation used by this
 * module and its client applications.
 */
const UmiDeveloperProjectLanguagePack *
umi_developer_project_language_registry_for_extension(
    const UmiDeveloperProjectLanguageRegistry *registry,
    const char *extension);

/**
 * Find developer project language registry while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiDeveloperProjectLanguagePack *
umi_developer_project_language_registry_at(
    const UmiDeveloperProjectLanguageRegistry *registry,
    size_t index);

/**
 * Return the number of records represented by developer project language registry without
 * changing their state.
 */
size_t umi_developer_project_language_registry_count(
    const UmiDeveloperProjectLanguageRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
