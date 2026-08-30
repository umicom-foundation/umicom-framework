/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_valuation_cache.c
 *
 * PURPOSE:
 *   Implement the test valuation cache behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/enterprise/valuation_cache.h"

int main(void){ UmiEnterpriseValuationCache c; UmiEnterpriseValuationCacheEntry e; umi_enterprise_valuation_cache_init(&c); CHECK(umi_enterprise_valuation_cache_key_init(&e.key,"p",1U,"m")==UMI_STATUS_OK); e.present_value=99.0;e.stored_ms=1; CHECK(umi_enterprise_valuation_cache_put(&c,&e)==UMI_STATUS_OK); CHECK(umi_enterprise_valuation_cache_find(&c,e.key.fingerprint)->present_value==99.0); return 0; }
