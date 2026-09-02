/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_cancellation_reset.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: cancellation reset.
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
 * Release or reset state held by test cancellation so the same storage can be reused
 * safely.
 */
void test_cancellation_reset(void)
{
    UmiProductExecutionCancellation c; umi_product_execution_cancellation_init(&c); umi_product_execution_cancellation_request(&c); umi_product_execution_cancellation_reset(&c); assert(!umi_product_execution_cancellation_requested(&c));
}
