/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/execution_snapshot.c
 *
 * PURPOSE:
 *   Implement capture immutable launch, runtime and result evidence for one test.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/execution_snapshot.h"
#include <string.h>
/*
 * Initialise test runtime execution snapshot from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_execution_snapshot_init(UmiTestRuntimeExecutionSnapshot *value,const char *id)
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
 * Check that test runtime execution snapshot satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_execution_snapshot_validate(const UmiTestRuntimeExecutionSnapshot *value)
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
 * Provide the test runtime execution snapshot set category operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_execution_snapshot_set_category(UmiTestRuntimeExecutionSnapshot *value,const char *category)
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
 * Provide the test runtime execution snapshot set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_execution_snapshot_set_detail(UmiTestRuntimeExecutionSnapshot *value,const char *detail)
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
 * Return the number of records represented by test runtime execution snapshot set evidence
 * without changing their state.
 */
UmiStatus umi_test_runtime_execution_snapshot_set_evidence_count(UmiTestRuntimeExecutionSnapshot *value,uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->evidence_count=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime execution snapshot set generation operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_execution_snapshot_set_generation(UmiTestRuntimeExecutionSnapshot *value,uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->generation=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime execution snapshot set active operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_execution_snapshot_set_active(UmiTestRuntimeExecutionSnapshot *value,bool active)
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
 * Provide the test runtime execution snapshot same identity operation used by this module
 * and its client applications.
 */
bool umi_test_runtime_execution_snapshot_same_identity(const UmiTestRuntimeExecutionSnapshot *left,const UmiTestRuntimeExecutionSnapshot *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
