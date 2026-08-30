/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/providers/bash.h
 *
 * PURPOSE:
 *   Publish the built-in Bash Script project provider.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_PROVIDER_BASH_H
#define UMICOM_DEVELOPER_PROJECT_PROVIDER_BASH_H

#include "umicom/developer_project/language_provider.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiDeveloperProjectLanguageProvider *umi_developer_project_provider_bash(void);

#ifdef __cplusplus
}
#endif

#endif
