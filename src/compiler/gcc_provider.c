/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/gcc_provider.c
 *
 * PURPOSE:
 *   Implement the gcc provider behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | GNU GCC provider | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/gcc_provider.h"
#include "provider_defaults.h"
UmiCompilerProvider umi_compiler_gcc_provider(const char *executable,const char *target)
{
    return umi_compiler_provider_make("gcc","GNU GCC",executable,target,UMI_COMPILER_FAMILY_GCC,UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_C) | UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_CPP) | UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_ASSEMBLY),true,true,true,true);
}
