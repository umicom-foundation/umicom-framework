/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_work_item_retry.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: work item retry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

void test_work_item_retry(void)
{
    UmiProductExecutionWorkItem item; UmiProductisationCompletionStep s=test_step(UMI_PRODUCTISATION_STAGE_FRAMEWORK,UMI_PRODUCTISATION_SEVERITY_HIGH,UMI_PRODUCTISATION_OWNER_FRAMEWORK,UMI_PRODUCTISATION_GAP_MISSING_COMPONENT,"x"); assert(umi_product_execution_work_item_init(&item,0U,&s,2U)==UMI_STATUS_OK); item.outcome=UMI_PRODUCT_EXECUTION_OUTCOME_RETRYABLE_FAILURE; item.attempts=1U; assert(umi_product_execution_work_item_retryable(&item));
}
