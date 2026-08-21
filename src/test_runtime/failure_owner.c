/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/failure_owner.c
 *
 * PURPOSE:
 *   Implement assign failing subsystems to durable component identities.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/failure_owner.h"
#include <string.h>
void umi_test_runtime_failure_owner_init(UmiTestRuntimeFailureOwner *value,const char *id)
{
    if(value==NULL)return;
    memset(value,0,sizeof(*value));
    value->structure_size=(uint32_t)sizeof(*value);
    value->revision=1U;
    value->enabled=true;
    if(id!=NULL)(void)umi_test_runtime_copy_text(value->id,sizeof(value->id),id);
    }
UmiStatus umi_test_runtime_failure_owner_validate(const UmiTestRuntimeFailureOwner *value)
{
    if(value==NULL||value->structure_size!=sizeof(*value))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_test_runtime_text_is_valid(value->id,sizeof(value->id))||value->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_test_runtime_text_is_valid(value->detail,sizeof(value->detail)))return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
    }
UmiStatus umi_test_runtime_failure_owner_set_detail(UmiTestRuntimeFailureOwner *value,const char *detail)
{
    UmiStatus s;
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_test_runtime_copy_text(value->detail,sizeof(value->detail),detail);
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
UmiStatus umi_test_runtime_failure_owner_set_priority(UmiTestRuntimeFailureOwner *value,uint64_t number)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->priority=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
UmiStatus umi_test_runtime_failure_owner_set_generation(UmiTestRuntimeFailureOwner *value,uint64_t number)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->generation=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
bool umi_test_runtime_failure_owner_same_identity(const UmiTestRuntimeFailureOwner *left,const UmiTestRuntimeFailureOwner *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
