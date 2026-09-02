/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/clang_provider.c
 *
 * PURPOSE:
 *   Implement the clang provider behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Clang / LLVM provider | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/clang_provider.h"
#include "provider_defaults.h"
/*
 * Provide the compiler clang provider operation used by this module and its client
 * applications.
 */
UmiCompilerProvider umi_compiler_clang_provider(const char *executable,const char *target)
{
    return umi_compiler_provider_make("clang","Clang / LLVM",executable,target,UMI_COMPILER_FAMILY_CLANG,UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_C) | UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_CPP) | UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_ASSEMBLY),true,true,true,true);
}
