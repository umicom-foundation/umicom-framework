/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/uai_provider.c
 *
 * PURPOSE:
 *   Implement the uai provider behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | UAI Language Compiler provider | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/uai_provider.h"
#include "provider_defaults.h"
/*
 * Provide the compiler uai provider operation used by this module and its client
 * applications.
 */
UmiCompilerProvider umi_compiler_uai_provider(const char *executable,const char *target)
{
    return umi_compiler_provider_make("uai","UAI Language Compiler",executable,target,UMI_COMPILER_FAMILY_UAI,UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_UAI),true,true,true,true);
}
