/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_calibration_cache.c
 *
 * PURPOSE:
 *   Implement the test calibration cache behavior for
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

#include "umicom/finance/enterprise/calibration_cache.h"

int main(void){ UmiEnterpriseCalibrationCache c; UmiEnterpriseCalibrationCacheEntry e={{0},123U,1e-8}; umi_enterprise_calibration_cache_init(&c); CHECK(umi_quant_copy_text(e.object_id,sizeof e.object_id,"curve")==UMI_STATUS_OK); CHECK(umi_enterprise_calibration_cache_put(&c,&e)==UMI_STATUS_OK); CHECK(umi_enterprise_calibration_cache_find(&c,"curve",123U)!=NULL); return 0; }
