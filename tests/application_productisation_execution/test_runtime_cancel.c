/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_runtime_cancel.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: runtime cancel.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

void test_runtime_cancel(void)
{
    UmiProductExecutionRuntime r; umi_product_execution_runtime_init(&r); umi_product_execution_runtime_cancel(&r); assert(umi_product_execution_cancellation_requested(&r.cancellation));
}
