/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/slow_test.c
 *
 * PURPOSE:
 *   Implement identify tests whose duration exceeds their historical operating envelope.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/slow_test.h"
#include <string.h>
void umi_test_runtime_slow_test_init(UmiTestRuntimeSlowTest *value,const char *id)
{
    if(value==NULL)return;
    memset(value,0,sizeof(*value));
    value->structure_size=(uint32_t)sizeof(*value);
    value->active=true;
    value->revision=1U;
    if(id!=NULL)(void)umi_test_runtime_copy_text(value->id,sizeof(value->id),id);
    }
UmiStatus umi_test_runtime_slow_test_validate(const UmiTestRuntimeSlowTest *value)
{
    if(value==NULL||value->structure_size!=sizeof(*value))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_test_runtime_text_is_valid(value->id,sizeof(value->id))||value->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_test_runtime_text_is_valid(value->category,sizeof(value->category))||!umi_test_runtime_text_is_valid(value->detail,sizeof(value->detail)))return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
    }
UmiStatus umi_test_runtime_slow_test_set_category(UmiTestRuntimeSlowTest *value,const char *category)
{
    UmiStatus s;
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_test_runtime_copy_text(value->category,sizeof(value->category),category);
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
UmiStatus umi_test_runtime_slow_test_set_detail(UmiTestRuntimeSlowTest *value,const char *detail)
{
    UmiStatus s;
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_test_runtime_copy_text(value->detail,sizeof(value->detail),detail);
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
UmiStatus umi_test_runtime_slow_test_set_duration_ms(UmiTestRuntimeSlowTest *value,uint64_t number)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->duration_ms=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
UmiStatus umi_test_runtime_slow_test_set_threshold_ms(UmiTestRuntimeSlowTest *value,uint64_t number)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->threshold_ms=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
UmiStatus umi_test_runtime_slow_test_set_active(UmiTestRuntimeSlowTest *value,bool active)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->active=active;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
bool umi_test_runtime_slow_test_same_identity(const UmiTestRuntimeSlowTest *left,const UmiTestRuntimeSlowTest *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
