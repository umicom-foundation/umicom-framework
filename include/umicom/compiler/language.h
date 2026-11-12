/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/language.h
 *
 * PURPOSE:
 *   Publish the public language contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_LANGUAGE_H
#define UMICOM_COMPILER_LANGUAGE_H
#include "umicom/compiler/common.h"

/* Keep the public C symbol names when this header is included directly by a
 * C++ client. Includes remain outside the linkage block; layouts are unchanged. */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the compiler language name operation used by this module and its client
 * applications.
 */
const char *umi_compiler_language_name(UmiCompilerLanguage language);
/**
 * Provide the compiler language default standard operation used by this module and its
 * client applications.
 */
const char *umi_compiler_language_default_standard(UmiCompilerLanguage language);
/**
 * Provide the compiler language extension operation used by this module and its client
 * applications.
 */
const char *umi_compiler_language_extension(UmiCompilerLanguage language);
/**
 * Provide the compiler language from extension operation used by this module and its
 * client applications.
 */
UmiCompilerLanguage umi_compiler_language_from_extension(const char *path);
/**
 * Provide the compiler language uses c abi operation used by this module and its client
 * applications.
 */
bool umi_compiler_language_uses_c_abi(UmiCompilerLanguage language);
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif
