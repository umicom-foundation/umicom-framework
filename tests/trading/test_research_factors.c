/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_research_factors.c
 *
 * PURPOSE:
 *   Validate research factors behaviour in the trading foundation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This focused regression test uses deterministic values so changes to the trading contract are visible immediately.
 */

#include <assert.h>
#include <stdio.h>
#include "umicom/trading/trading.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){
    UmiFeatureVector v;umi_feature_vector_init(&v);UmiMarketFactor f={0};(void)snprintf(f.name,sizeof(f.name),"%s","VIX");f.value_before=18;f.value_after=20;f.contribution=umi_factor_contribution(umi_factor_change(&f),0.5);
    assert(umi_feature_vector_add(&v,&f)==UMI_STATUS_OK);assert(v.count==1U);assert(f.contribution==1.0);return 0;
}
