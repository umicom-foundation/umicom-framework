/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test/workbench/test_retry_policy.c
 *
 * PURPOSE:
 *   Implement test retry policy state transitions and validation.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test/workbench/test_retry_policy.h"
#include <string.h>
/*
 * Initialise test retry policy from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_test_retry_policy_init(UmiTestRetryPolicy *model,const char *id,const char *label){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(model,0,sizeof *model);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_test_workbench_entry_init(&model->value,id,label)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;model->generation=1U;return UMI_STATUS_OK;}
/*
 * Exercise test retry policy set active and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_retry_policy_set_active(UmiTestRetryPolicy *model,bool active){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;model->active=active;model->generation++;model->value.revision++;return UMI_STATUS_OK;}
/*
 * Return the number of records represented by test retry policy set without changing their
 * state.
 */
UmiStatus umi_test_retry_policy_set_count(UmiTestRetryPolicy *model,uint32_t item_count){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(model==NULL||item_count>UMI_TEST_WORKBENCH_MAX_ITEMS)return UMI_STATUS_INVALID_ARGUMENT;model->item_count=item_count;model->generation++;model->value.revision++;return UMI_STATUS_OK;}
/*
 * Exercise test retry policy set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_retry_policy_set_state(UmiTestRetryPolicy *model,UmiTestWorkbenchState state){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_test_workbench_entry_set_state(&model->value,state);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)model->generation++;return s;}
/* Check that test retry policy satisfies its contract before another service relies on it. */
int umi_test_retry_policy_valid(const UmiTestRetryPolicy *model){return model!=NULL&&umi_test_workbench_entry_valid(&model->value)&&model->item_count<=UMI_TEST_WORKBENCH_MAX_ITEMS&&model->generation>0U;}
