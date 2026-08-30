/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_profile_rust_analyzer.c
 *
 * PURPOSE:
 *   Verify the built-in rust-analyzer profile contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/language_runtime/profiles/rust_analyzer.h"
int main(void){const UmiLanguageServerProfile*p=umi_language_runtime_profile_rust_analyzer();assert(p!=NULL);assert(p->id[0]);assert(p->executable[0]);assert(p->capabilities!=0U);return 0;}
