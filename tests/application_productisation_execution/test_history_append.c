/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_history_append.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: history append.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

/* Add test history only after its inputs and available capacity have been checked. */
void test_history_append(void)
{
    UmiProductExecutionHistory h; umi_product_execution_history_init(&h); assert(umi_product_execution_history_append(&h,1U,UMI_PRODUCT_EXECUTION_EVENT_STARTED,UMI_STATUS_OK,"x")==UMI_STATUS_OK); assert(h.count==1U);
}
