/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk_runtime/deployment_probe.c
 *
 * PURPOSE:
 *   Implement probe an installed or extracted runtime before application launch.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/sdk_runtime/deployment_probe.h"
#include <string.h>
/*
 * Initialise sdk runtime deployment probe from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_deployment_probe_init(UmiSdkRuntimeDeploymentProbe *value,const char *id)
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
 * Check that sdk runtime deployment probe satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_deployment_probe_validate(const UmiSdkRuntimeDeploymentProbe *value)
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
 * Provide the sdk runtime deployment probe set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_deployment_probe_set_path(UmiSdkRuntimeDeploymentProbe *value,const char *path)
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
 * Provide the sdk runtime deployment probe set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_deployment_probe_set_detail(UmiSdkRuntimeDeploymentProbe *value,const char *detail)
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
 * Return the number of records represented by sdk runtime deployment probe set checked
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_deployment_probe_set_checked_count(UmiSdkRuntimeDeploymentProbe *value,uint64_t n)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->checked_count=n;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Return the number of records represented by sdk runtime deployment probe set failure
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_deployment_probe_set_failure_count(UmiSdkRuntimeDeploymentProbe *value,uint64_t n)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->failure_count=n;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the sdk runtime deployment probe set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_deployment_probe_set_state(UmiSdkRuntimeDeploymentProbe *value,UmiSdkRuntimeState state)
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
 * Provide the sdk runtime deployment probe same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_deployment_probe_same_identity(const UmiSdkRuntimeDeploymentProbe *left,const UmiSdkRuntimeDeploymentProbe *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
