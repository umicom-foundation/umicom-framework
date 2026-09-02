/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/rust_provider.c
 *
 * PURPOSE:
 *   Implement the rust provider behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Rust Compiler provider | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/rust_provider.h"
#include "provider_defaults.h"
/*
 * Provide the compiler rust provider operation used by this module and its client
 * applications.
 */
UmiCompilerProvider umi_compiler_rust_provider(const char *executable,const char *target)
{
    return umi_compiler_provider_make("rustc","Rust Compiler",executable,target,UMI_COMPILER_FAMILY_RUSTC,UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_RUST),true,false,true,true);
}
