/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_history_sequence.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: history sequence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

/*
 * Exercise test history sequence and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_history_sequence(void)
{
    UmiProductExecutionHistory h; umi_product_execution_history_init(&h); assert(umi_product_execution_history_append(&h,0U,UMI_PRODUCT_EXECUTION_EVENT_READY,UMI_STATUS_OK,"a")==UMI_STATUS_OK); assert(umi_product_execution_history_append(&h,0U,UMI_PRODUCT_EXECUTION_EVENT_STARTED,UMI_STATUS_OK,"b")==UMI_STATUS_OK); assert(h.events[1].sequence==2U);
}
