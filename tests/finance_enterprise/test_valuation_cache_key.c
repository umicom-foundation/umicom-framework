#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/enterprise/valuation_cache_key.h"

int main(void){ UmiEnterpriseValuationCacheKey k; CHECK(umi_enterprise_valuation_cache_key_init(&k,"swap",123U,"discount")==UMI_STATUS_OK); CHECK(k.fingerprint!=0U); return 0; }
