/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/exit_interpretation.c
 *
 * PURPOSE:
 *   Implement interpret normal exits separately from native crash status.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/exit_interpretation.h"
#include <string.h>
/*
 * Initialise test runtime exit interpretation from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_exit_interpretation_init(UmiTestRuntimeExitInterpretation *value,const char *id)
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
 * Check that test runtime exit interpretation satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_test_runtime_exit_interpretation_validate(const UmiTestRuntimeExitInterpretation *value)
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
 * Provide the test runtime exit interpretation set category operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_exit_interpretation_set_category(UmiTestRuntimeExitInterpretation *value,const char *category)
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
 * Provide the test runtime exit interpretation set detail operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_exit_interpretation_set_detail(UmiTestRuntimeExitInterpretation *value,const char *detail)
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
 * Provide the test runtime exit interpretation set exit code operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_exit_interpretation_set_exit_code(UmiTestRuntimeExitInterpretation *value,uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->exit_code=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime exit interpretation set native status operation used by this
 * module and its client applications.
 */
UmiStatus umi_test_runtime_exit_interpretation_set_native_status(UmiTestRuntimeExitInterpretation *value,uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->native_status=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime exit interpretation set active operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_exit_interpretation_set_active(UmiTestRuntimeExitInterpretation *value,bool active)
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
 * Provide the test runtime exit interpretation same identity operation used by this module
 * and its client applications.
 */
bool umi_test_runtime_exit_interpretation_same_identity(const UmiTestRuntimeExitInterpretation *left,const UmiTestRuntimeExitInterpretation *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
