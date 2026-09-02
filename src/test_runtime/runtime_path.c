/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/runtime_path.c
 *
 * PURPOSE:
 *   Implement retain one normalised runtime search path with provenance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/runtime_path.h"
#include <string.h>
/*
 * Initialise test runtime runtime path from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_runtime_path_init(UmiTestRuntimeRuntimePath *value,const char *id)
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
 * Check that test runtime runtime path satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_runtime_path_validate(const UmiTestRuntimeRuntimePath *value)
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
 * Provide the test runtime runtime path set category operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_runtime_path_set_category(UmiTestRuntimeRuntimePath *value,const char *category)
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
 * Provide the test runtime runtime path set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_runtime_path_set_detail(UmiTestRuntimeRuntimePath *value,const char *detail)
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
 * Provide the test runtime runtime path set priority operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_runtime_path_set_priority(UmiTestRuntimeRuntimePath *value,uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->priority=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime runtime path set source kind operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_runtime_path_set_source_kind(UmiTestRuntimeRuntimePath *value,uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->source_kind=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime runtime path set active operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_runtime_path_set_active(UmiTestRuntimeRuntimePath *value,bool active)
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
 * Provide the test runtime runtime path same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_runtime_path_same_identity(const UmiTestRuntimeRuntimePath *left,const UmiTestRuntimeRuntimePath *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
