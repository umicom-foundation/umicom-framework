/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/run_summary.c
 *
 * PURPOSE:
 *   Implement summarise one run without losing native failure categories.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/run_summary.h"
#include <string.h>
/*
 * Initialise test runtime run summary from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_run_summary_init(UmiTestRuntimeRunSummary *value,const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return;
    memset(value,0,sizeof(*value));
    value->structure_size=(uint32_t)sizeof(*value);
    value->revision=1U;
    value->enabled=true;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(id!=NULL)(void)umi_test_runtime_copy_text(value->id,sizeof(value->id),id);
    }
/*
 * Check that test runtime run summary satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_test_runtime_run_summary_validate(const UmiTestRuntimeRunSummary *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL||value->structure_size!=sizeof(*value))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_test_runtime_text_is_valid(value->id,sizeof(value->id))||value->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_test_runtime_text_is_valid(value->detail,sizeof(value->detail)))return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime run summary set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_run_summary_set_detail(UmiTestRuntimeRunSummary *value,const char *detail)
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
 * Return the number of records represented by test runtime run summary set passed without
 * changing their state.
 */
UmiStatus umi_test_runtime_run_summary_set_passed_count(UmiTestRuntimeRunSummary *value,uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->passed_count=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Return the number of records represented by test runtime run summary set failed without
 * changing their state.
 */
UmiStatus umi_test_runtime_run_summary_set_failed_count(UmiTestRuntimeRunSummary *value,uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->failed_count=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime run summary same identity operation used by this module and its
 * client applications.
 */
bool umi_test_runtime_run_summary_same_identity(const UmiTestRuntimeRunSummary *left,const UmiTestRuntimeRunSummary *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
