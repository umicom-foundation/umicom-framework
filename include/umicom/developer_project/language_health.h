/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/language_health.h
 *
 * PURPOSE:
 *   Report whether the external tools advertised by a language pack are
 *   currently discoverable, so Studio can explain missing build/run capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_LANGUAGE_HEALTH_H
#define UMICOM_DEVELOPER_PROJECT_LANGUAGE_HEALTH_H

#include "umicom/developer_project/language_pack.h"
#include "umicom/toolchain/discovery.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeveloperProjectLanguageToolHealth {
    char tool[UMI_DEVELOPER_PROJECT_ID_CAPACITY];
    char path[UMI_DEVELOPER_PROJECT_PATH_CAPACITY];
    int available;
} UmiDeveloperProjectLanguageToolHealth;

typedef struct UmiDeveloperProjectLanguageHealth {
    char language_id[UMI_DEVELOPER_PROJECT_ID_CAPACITY];
    UmiDeveloperProjectLanguageToolHealth
        tools[UMI_DEVELOPER_PROJECT_LANGUAGE_TOOL_CAPACITY];
    size_t tool_count;
    size_t available_count;
    size_t missing_count;
    int ready;
} UmiDeveloperProjectLanguageHealth;

UmiStatus umi_developer_project_language_health_probe(
    const UmiDeveloperProjectLanguagePack *pack,
    UmiDeveloperProjectLanguageHealth *out_health);

#ifdef __cplusplus
}
#endif

#endif
