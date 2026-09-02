/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/sanitizer_evidence.c
 *
 * PURPOSE:
 *   Implement retain sanitizer type, finding count and first failing location.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/sanitizer_evidence.h"
#include <string.h>
/*
 * Initialise test runtime sanitizer evidence from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_sanitizer_evidence_init(UmiTestRuntimeSanitizerEvidence *value,const char *id)
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
 * Check that test runtime sanitizer evidence satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_sanitizer_evidence_validate(const UmiTestRuntimeSanitizerEvidence *value)
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
 * Provide the test runtime sanitizer evidence set category operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_sanitizer_evidence_set_category(UmiTestRuntimeSanitizerEvidence *value,const char *category)
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
 * Provide the test runtime sanitizer evidence set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_sanitizer_evidence_set_detail(UmiTestRuntimeSanitizerEvidence *value,const char *detail)
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
 * Return the number of records represented by test runtime sanitizer evidence set finding
 * without changing their state.
 */
UmiStatus umi_test_runtime_sanitizer_evidence_set_finding_count(UmiTestRuntimeSanitizerEvidence *value,uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->finding_count=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Return the number of records represented by test runtime sanitizer evidence set
 * suppressed without changing their state.
 */
UmiStatus umi_test_runtime_sanitizer_evidence_set_suppressed_count(UmiTestRuntimeSanitizerEvidence *value,uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->suppressed_count=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime sanitizer evidence set active operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_sanitizer_evidence_set_active(UmiTestRuntimeSanitizerEvidence *value,bool active)
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
 * Provide the test runtime sanitizer evidence same identity operation used by this module
 * and its client applications.
 */
bool umi_test_runtime_sanitizer_evidence_same_identity(const UmiTestRuntimeSanitizerEvidence *left,const UmiTestRuntimeSanitizerEvidence *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
