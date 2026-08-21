/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/execution_statistics.c
 *
 * PURPOSE:
 *   Implement aggregate deterministic counts and duration percentiles.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/execution_statistics.h"
#include <string.h>
void umi_test_runtime_execution_statistics_init(UmiTestRuntimeExecutionStatistics *value,const char *id)
{
    if(value==NULL)return;
    memset(value,0,sizeof(*value));
    value->structure_size=(uint32_t)sizeof(*value);
    value->active=true;
    value->revision=1U;
    if(id!=NULL)(void)umi_test_runtime_copy_text(value->id,sizeof(value->id),id);
    }
UmiStatus umi_test_runtime_execution_statistics_validate(const UmiTestRuntimeExecutionStatistics *value)
{
    if(value==NULL||value->structure_size!=sizeof(*value))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_test_runtime_text_is_valid(value->id,sizeof(value->id))||value->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_test_runtime_text_is_valid(value->category,sizeof(value->category))||!umi_test_runtime_text_is_valid(value->detail,sizeof(value->detail)))return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
    }
UmiStatus umi_test_runtime_execution_statistics_set_category(UmiTestRuntimeExecutionStatistics *value,const char *category)
{
    UmiStatus s;
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_test_runtime_copy_text(value->category,sizeof(value->category),category);
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
UmiStatus umi_test_runtime_execution_statistics_set_detail(UmiTestRuntimeExecutionStatistics *value,const char *detail)
{
    UmiStatus s;
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_test_runtime_copy_text(value->detail,sizeof(value->detail),detail);
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
UmiStatus umi_test_runtime_execution_statistics_set_sample_count(UmiTestRuntimeExecutionStatistics *value,uint64_t number)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->sample_count=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
UmiStatus umi_test_runtime_execution_statistics_set_total_duration_ms(UmiTestRuntimeExecutionStatistics *value,uint64_t number)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->total_duration_ms=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
UmiStatus umi_test_runtime_execution_statistics_set_active(UmiTestRuntimeExecutionStatistics *value,bool active)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->active=active;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
bool umi_test_runtime_execution_statistics_same_identity(const UmiTestRuntimeExecutionStatistics *left,const UmiTestRuntimeExecutionStatistics *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
