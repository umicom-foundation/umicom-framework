/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/profiles/rust_analyzer.h
 *
 * PURPOSE:
 *   Publish the built-in rust-analyzer language-server profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_PROFILE_RUST_ANALYZER_H
#define UMICOM_LANGUAGE_RUNTIME_PROFILE_RUST_ANALYZER_H
#include "umicom/language/server_profile.h"
#ifdef __cplusplus
extern "C" {
#endif
const UmiLanguageServerProfile *umi_language_runtime_profile_rust_analyzer(void);
#ifdef __cplusplus
}
#endif
#endif
