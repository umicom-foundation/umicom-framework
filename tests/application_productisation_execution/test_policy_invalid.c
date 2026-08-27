/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_policy_invalid.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: policy invalid.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

void test_policy_invalid(void)
{
    UmiProductExecutionPolicy p=umi_product_execution_policy_default(); p.maximum_attempts=0U; assert(umi_product_execution_policy_validate(&p)==UMI_STATUS_INVALID_ARGUMENT);
}
