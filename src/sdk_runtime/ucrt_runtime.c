/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk_runtime/ucrt_runtime.c
 *
 * PURPOSE:
 *   Implement describe windows ucrt runtime resolution and deployment evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/sdk_runtime/ucrt_runtime.h"
#include <string.h>
void umi_sdk_runtime_ucrt_runtime_init(UmiSdkRuntimeUcrtRuntime *value,const char *id)
{
    if(value==NULL)return;
    memset(value,0,sizeof(*value));
    value->structure_size=(uint32_t)sizeof(*value);
    value->state=UMI_SDK_RUNTIME_STATE_UNKNOWN;
    value->enabled=true;
    value->revision=1U;
    if(id!=NULL)(void)umi_sdk_runtime_copy_text(value->id,sizeof(value->id),id);
    }
UmiStatus umi_sdk_runtime_ucrt_runtime_validate(const UmiSdkRuntimeUcrtRuntime *value)
{
    if(value==NULL||value->structure_size!=sizeof(*value))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_sdk_runtime_text_is_valid(value->id,sizeof(value->id))||value->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_sdk_runtime_text_is_valid(value->path,sizeof(value->path))||!umi_sdk_runtime_text_is_valid(value->detail,sizeof(value->detail)))return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
    }
UmiStatus umi_sdk_runtime_ucrt_runtime_set_path(UmiSdkRuntimeUcrtRuntime *value,const char *path)
{
    UmiStatus s;
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_sdk_runtime_copy_text(value->path,sizeof(value->path),path);
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
UmiStatus umi_sdk_runtime_ucrt_runtime_set_detail(UmiSdkRuntimeUcrtRuntime *value,const char *detail)
{
    UmiStatus s;
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_sdk_runtime_copy_text(value->detail,sizeof(value->detail),detail);
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
UmiStatus umi_sdk_runtime_ucrt_runtime_set_dependency_count(UmiSdkRuntimeUcrtRuntime *value,uint64_t n)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->dependency_count=n;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
UmiStatus umi_sdk_runtime_ucrt_runtime_set_resolved_count(UmiSdkRuntimeUcrtRuntime *value,uint64_t n)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->resolved_count=n;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
UmiStatus umi_sdk_runtime_ucrt_runtime_set_state(UmiSdkRuntimeUcrtRuntime *value,UmiSdkRuntimeState state)
{
    if(value==NULL||state>UMI_SDK_RUNTIME_STATE_MISSING)return UMI_STATUS_INVALID_ARGUMENT;
    value->state=state;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
bool umi_sdk_runtime_ucrt_runtime_same_identity(const UmiSdkRuntimeUcrtRuntime *left,const UmiSdkRuntimeUcrtRuntime *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
