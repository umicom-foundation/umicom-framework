/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/postflight.c
 *
 * PURPOSE:
 *   Implement verify cleanup, evidence capture and resource release after tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/postflight.h"
#include <string.h>
void umi_test_runtime_postflight_init(UmiTestRuntimePostflight *value,const char *id)
{
    if(value==NULL)return;
    memset(value,0,sizeof(*value));
    value->structure_size=(uint32_t)sizeof(*value);
    value->revision=1U;
    value->enabled=true;
    if(id!=NULL)(void)umi_test_runtime_copy_text(value->id,sizeof(value->id),id);
    }
UmiStatus umi_test_runtime_postflight_validate(const UmiTestRuntimePostflight *value)
{
    if(value==NULL||value->structure_size!=sizeof(*value))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_test_runtime_text_is_valid(value->id,sizeof(value->id))||value->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_test_runtime_text_is_valid(value->detail,sizeof(value->detail)))return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
    }
UmiStatus umi_test_runtime_postflight_set_detail(UmiTestRuntimePostflight *value,const char *detail)
{
    UmiStatus s;
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_test_runtime_copy_text(value->detail,sizeof(value->detail),detail);
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
UmiStatus umi_test_runtime_postflight_set_check_count(UmiTestRuntimePostflight *value,uint64_t number)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->check_count=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
UmiStatus umi_test_runtime_postflight_set_failure_count(UmiTestRuntimePostflight *value,uint64_t number)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->failure_count=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
bool umi_test_runtime_postflight_same_identity(const UmiTestRuntimePostflight *left,const UmiTestRuntimePostflight *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
