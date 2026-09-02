/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/runtime_capability.c
 *
 * PURPOSE:
 *   Implement describe one capability required before a test can be launched.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/runtime_capability.h"
#include <string.h>
/*
 * Initialise test runtime runtime capability from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_runtime_capability_init(UmiTestRuntimeRuntimeCapability *value,const char *id)
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
 * Check that test runtime runtime capability satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_runtime_capability_validate(const UmiTestRuntimeRuntimeCapability *value)
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
 * Provide the test runtime runtime capability set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_runtime_capability_set_detail(UmiTestRuntimeRuntimeCapability *value,const char *detail)
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
 * Provide the test runtime runtime capability set required operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_runtime_capability_set_required(UmiTestRuntimeRuntimeCapability *value,uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->required=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime runtime capability set available operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_runtime_capability_set_available(UmiTestRuntimeRuntimeCapability *value,uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->available=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime runtime capability same identity operation used by this module
 * and its client applications.
 */
bool umi_test_runtime_runtime_capability_same_identity(const UmiTestRuntimeRuntimeCapability *left,const UmiTestRuntimeRuntimeCapability *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
