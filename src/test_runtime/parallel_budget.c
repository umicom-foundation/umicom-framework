/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/parallel_budget.c
 *
 * PURPOSE:
 *   Implement define safe parallelism for constrained windows and ci environments.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/parallel_budget.h"
#include <string.h>
/*
 * Initialise test runtime parallel budget from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_parallel_budget_init(UmiTestRuntimeParallelBudget *value,const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return;
    memset(value,0,sizeof(*value));
    value->structure_size=(uint32_t)sizeof(*value);
    value->active=true;
    value->revision=1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(id!=NULL)(void)umi_test_runtime_copy_text(value->id,sizeof(value->id),id);
    }
/*
 * Check that test runtime parallel budget satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_parallel_budget_validate(const UmiTestRuntimeParallelBudget *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL||value->structure_size!=sizeof(*value))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_test_runtime_text_is_valid(value->id,sizeof(value->id))||value->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_test_runtime_text_is_valid(value->category,sizeof(value->category))||!umi_test_runtime_text_is_valid(value->detail,sizeof(value->detail)))return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime parallel budget set category operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_parallel_budget_set_category(UmiTestRuntimeParallelBudget *value,const char *category)
{
    UmiStatus s;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_test_runtime_copy_text(value->category,sizeof(value->category),category);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
/*
 * Provide the test runtime parallel budget set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_parallel_budget_set_detail(UmiTestRuntimeParallelBudget *value,const char *detail)
{
    UmiStatus s;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_test_runtime_copy_text(value->detail,sizeof(value->detail),detail);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
/*
 * Provide the test runtime parallel budget set maximum parallel operation used by this
 * module and its client applications.
 */
UmiStatus umi_test_runtime_parallel_budget_set_maximum_parallel(UmiTestRuntimeParallelBudget *value,uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->maximum_parallel=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime parallel budget set processor cost operation used by this
 * module and its client applications.
 */
UmiStatus umi_test_runtime_parallel_budget_set_processor_cost(UmiTestRuntimeParallelBudget *value,uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->processor_cost=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime parallel budget set active operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_parallel_budget_set_active(UmiTestRuntimeParallelBudget *value,bool active)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->active=active;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime parallel budget same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_parallel_budget_same_identity(const UmiTestRuntimeParallelBudget *left,const UmiTestRuntimeParallelBudget *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
