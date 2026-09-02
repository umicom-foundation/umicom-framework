/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_policy_invalid.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: policy invalid.
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
 * Exercise test policy invalid and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_policy_invalid(void)
{
    UmiProductExecutionPolicy p=umi_product_execution_policy_default(); p.maximum_attempts=0U; assert(umi_product_execution_policy_validate(&p)==UMI_STATUS_INVALID_ARGUMENT);
}
