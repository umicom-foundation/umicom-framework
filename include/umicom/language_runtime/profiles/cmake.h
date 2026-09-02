/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/profiles/cmake.h
 *
 * PURPOSE:
 *   Publish the built-in CMake Language Server language-server profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_PROFILE_CMAKE_H
#define UMICOM_LANGUAGE_RUNTIME_PROFILE_CMAKE_H
#include "umicom/language/server_profile.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the language runtime profile cmake operation used by this module and its client
 * applications.
 */
const UmiLanguageServerProfile *umi_language_runtime_profile_cmake(void);
#ifdef __cplusplus
}
#endif
#endif
