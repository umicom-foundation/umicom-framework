/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_state_transition_valid.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: state transition valid.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

void test_state_transition_valid(void)
{
    assert(umi_product_execution_state_can_transition(UMI_PRODUCT_EXECUTION_PENDING, UMI_PRODUCT_EXECUTION_READY));
}
