/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/evidence_retention.c
 *
 * PURPOSE:
 *   Implement define how long regression evidence remains queryable.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/evidence_retention.h"
#include <string.h>
/*
 * Initialise test runtime evidence retention from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_evidence_retention_init(UmiTestRuntimeEvidenceRetention *value,const char *id)
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
 * Check that test runtime evidence retention satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_evidence_retention_validate(const UmiTestRuntimeEvidenceRetention *value)
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
 * Provide the test runtime evidence retention set category operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_evidence_retention_set_category(UmiTestRuntimeEvidenceRetention *value,const char *category)
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
 * Provide the test runtime evidence retention set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_evidence_retention_set_detail(UmiTestRuntimeEvidenceRetention *value,const char *detail)
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
 * Provide the test runtime evidence retention set retention days operation used by this
 * module and its client applications.
 */
UmiStatus umi_test_runtime_evidence_retention_set_retention_days(UmiTestRuntimeEvidenceRetention *value,uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->retention_days=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime evidence retention set maximum runs operation used by this
 * module and its client applications.
 */
UmiStatus umi_test_runtime_evidence_retention_set_maximum_runs(UmiTestRuntimeEvidenceRetention *value,uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->maximum_runs=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime evidence retention set active operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_evidence_retention_set_active(UmiTestRuntimeEvidenceRetention *value,bool active)
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
 * Provide the test runtime evidence retention same identity operation used by this module
 * and its client applications.
 */
bool umi_test_runtime_evidence_retention_same_identity(const UmiTestRuntimeEvidenceRetention *left,const UmiTestRuntimeEvidenceRetention *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
