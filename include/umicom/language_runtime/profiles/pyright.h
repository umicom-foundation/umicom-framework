/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/profiles/pyright.h
 *
 * PURPOSE:
 *   Publish the built-in Pyright language-server profile.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_PROFILE_PYRIGHT_H
#define UMICOM_LANGUAGE_RUNTIME_PROFILE_PYRIGHT_H
#include "umicom/language/server_profile.h"
#ifdef __cplusplus
extern "C" {
#endif
const UmiLanguageServerProfile *umi_language_runtime_profile_pyright(void);
#ifdef __cplusplus
}
#endif
#endif
