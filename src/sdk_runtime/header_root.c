/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk_runtime/header_root.c
 *
 * PURPOSE:
 *   Implement describe public sdk header roots and aggregate include evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/sdk_runtime/header_root.h"
#include <string.h>
void umi_sdk_runtime_header_root_init(UmiSdkRuntimeHeaderRoot *value,const char *id)
{
    if(value==NULL)return;
    memset(value,0,sizeof(*value));
    value->structure_size=(uint32_t)sizeof(*value);
    value->state=UMI_SDK_RUNTIME_STATE_UNKNOWN;
    value->enabled=true;
    value->revision=1U;
    if(id!=NULL)(void)umi_sdk_runtime_copy_text(value->id,sizeof(value->id),id);
    }
UmiStatus umi_sdk_runtime_header_root_validate(const UmiSdkRuntimeHeaderRoot *value)
{
    if(value==NULL||value->structure_size!=sizeof(*value))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_sdk_runtime_text_is_valid(value->id,sizeof(value->id))||value->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_sdk_runtime_text_is_valid(value->path,sizeof(value->path))||!umi_sdk_runtime_text_is_valid(value->detail,sizeof(value->detail)))return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
    }
UmiStatus umi_sdk_runtime_header_root_set_path(UmiSdkRuntimeHeaderRoot *value,const char *path)
{
    UmiStatus s;
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_sdk_runtime_copy_text(value->path,sizeof(value->path),path);
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
UmiStatus umi_sdk_runtime_header_root_set_detail(UmiSdkRuntimeHeaderRoot *value,const char *detail)
{
    UmiStatus s;
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_sdk_runtime_copy_text(value->detail,sizeof(value->detail),detail);
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
UmiStatus umi_sdk_runtime_header_root_set_header_count(UmiSdkRuntimeHeaderRoot *value,uint64_t n)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->header_count=n;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
UmiStatus umi_sdk_runtime_header_root_set_generation(UmiSdkRuntimeHeaderRoot *value,uint64_t n)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->generation=n;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
UmiStatus umi_sdk_runtime_header_root_set_state(UmiSdkRuntimeHeaderRoot *value,UmiSdkRuntimeState state)
{
    if(value==NULL||state>UMI_SDK_RUNTIME_STATE_MISSING)return UMI_STATUS_INVALID_ARGUMENT;
    value->state=state;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
bool umi_sdk_runtime_header_root_same_identity(const UmiSdkRuntimeHeaderRoot *left,const UmiSdkRuntimeHeaderRoot *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
