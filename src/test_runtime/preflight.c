/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/preflight.c
 *
 * PURPOSE:
 *   Implement run environment, executable and dependency checks before ctest launch.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/preflight.h"
#include <string.h>
void umi_test_runtime_preflight_init(UmiTestRuntimePreflight *value,const char *id)
{
    if(value==NULL)return;
    memset(value,0,sizeof(*value));
    value->structure_size=(uint32_t)sizeof(*value);
    value->revision=1U;
    value->enabled=true;
    if(id!=NULL)(void)umi_test_runtime_copy_text(value->id,sizeof(value->id),id);
    }
UmiStatus umi_test_runtime_preflight_validate(const UmiTestRuntimePreflight *value)
{
    if(value==NULL||value->structure_size!=sizeof(*value))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_test_runtime_text_is_valid(value->id,sizeof(value->id))||value->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_test_runtime_text_is_valid(value->detail,sizeof(value->detail)))return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
    }
UmiStatus umi_test_runtime_preflight_set_detail(UmiTestRuntimePreflight *value,const char *detail)
{
    UmiStatus s;
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_test_runtime_copy_text(value->detail,sizeof(value->detail),detail);
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
UmiStatus umi_test_runtime_preflight_set_check_count(UmiTestRuntimePreflight *value,uint64_t number)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->check_count=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
UmiStatus umi_test_runtime_preflight_set_failure_count(UmiTestRuntimePreflight *value,uint64_t number)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->failure_count=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
bool umi_test_runtime_preflight_same_identity(const UmiTestRuntimePreflight *left,const UmiTestRuntimePreflight *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
