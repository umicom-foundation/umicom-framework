/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_scheduler_priority.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: scheduler priority.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

void test_scheduler_priority(void)
{
    UmiProductExecutionWorkQueue q; UmiProductisationCompletionPlan p; size_t i; memset(&p,0,sizeof(p)); p.steps[0]=test_step(UMI_PRODUCTISATION_STAGE_FRAMEWORK,UMI_PRODUCTISATION_SEVERITY_LOW,UMI_PRODUCTISATION_OWNER_FRAMEWORK,UMI_PRODUCTISATION_GAP_MISSING_COMPONENT,"a"); p.steps[1]=test_step(UMI_PRODUCTISATION_STAGE_FRAMEWORK,UMI_PRODUCTISATION_SEVERITY_BLOCKER,UMI_PRODUCTISATION_OWNER_FRAMEWORK,UMI_PRODUCTISATION_GAP_MISSING_COMPONENT,"b"); p.step_count=2U; assert(umi_product_execution_work_queue_from_plan(&q,&p,3U)==UMI_STATUS_OK); q.items[0].state=UMI_PRODUCT_EXECUTION_READY; q.items[1].state=UMI_PRODUCT_EXECUTION_READY; assert(umi_product_execution_scheduler_next(&q,&i)==UMI_STATUS_OK); assert(i==1U);
}
