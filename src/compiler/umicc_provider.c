/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/umicc_provider.c
 *
 * PURPOSE:
 *   Implement the umicc provider behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Umicom Compiler provider | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/umicc_provider.h"
#include "provider_defaults.h"
UmiCompilerProvider umi_compiler_umicc_provider(const char *executable,const char *target)
{
    return umi_compiler_provider_make("umicc","Umicom Compiler",executable,target,UMI_COMPILER_FAMILY_UMICC,UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_C) | UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_CPP) | UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_ASSEMBLY) | UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_RUST) | UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_ZIG) | UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_UAI),true,true,true,true);
}
