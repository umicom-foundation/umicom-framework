/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_adapter_validate.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: adapter validate.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

void test_adapter_validate(void)
{
    UmiProductExecutionAdapter a=test_adapter(); assert(umi_product_execution_adapter_validate(&a)==UMI_STATUS_OK);
}
