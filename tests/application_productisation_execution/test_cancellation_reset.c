/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_cancellation_reset.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: cancellation reset.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

void test_cancellation_reset(void)
{
    UmiProductExecutionCancellation c; umi_product_execution_cancellation_init(&c); umi_product_execution_cancellation_request(&c); umi_product_execution_cancellation_reset(&c); assert(!umi_product_execution_cancellation_requested(&c));
}
