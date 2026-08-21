/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/trend.c
 *
 * PURPOSE:
 *   Implement retain pass rate, duration and failure-category trends across runs.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/trend.h"
#include <string.h>
void umi_test_runtime_trend_init(UmiTestRuntimeTrend *value,const char *id)
{
    if(value==NULL)return;
    memset(value,0,sizeof(*value));
    value->structure_size=(uint32_t)sizeof(*value);
    value->active=true;
    value->revision=1U;
    if(id!=NULL)(void)umi_test_runtime_copy_text(value->id,sizeof(value->id),id);
    }
UmiStatus umi_test_runtime_trend_validate(const UmiTestRuntimeTrend *value)
{
    if(value==NULL||value->structure_size!=sizeof(*value))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_test_runtime_text_is_valid(value->id,sizeof(value->id))||value->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_test_runtime_text_is_valid(value->category,sizeof(value->category))||!umi_test_runtime_text_is_valid(value->detail,sizeof(value->detail)))return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
    }
UmiStatus umi_test_runtime_trend_set_category(UmiTestRuntimeTrend *value,const char *category)
{
    UmiStatus s;
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_test_runtime_copy_text(value->category,sizeof(value->category),category);
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
UmiStatus umi_test_runtime_trend_set_detail(UmiTestRuntimeTrend *value,const char *detail)
{
    UmiStatus s;
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_test_runtime_copy_text(value->detail,sizeof(value->detail),detail);
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
UmiStatus umi_test_runtime_trend_set_sample_count(UmiTestRuntimeTrend *value,uint64_t number)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->sample_count=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
UmiStatus umi_test_runtime_trend_set_generation(UmiTestRuntimeTrend *value,uint64_t number)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->generation=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
UmiStatus umi_test_runtime_trend_set_active(UmiTestRuntimeTrend *value,bool active)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->active=active;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
bool umi_test_runtime_trend_same_identity(const UmiTestRuntimeTrend *left,const UmiTestRuntimeTrend *right){return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;}
