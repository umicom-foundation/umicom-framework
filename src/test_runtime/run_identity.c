/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/run_identity.c
 *
 * PURPOSE:
 *   Implement provide stable identity for one complete regression execution.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/run_identity.h"
#include <string.h>
void umi_test_runtime_run_identity_init(UmiTestRuntimeRunIdentity *value,const char *id)
{
    if(value==NULL)return;
    memset(value,0,sizeof(*value));
    value->structure_size=(uint32_t)sizeof(*value);
    value->revision=1U;
    value->enabled=true;
    if(id!=NULL)(void)umi_test_runtime_copy_text(value->id,sizeof(value->id),id);
    }
UmiStatus umi_test_runtime_run_identity_validate(const UmiTestRuntimeRunIdentity *value)
{
    if(value==NULL||value->structure_size!=sizeof(*value))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_test_runtime_text_is_valid(value->id,sizeof(value->id))||value->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_test_runtime_text_is_valid(value->detail,sizeof(value->detail)))return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
    }
UmiStatus umi_test_runtime_run_identity_set_detail(UmiTestRuntimeRunIdentity *value,const char *detail)
{
    UmiStatus s;
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_test_runtime_copy_text(value->detail,sizeof(value->detail),detail);
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
UmiStatus umi_test_runtime_run_identity_set_sequence(UmiTestRuntimeRunIdentity *value,uint64_t number)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->sequence=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
UmiStatus umi_test_runtime_run_identity_set_generation(UmiTestRuntimeRunIdentity *value,uint64_t number)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->generation=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
bool umi_test_runtime_run_identity_same_identity(const UmiTestRuntimeRunIdentity *left,const UmiTestRuntimeRunIdentity *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
