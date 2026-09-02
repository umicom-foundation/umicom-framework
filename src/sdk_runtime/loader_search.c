/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk_runtime/loader_search.c
 *
 * PURPOSE:
 *   Implement search runtime dependencies using explicit platform rules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/sdk_runtime/loader_search.h"
#include <string.h>
/*
 * Initialise sdk runtime loader search from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_loader_search_init(UmiSdkRuntimeLoaderSearch *value,const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return;
    memset(value,0,sizeof(*value));
    value->structure_size=(uint32_t)sizeof(*value);
    value->state=UMI_SDK_RUNTIME_STATE_UNKNOWN;
    value->enabled=true;
    value->revision=1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(id!=NULL)(void)umi_sdk_runtime_copy_text(value->id,sizeof(value->id),id);
    }
/*
 * Check that sdk runtime loader search satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_loader_search_validate(const UmiSdkRuntimeLoaderSearch *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL||value->structure_size!=sizeof(*value))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_sdk_runtime_text_is_valid(value->id,sizeof(value->id))||value->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_sdk_runtime_text_is_valid(value->path,sizeof(value->path))||!umi_sdk_runtime_text_is_valid(value->detail,sizeof(value->detail)))return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
    }
/*
 * Provide the sdk runtime loader search set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_loader_search_set_path(UmiSdkRuntimeLoaderSearch *value,const char *path)
{
    UmiStatus s;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_sdk_runtime_copy_text(value->path,sizeof(value->path),path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
/*
 * Provide the sdk runtime loader search set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_loader_search_set_detail(UmiSdkRuntimeLoaderSearch *value,const char *detail)
{
    UmiStatus s;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_sdk_runtime_copy_text(value->detail,sizeof(value->detail),detail);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
/*
 * Return the number of records represented by sdk runtime loader search set candidate
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_loader_search_set_candidate_count(UmiSdkRuntimeLoaderSearch *value,uint64_t n)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->candidate_count=n;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Return the number of records represented by sdk runtime loader search set resolved
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_loader_search_set_resolved_count(UmiSdkRuntimeLoaderSearch *value,uint64_t n)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->resolved_count=n;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the sdk runtime loader search set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_loader_search_set_state(UmiSdkRuntimeLoaderSearch *value,UmiSdkRuntimeState state)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL||state>UMI_SDK_RUNTIME_STATE_MISSING)return UMI_STATUS_INVALID_ARGUMENT;
    value->state=state;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the sdk runtime loader search same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_loader_search_same_identity(const UmiSdkRuntimeLoaderSearch *left,const UmiSdkRuntimeLoaderSearch *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
