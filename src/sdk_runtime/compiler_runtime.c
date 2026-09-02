/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk_runtime/compiler_runtime.c
 *
 * PURPOSE:
 *   Implement describe compiler runtime libraries required by installed binaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/sdk_runtime/compiler_runtime.h"
#include <string.h>
/*
 * Initialise sdk runtime compiler runtime from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_compiler_runtime_init(UmiSdkRuntimeCompilerRuntime *value,const char *id)
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
 * Check that sdk runtime compiler runtime satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_compiler_runtime_validate(const UmiSdkRuntimeCompilerRuntime *value)
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
 * Provide the sdk runtime compiler runtime set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_compiler_runtime_set_path(UmiSdkRuntimeCompilerRuntime *value,const char *path)
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
 * Provide the sdk runtime compiler runtime set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_compiler_runtime_set_detail(UmiSdkRuntimeCompilerRuntime *value,const char *detail)
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
 * Return the number of records represented by sdk runtime compiler runtime set dependency
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_compiler_runtime_set_dependency_count(UmiSdkRuntimeCompilerRuntime *value,uint64_t n)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->dependency_count=n;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Return the number of records represented by sdk runtime compiler runtime set missing
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_compiler_runtime_set_missing_count(UmiSdkRuntimeCompilerRuntime *value,uint64_t n)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->missing_count=n;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the sdk runtime compiler runtime set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_compiler_runtime_set_state(UmiSdkRuntimeCompilerRuntime *value,UmiSdkRuntimeState state)
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
 * Provide the sdk runtime compiler runtime same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_compiler_runtime_same_identity(const UmiSdkRuntimeCompilerRuntime *left,const UmiSdkRuntimeCompilerRuntime *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
