/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/profiles/clangd.h
 *
 * PURPOSE:
 *   Publish the built-in Clangd language-server profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_PROFILE_CLANGD_H
#define UMICOM_LANGUAGE_RUNTIME_PROFILE_CLANGD_H
#include "umicom/language/server_profile.h"
#ifdef __cplusplus
extern "C" {
#endif
const UmiLanguageServerProfile *umi_language_runtime_profile_clangd(void);
#ifdef __cplusplus
}
#endif
#endif
