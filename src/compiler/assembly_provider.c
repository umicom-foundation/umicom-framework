/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/assembly_provider.c
 *
 * PURPOSE:
 *   Implement the assembly provider behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Native Assembler provider | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/assembly_provider.h"
#include "provider_defaults.h"
/*
 * Provide the compiler assembly provider operation used by this module and its client
 * applications.
 */
UmiCompilerProvider umi_compiler_assembly_provider(const char *executable,const char *target)
{
    return umi_compiler_provider_make("assembler","Native Assembler",executable,target,UMI_COMPILER_FAMILY_ASSEMBLER,UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_ASSEMBLY),true,false,true,true);
}
