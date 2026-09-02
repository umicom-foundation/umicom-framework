/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/ctest_record.c
 *
 * PURPOSE:
 *   Implement represent one ctest registration independently of generated ctest files.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/ctest_record.h"
#include <string.h>
/*
 * Initialise test runtime ctest record from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_ctest_record_init(UmiTestRuntimeCtestRecord *value,const char *id)
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
 * Check that test runtime ctest record satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_ctest_record_validate(const UmiTestRuntimeCtestRecord *value)
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
 * Provide the test runtime ctest record set category operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_ctest_record_set_category(UmiTestRuntimeCtestRecord *value,const char *category)
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
 * Provide the test runtime ctest record set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_ctest_record_set_detail(UmiTestRuntimeCtestRecord *value,const char *detail)
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
 * Return the number of records represented by test runtime ctest record set processor
 * without changing their state.
 */
UmiStatus umi_test_runtime_ctest_record_set_processor_count(UmiTestRuntimeCtestRecord *value,uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->processor_count=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime ctest record set timeout seconds operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_ctest_record_set_timeout_seconds(UmiTestRuntimeCtestRecord *value,uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->timeout_seconds=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime ctest record set active operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_ctest_record_set_active(UmiTestRuntimeCtestRecord *value,bool active)
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
 * Provide the test runtime ctest record same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_ctest_record_same_identity(const UmiTestRuntimeCtestRecord *left,const UmiTestRuntimeCtestRecord *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
