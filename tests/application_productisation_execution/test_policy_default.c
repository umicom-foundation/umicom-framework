/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_policy_default.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: policy default.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

void test_policy_default(void)
{
    UmiProductExecutionPolicy p=umi_product_execution_policy_default(); assert(p.maximum_attempts==3U); assert(p.require_framework_completion_first);
}
