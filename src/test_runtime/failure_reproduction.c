/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/failure_reproduction.c
 *
 * PURPOSE:
 *   Implement retain the exact runtime context required to reproduce a failure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/failure_reproduction.h"
#include <string.h>
/*
 * Initialise test runtime failure reproduction from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_failure_reproduction_init(UmiTestRuntimeFailureReproduction *value,const char *id)
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
 * Check that test runtime failure reproduction satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_test_runtime_failure_reproduction_validate(const UmiTestRuntimeFailureReproduction *value)
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
 * Provide the test runtime failure reproduction set detail operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_failure_reproduction_set_detail(UmiTestRuntimeFailureReproduction *value,const char *detail)
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
 * Return the number of records represented by test runtime failure reproduction set
 * evidence without changing their state.
 */
UmiStatus umi_test_runtime_failure_reproduction_set_evidence_count(UmiTestRuntimeFailureReproduction *value,uint64_t number)
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
 * Provide the test runtime failure reproduction set generation operation used by this
 * module and its client applications.
 */
UmiStatus umi_test_runtime_failure_reproduction_set_generation(UmiTestRuntimeFailureReproduction *value,uint64_t number)
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
 * Provide the test runtime failure reproduction same identity operation used by this
 * module and its client applications.
 */
bool umi_test_runtime_failure_reproduction_same_identity(const UmiTestRuntimeFailureReproduction *left,const UmiTestRuntimeFailureReproduction *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
