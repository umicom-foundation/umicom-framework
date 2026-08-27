/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_queue_bounds.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: queue bounds.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

void test_queue_bounds(void)
{
    UmiProductExecutionWorkQueue q; umi_product_execution_work_queue_init(&q); assert(umi_product_execution_work_queue_at(&q,0U)==NULL);
}
