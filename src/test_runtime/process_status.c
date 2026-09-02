/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/process_status.c
 *
 * PURPOSE:
 *   Implement retain native process status independently from test assertion outcome.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/process_status.h"
#include <string.h>
/*
 * Initialise test runtime process status from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_process_status_init(UmiTestRuntimeProcessStatus *value,const char *id)
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
 * Check that test runtime process status satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_process_status_validate(const UmiTestRuntimeProcessStatus *value)
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
 * Provide the test runtime process status set category operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_process_status_set_category(UmiTestRuntimeProcessStatus *value,const char *category)
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
 * Provide the test runtime process status set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_process_status_set_detail(UmiTestRuntimeProcessStatus *value,const char *detail)
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
 * Provide the test runtime process status set native status operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_process_status_set_native_status(UmiTestRuntimeProcessStatus *value,uint64_t number)
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
 * Provide the test runtime process status set exit code operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_process_status_set_exit_code(UmiTestRuntimeProcessStatus *value,uint64_t number)
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
 * Provide the test runtime process status set active operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_process_status_set_active(UmiTestRuntimeProcessStatus *value,bool active)
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
 * Provide the test runtime process status same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_process_status_same_identity(const UmiTestRuntimeProcessStatus *left,const UmiTestRuntimeProcessStatus *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
