/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/zig_provider.c
 *
 * PURPOSE:
 *   Implement the zig provider behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Zig Compiler provider | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/zig_provider.h"
#include "provider_defaults.h"
/*
 * Provide the compiler zig provider operation used by this module and its client
 * applications.
 */
UmiCompilerProvider umi_compiler_zig_provider(const char *executable,const char *target)
{
    return umi_compiler_provider_make("zig","Zig Compiler",executable,target,UMI_COMPILER_FAMILY_ZIG,UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_ZIG) | UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_C) | UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_CPP) | UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_ASSEMBLY),true,true,true,true);
}
