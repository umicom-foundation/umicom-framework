/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/msvc_provider.c
 *
 * PURPOSE:
 *   Implement the msvc provider behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Microsoft Visual C++ provider | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/msvc_provider.h"
#include "provider_defaults.h"
/*
 * Provide the compiler msvc provider operation used by this module and its client
 * applications.
 */
UmiCompilerProvider umi_compiler_msvc_provider(const char *executable,const char *target)
{
    return umi_compiler_provider_make("msvc","Microsoft Visual C++",executable,target,UMI_COMPILER_FAMILY_MSVC,UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_C) | UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_CPP),false,true,true,true);
}
