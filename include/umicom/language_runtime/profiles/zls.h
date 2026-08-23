/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/profiles/zls.h
 *
 * PURPOSE:
 *   Publish the built-in ZLS language-server profile.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_PROFILE_ZLS_H
#define UMICOM_LANGUAGE_RUNTIME_PROFILE_ZLS_H
#include "umicom/language/server_profile.h"
#ifdef __cplusplus
extern "C" {
#endif
const UmiLanguageServerProfile *umi_language_runtime_profile_zls(void);
#ifdef __cplusplus
}
#endif
#endif
