/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/stack_evidence.c
 *
 * PURPOSE:
 *   Implement retain stack exhaustion and recursion depth evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/stack_evidence.h"
#include <string.h>
/*
 * Initialise test runtime stack evidence from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_stack_evidence_init(UmiTestRuntimeStackEvidence *value,const char *id)
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
 * Check that test runtime stack evidence satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_stack_evidence_validate(const UmiTestRuntimeStackEvidence *value)
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
 * Provide the test runtime stack evidence set category operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_stack_evidence_set_category(UmiTestRuntimeStackEvidence *value,const char *category)
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
 * Provide the test runtime stack evidence set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_stack_evidence_set_detail(UmiTestRuntimeStackEvidence *value,const char *detail)
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
 * Provide the test runtime stack evidence set estimated bytes operation used by this
 * module and its client applications.
 */
UmiStatus umi_test_runtime_stack_evidence_set_estimated_bytes(UmiTestRuntimeStackEvidence *value,uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->estimated_bytes=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime stack evidence set depth operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_stack_evidence_set_depth(UmiTestRuntimeStackEvidence *value,uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->depth=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime stack evidence set active operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_stack_evidence_set_active(UmiTestRuntimeStackEvidence *value,bool active)
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
 * Provide the test runtime stack evidence same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_stack_evidence_same_identity(const UmiTestRuntimeStackEvidence *left,const UmiTestRuntimeStackEvidence *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
