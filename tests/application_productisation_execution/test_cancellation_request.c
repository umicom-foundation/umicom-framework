/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_cancellation_request.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: cancellation request.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

void test_cancellation_request(void)
{
    UmiProductExecutionCancellation c; umi_product_execution_cancellation_init(&c); umi_product_execution_cancellation_request(&c); assert(umi_product_execution_cancellation_requested(&c));
}
