/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_models/test_kv_token_plan.c
 *
 * PURPOSE:
 *   Check dense cache budgets, exact token limits, overflow and failure output preservation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/local_models/memory_plan.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);return 1;} } while(0)
int main(void)
{
    /* Independently calculated: 2 sequences * 3 layers * (2*4*2 + 2*4*2)
     * = 192 bytes per token, across key and value arrays. */
    UmiModelDenseKvShape shape={10,2,3,2,2,4,4,2,2};
    UmiModelKvTokenPlan plan;
    CHECK(UmiModelDenseKvPlanTokens(&shape,1920,&plan)==UMI_STATUS_OK);
    CHECK(plan.affordable_tokens==10 && plan.request_fits && plan.bytes_per_token==192 && plan.cache_bytes==1920);
    CHECK(UmiModelDenseKvPlanTokens(&shape,1919,&plan)==UMI_STATUS_OK);
    CHECK(plan.affordable_tokens==9 && !plan.request_fits && plan.unused_budget_bytes==191);
    CHECK(UmiModelDenseKvPlanTokens(&shape,0,&plan)==UMI_STATUS_OK && plan.affordable_tokens==0 && plan.cache_bytes==0);
    CHECK(UmiModelDenseKvPlanTokens(&shape,191,&plan)==UMI_STATUS_OK && plan.affordable_tokens==0 && plan.unused_budget_bytes==191);
    CHECK(UmiModelDenseKvPlanTokens(&shape,UINT64_MAX,&plan)==UMI_STATUS_OK && plan.affordable_tokens==10);
    shape.token_capacity=UINT64_MAX;
    CHECK(UmiModelDenseKvPlanTokens(&shape,1920,&plan)==UMI_STATUS_OK && plan.affordable_tokens==10 && !plan.request_fits);
    memset(&plan,0x55,sizeof plan); UmiModelKvTokenPlan before=plan;
    shape.key_element_bytes=3;
    CHECK(UmiModelDenseKvPlanTokens(&shape,100,&plan)==UMI_STATUS_INVALID_ARGUMENT && memcmp(&plan,&before,sizeof plan)==0);
    shape.key_element_bytes=2;shape.layers=UINT64_MAX;
    CHECK(UmiModelDenseKvPlanTokens(&shape,UINT64_MAX,&plan)==UMI_STATUS_CAPACITY_EXCEEDED && memcmp(&plan,&before,sizeof plan)==0);
    CHECK(UmiModelDenseKvPlanTokens(NULL,0,&plan)==UMI_STATUS_INVALID_ARGUMENT);
    shape.layers=3;shape.token_capacity=0;
    CHECK(UmiModelDenseKvPlanTokens(&shape,0,&plan)==UMI_STATUS_INVALID_ARGUMENT);
    shape.token_capacity=10;
    CHECK(UmiModelDenseKvPlanTokens(&shape,100,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    /* Exhaust small budgets against literal independent payload geometry. */
    for(uint64_t budget=0;budget<2300;++budget) {
        CHECK(UmiModelDenseKvPlanTokens(&shape,budget,&plan)==UMI_STATUS_OK);
        const uint64_t expected=budget/192<10?budget/192:10;
        CHECK(plan.affordable_tokens==expected && plan.cache_bytes==expected*192);
        CHECK(plan.unused_budget_bytes==budget-expected*192);
    }
    return 0;
}
