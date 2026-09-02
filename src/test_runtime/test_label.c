/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/test_label.c
 *
 * PURPOSE:
 *   Implement describe one stable label used by execution profiles and filtering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/test_label.h"
#include <string.h>
/*
 * Initialise test runtime test label from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_test_label_init(UmiTestRuntimeTestLabel *value,const char *id)
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
 * Check that test runtime test label satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_test_runtime_test_label_validate(const UmiTestRuntimeTestLabel *value)
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
 * Provide the test runtime test label set category operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_test_label_set_category(UmiTestRuntimeTestLabel *value,const char *category)
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
 * Provide the test runtime test label set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_test_label_set_detail(UmiTestRuntimeTestLabel *value,const char *detail)
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
 * Return the number of records represented by test runtime test label set member without
 * changing their state.
 */
UmiStatus umi_test_runtime_test_label_set_member_count(UmiTestRuntimeTestLabel *value,uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->member_count=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime test label set generation operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_test_label_set_generation(UmiTestRuntimeTestLabel *value,uint64_t number)
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
 * Provide the test runtime test label set active operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_test_label_set_active(UmiTestRuntimeTestLabel *value,bool active)
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
 * Provide the test runtime test label same identity operation used by this module and its
 * client applications.
 */
bool umi_test_runtime_test_label_same_identity(const UmiTestRuntimeTestLabel *left,const UmiTestRuntimeTestLabel *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
