/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_patch_budget.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web patch budget.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "umicom/frontend/native_web/patch_budget.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
int main(void)
{
    UmiNativeWebPatchBatch b; UmiNativeWebPatchBudget p={1U,128U}; size_t bytes=0U; umi_native_web_patch_batch_init(&b,1U); CHECK(umi_native_web_patch_budget_check(&b,&p,&bytes)==UMI_STATUS_OK); CHECK(bytes>0U);
    return 0;
}
