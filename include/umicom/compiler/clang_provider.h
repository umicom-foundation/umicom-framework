/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/clang_provider.h
 *
 * PURPOSE:
 *   Publish the public clang provider contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_CLANG_PROVIDER_H
#define UMICOM_COMPILER_CLANG_PROVIDER_H
#include "umicom/compiler/provider.h"
UmiCompilerProvider umi_compiler_clang_provider(const char *executable,const char *target);
#endif
