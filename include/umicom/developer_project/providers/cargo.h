/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/providers/cargo.h
 *
 * PURPOSE:
 *   Publish the built-in Cargo project provider.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_PROVIDER_CARGO_H
#define UMICOM_DEVELOPER_PROJECT_PROVIDER_CARGO_H

#include "umicom/developer_project/language_provider.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer project provider cargo operation used by this module and its
 * client applications.
 */
const UmiDeveloperProjectLanguageProvider *umi_developer_project_provider_cargo(void);

#ifdef __cplusplus
}
#endif

#endif
