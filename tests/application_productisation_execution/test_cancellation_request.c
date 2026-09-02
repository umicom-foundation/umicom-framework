/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_cancellation_request.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: cancellation request.
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
 * Exercise test cancellation request and return a clear result when the behaviour no
 * longer matches its contract.
 */
void test_cancellation_request(void)
{
    UmiProductExecutionCancellation c; umi_product_execution_cancellation_init(&c); umi_product_execution_cancellation_request(&c); assert(umi_product_execution_cancellation_requested(&c));
}
