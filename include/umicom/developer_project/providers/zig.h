/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/providers/zig.h
 *
 * PURPOSE:
 *   Publish the built-in Zig Build project provider.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_PROVIDER_ZIG_H
#define UMICOM_DEVELOPER_PROJECT_PROVIDER_ZIG_H

#include "umicom/developer_project/language_provider.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer project provider zig operation used by this module and its client
 * applications.
 */
const UmiDeveloperProjectLanguageProvider *umi_developer_project_provider_zig(void);

#ifdef __cplusplus
}
#endif

#endif
