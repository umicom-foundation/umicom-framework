/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_adapter_external_missing.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: adapter external missing.
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
 * Exercise test adapter external missing and return a clear result when the behaviour no
 * longer matches its contract.
 */
void test_adapter_external_missing(void)
{
    UmiProductExecutionAdapter a=test_adapter(); UmiProductExecutionWorkItem item; UmiProductisationCompletionStep s=test_step(UMI_PRODUCTISATION_STAGE_REMAINING_APPLICATIONS,UMI_PRODUCTISATION_SEVERITY_LOW,UMI_PRODUCTISATION_OWNER_EXTERNAL_ADAPTER,UMI_PRODUCTISATION_GAP_EXTERNAL_ADAPTER,"x"); UmiProductExecutionOutcome o; char e[64]; assert(umi_product_execution_work_item_init(&item,0U,&s,1U)==UMI_STATUS_OK); assert(umi_product_execution_adapter_invoke(&a,&item,e,sizeof(e),&o)==UMI_STATUS_UNAVAILABLE);
}
