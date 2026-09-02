/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk_runtime/package_channel.c
 *
 * PURPOSE:
 *   Implement describe stable, preview and development runtime channels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/sdk_runtime/package_channel.h"
#include <string.h>
/*
 * Initialise sdk runtime package channel from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_package_channel_init(UmiSdkRuntimePackageChannel *value,const char *id)
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
 * Check that sdk runtime package channel satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_package_channel_validate(const UmiSdkRuntimePackageChannel *value)
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
 * Provide the sdk runtime package channel set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_package_channel_set_path(UmiSdkRuntimePackageChannel *value,const char *path)
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
 * Provide the sdk runtime package channel set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_package_channel_set_detail(UmiSdkRuntimePackageChannel *value,const char *detail)
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
 * Provide the sdk runtime package channel set priority operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_package_channel_set_priority(UmiSdkRuntimePackageChannel *value,uint64_t n)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->priority=n;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the sdk runtime package channel set generation operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_package_channel_set_generation(UmiSdkRuntimePackageChannel *value,uint64_t n)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->generation=n;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the sdk runtime package channel set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_package_channel_set_state(UmiSdkRuntimePackageChannel *value,UmiSdkRuntimeState state)
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
 * Provide the sdk runtime package channel same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_package_channel_same_identity(const UmiSdkRuntimePackageChannel *left,const UmiSdkRuntimePackageChannel *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
