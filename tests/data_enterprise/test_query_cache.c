/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_query_cache.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the query cache enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/query_cache.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataQueryCache c; uint64_t fp=0U; umi_data_query_cache_init(&c); CHECK(umi_data_query_cache_put(&c,"q1",1234U,10U)==UMI_STATUS_OK); CHECK(umi_data_query_cache_hit(&c,"q1",&fp)==UMI_STATUS_OK); CHECK(fp==1234U); CHECK(c.items[0].hits==1U); umi_data_query_cache_invalidate_all(&c); CHECK(umi_data_query_cache_hit(&c,"q1",&fp)==UMI_STATUS_NOT_FOUND);
    return 0;
}
