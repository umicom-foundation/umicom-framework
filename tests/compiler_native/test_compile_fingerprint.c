/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_compile_fingerprint.c
 *
 * PURPOSE:
 *   Regression coverage for compose stable compilation fingerprints from source, options, target and dependency state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/compile_fingerprint.h"
int main(void){UmiNativeCompileFingerprintInput i={1U,2U,3U,4U,5U};uint64_t a=umi_nc_compile_fingerprint(&i),b=umi_nc_compile_fingerprint(&i);if(a==0U||a!=b)return 1;return 0;}
