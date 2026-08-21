/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk_runtime/rollback_plan.c
 *
 * PURPOSE:
 *   Implement plan runtime rollback to a known compatible package generation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/sdk_runtime/rollback_plan.h"
#include <string.h>
void umi_sdk_runtime_rollback_plan_init(UmiSdkRuntimeRollbackPlan *value,const char *id)
{
    if(value==NULL)return;
    memset(value,0,sizeof(*value));
    value->structure_size=(uint32_t)sizeof(*value);
    value->state=UMI_SDK_RUNTIME_STATE_UNKNOWN;
    value->enabled=true;
    value->revision=1U;
    if(id!=NULL)(void)umi_sdk_runtime_copy_text(value->id,sizeof(value->id),id);
    }
UmiStatus umi_sdk_runtime_rollback_plan_validate(const UmiSdkRuntimeRollbackPlan *value)
{
    if(value==NULL||value->structure_size!=sizeof(*value))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_sdk_runtime_text_is_valid(value->id,sizeof(value->id))||value->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_sdk_runtime_text_is_valid(value->path,sizeof(value->path))||!umi_sdk_runtime_text_is_valid(value->detail,sizeof(value->detail)))return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
    }
UmiStatus umi_sdk_runtime_rollback_plan_set_path(UmiSdkRuntimeRollbackPlan *value,const char *path)
{
    UmiStatus s;
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_sdk_runtime_copy_text(value->path,sizeof(value->path),path);
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
UmiStatus umi_sdk_runtime_rollback_plan_set_detail(UmiSdkRuntimeRollbackPlan *value,const char *detail)
{
    UmiStatus s;
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_sdk_runtime_copy_text(value->detail,sizeof(value->detail),detail);
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
UmiStatus umi_sdk_runtime_rollback_plan_set_action_count(UmiSdkRuntimeRollbackPlan *value,uint64_t n)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->action_count=n;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
UmiStatus umi_sdk_runtime_rollback_plan_set_generation(UmiSdkRuntimeRollbackPlan *value,uint64_t n)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->generation=n;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
UmiStatus umi_sdk_runtime_rollback_plan_set_state(UmiSdkRuntimeRollbackPlan *value,UmiSdkRuntimeState state)
{
    if(value==NULL||state>UMI_SDK_RUNTIME_STATE_MISSING)return UMI_STATUS_INVALID_ARGUMENT;
    value->state=state;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
bool umi_sdk_runtime_rollback_plan_same_identity(const UmiSdkRuntimeRollbackPlan *left,const UmiSdkRuntimeRollbackPlan *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
