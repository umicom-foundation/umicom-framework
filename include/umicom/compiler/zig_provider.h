/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/zig_provider.h
 *
 * PURPOSE:
 *   Publish the public zig provider contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_ZIG_PROVIDER_H
#define UMICOM_COMPILER_ZIG_PROVIDER_H
#include "umicom/compiler/provider.h"
/**
 * Provide the compiler zig provider operation used by this module and its client
 * applications.
 */
UmiCompilerProvider umi_compiler_zig_provider(const char *executable,const char *target);
#endif
