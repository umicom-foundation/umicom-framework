/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_provider_registry_v2.c
 *
 * PURPOSE:
 *   Implement the test compiler provider registry v2 behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Batch 58 test | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/compiler.h"
#include <assert.h>
int main(void) { UmiCompilerRegistry registry; UmiCompilerProvider gcc = umi_compiler_gcc_provider("gcc","x86_64-unknown-linux-gnu"); UmiCompilerProvider rust = umi_compiler_rust_provider("rustc","x86_64-unknown-linux-gnu"); umi_compiler_registry_init(&registry); assert(umi_compiler_registry_register(&registry,&gcc) == UMI_STATUS_OK); assert(umi_compiler_registry_register(&registry,&rust) == UMI_STATUS_OK); assert(umi_compiler_registry_resolve(&registry,UMI_COMPILER_LANGUAGE_RUST,true) != NULL); assert(umi_compiler_registry_register(&registry,&gcc) == UMI_STATUS_ALREADY_EXISTS); return 0; }
