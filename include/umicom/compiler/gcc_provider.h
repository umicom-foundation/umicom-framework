/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/gcc_provider.h
 *
 * PURPOSE:
 *   Publish the public gcc provider contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_GCC_PROVIDER_H
#define UMICOM_COMPILER_GCC_PROVIDER_H
#include "umicom/compiler/provider.h"
/**
 * Provide the compiler gcc provider operation used by this module and its client
 * applications.
 */
UmiCompilerProvider umi_compiler_gcc_provider(const char *executable,const char *target);
#endif
