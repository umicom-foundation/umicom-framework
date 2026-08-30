/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_cache_runtime_v2.c
 *
 * PURPOSE:
 *   Implement the test compiler cache runtime v2 behavior for
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
#include <string.h>
int main(void) { UmiCompilerCache cache = {0}; UmiCompilerCacheEntry entry = {0}; UmiCompilerRequest request; assert(umi_compiler_request_init(&request,"p","u",UMI_COMPILER_LANGUAGE_C,"a.c","a.o") == UMI_STATUS_OK); entry.key = umi_compiler_request_hash(&request); (void)strcpy(entry.artifact_path,"a.o"); entry.created_at_ns = 5U; entry.valid = true; assert(umi_compiler_cache_put(&cache,&entry) == UMI_STATUS_OK); assert(umi_compiler_cache_get(&cache,entry.key) != NULL); assert(cache.hits == 1U); umi_compiler_cache_invalidate(&cache,6U); assert(umi_compiler_cache_get(&cache,entry.key) == NULL); assert(cache.misses == 1U); return 0; }
