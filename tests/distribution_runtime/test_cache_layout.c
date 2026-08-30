/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_cache_layout.c
 *
 * PURPOSE:
 *   Focused regression coverage for cache namespace and eviction-budget configuration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/cache_layout.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrCacheLayout value; umi_dr_cache_layout_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"cache")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.namespace_id,sizeof(value.namespace_id),"studio")==UMI_STATUS_OK); value.max_bytes=1024U; CHECK(umi_dr_cache_layout_valid(&value)); CHECK(umi_dr_cache_layout_fingerprint(&value) != 0U);
    return 0;
}
