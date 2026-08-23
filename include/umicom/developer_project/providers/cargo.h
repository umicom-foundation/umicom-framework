/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/providers/cargo.h
 *
 * PURPOSE:
 *   Publish the built-in Cargo project provider.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_PROVIDER_CARGO_H
#define UMICOM_DEVELOPER_PROJECT_PROVIDER_CARGO_H

#include "umicom/developer_project/language_provider.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiDeveloperProjectLanguageProvider *umi_developer_project_provider_cargo(void);

#ifdef __cplusplus
}
#endif

#endif
