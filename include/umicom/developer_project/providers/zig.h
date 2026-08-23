/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/providers/zig.h
 *
 * PURPOSE:
 *   Publish the built-in Zig Build project provider.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_PROVIDER_ZIG_H
#define UMICOM_DEVELOPER_PROJECT_PROVIDER_ZIG_H

#include "umicom/developer_project/language_provider.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiDeveloperProjectLanguageProvider *umi_developer_project_provider_zig(void);

#ifdef __cplusplus
}
#endif

#endif
