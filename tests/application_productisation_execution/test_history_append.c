/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_history_append.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: history append.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

void test_history_append(void)
{
    UmiProductExecutionHistory h; umi_product_execution_history_init(&h); assert(umi_product_execution_history_append(&h,1U,UMI_PRODUCT_EXECUTION_EVENT_STARTED,UMI_STATUS_OK,"x")==UMI_STATUS_OK); assert(h.count==1U);
}
