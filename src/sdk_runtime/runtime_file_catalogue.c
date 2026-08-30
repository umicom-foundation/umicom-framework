/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk_runtime/runtime_file_catalogue.c
 *
 * PURPOSE:
 *   Implement maintain the runtime-file inventory for clean-machine validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/sdk_runtime/runtime_file_catalogue.h"
#include <string.h>
void umi_sdk_runtime_runtime_file_catalogue_init(UmiSdkRuntimeRuntimeFileCatalogue *value,const char *id)
{
    if(value==NULL)return;
    memset(value,0,sizeof(*value));
    value->structure_size=(uint32_t)sizeof(*value);
    value->state=UMI_SDK_RUNTIME_STATE_UNKNOWN;
    value->enabled=true;
    value->revision=1U;
    if(id!=NULL)(void)umi_sdk_runtime_copy_text(value->id,sizeof(value->id),id);
    }
UmiStatus umi_sdk_runtime_runtime_file_catalogue_validate(const UmiSdkRuntimeRuntimeFileCatalogue *value)
{
    if(value==NULL||value->structure_size!=sizeof(*value))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_sdk_runtime_text_is_valid(value->id,sizeof(value->id))||value->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_sdk_runtime_text_is_valid(value->path,sizeof(value->path))||!umi_sdk_runtime_text_is_valid(value->detail,sizeof(value->detail)))return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
    }
UmiStatus umi_sdk_runtime_runtime_file_catalogue_set_path(UmiSdkRuntimeRuntimeFileCatalogue *value,const char *path)
{
    UmiStatus s;
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_sdk_runtime_copy_text(value->path,sizeof(value->path),path);
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
UmiStatus umi_sdk_runtime_runtime_file_catalogue_set_detail(UmiSdkRuntimeRuntimeFileCatalogue *value,const char *detail)
{
    UmiStatus s;
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_sdk_runtime_copy_text(value->detail,sizeof(value->detail),detail);
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
UmiStatus umi_sdk_runtime_runtime_file_catalogue_set_file_count(UmiSdkRuntimeRuntimeFileCatalogue *value,uint64_t n)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->file_count=n;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
UmiStatus umi_sdk_runtime_runtime_file_catalogue_set_generation(UmiSdkRuntimeRuntimeFileCatalogue *value,uint64_t n)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->generation=n;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
UmiStatus umi_sdk_runtime_runtime_file_catalogue_set_state(UmiSdkRuntimeRuntimeFileCatalogue *value,UmiSdkRuntimeState state)
{
    if(value==NULL||state>UMI_SDK_RUNTIME_STATE_MISSING)return UMI_STATUS_INVALID_ARGUMENT;
    value->state=state;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
bool umi_sdk_runtime_runtime_file_catalogue_same_identity(const UmiSdkRuntimeRuntimeFileCatalogue *left,const UmiSdkRuntimeRuntimeFileCatalogue *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
