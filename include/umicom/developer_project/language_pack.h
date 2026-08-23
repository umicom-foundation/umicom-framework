/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/language_pack.h
 *
 * PURPOSE:
 *   Define reusable language/provider metadata for project creation, build/run
 *   planning and future language-service/LSP adapters.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_LANGUAGE_PACK_H
#define UMICOM_DEVELOPER_PROJECT_LANGUAGE_PACK_H

#include "umicom/developer_project/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_PROJECT_LANGUAGE_EXTENSION_CAPACITY 16U
#define UMI_DEVELOPER_PROJECT_LANGUAGE_TOOL_CAPACITY 12U

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

UmiStatus umi_developer_project_language_pack_validate(
    const UmiDeveloperProjectLanguagePack *pack);

int umi_developer_project_language_pack_supports_extension(
    const UmiDeveloperProjectLanguagePack *pack,
    const char *extension);

int umi_developer_project_language_pack_has_capability(
    const UmiDeveloperProjectLanguagePack *pack,
    UmiDeveloperProjectLanguageCapability capability);

#ifdef __cplusplus
}
#endif

#endif
