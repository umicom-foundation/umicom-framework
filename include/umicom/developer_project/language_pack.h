/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/language_pack.h
 *
 * PURPOSE:
 *   Define reusable language/provider metadata for project creation, build/run
 *   planning and future language-service/LSP adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_LANGUAGE_PACK_H
#define UMICOM_DEVELOPER_PROJECT_LANGUAGE_PACK_H

#include "umicom/developer_project/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_PROJECT_LANGUAGE_EXTENSION_CAPACITY 16U
#define UMI_DEVELOPER_PROJECT_LANGUAGE_TOOL_CAPACITY 12U

/**
 * Represent the developer project language pack data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperProjectLanguagePack {
    uint32_t structure_size;
    uint32_t api_version;
    const char *language_id;
    const char *title;
    const char *description;
    const char *const *extensions;
    size_t extension_count;
    const char *const *build_tools;
    size_t build_tool_count;
    const char *language_server;
    const char *formatter;
    const char *debug_adapter;
    UmiDeveloperProjectLanguageCapabilities capabilities;
    int native_language;
    int compiled_language;
    int built_in;
} UmiDeveloperProjectLanguagePack;

/**
 * Check that developer project language pack satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_developer_project_language_pack_validate(
    const UmiDeveloperProjectLanguagePack *pack);

/**
 * Provide the developer project language pack supports extension operation used by this
 * module and its client applications.
 */
int umi_developer_project_language_pack_supports_extension(
    const UmiDeveloperProjectLanguagePack *pack,
    const char *extension);

/**
 * Provide the developer project language pack has capability operation used by this module
 * and its client applications.
 */
int umi_developer_project_language_pack_has_capability(
    const UmiDeveloperProjectLanguagePack *pack,
    UmiDeveloperProjectLanguageCapability capability);

#ifdef __cplusplus
}
#endif

#endif
