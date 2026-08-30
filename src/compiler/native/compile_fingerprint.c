/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/compile_fingerprint.c
 *
 * PURPOSE:
 *   Compose stable compilation fingerprints from source, options, target and dependency state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/compile_fingerprint.h"
uint64_t umi_nc_compile_fingerprint(const UmiNativeCompileFingerprintInput *i){if(i==NULL)return 0U;uint64_t h=1469598103934665603ULL;const uint64_t v[]={i->source_hash,i->options_hash,i->target_hash,i->dependency_hash,i->compiler_revision_hash};for(size_t n=0U;n<sizeof(v)/sizeof(v[0]);n++){h^=v[n];h*=1099511628211ULL;h^=h>>32U;}return h;}
