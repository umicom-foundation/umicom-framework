/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/run_summary.c
 *
 * PURPOSE:
 *   Implement summarise one run without losing native failure categories.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/run_summary.h"
#include <string.h>
void umi_test_runtime_run_summary_init(UmiTestRuntimeRunSummary *value,const char *id)
{
    if(value==NULL)return;
    memset(value,0,sizeof(*value));
    value->structure_size=(uint32_t)sizeof(*value);
    value->revision=1U;
    value->enabled=true;
    if(id!=NULL)(void)umi_test_runtime_copy_text(value->id,sizeof(value->id),id);
    }
UmiStatus umi_test_runtime_run_summary_validate(const UmiTestRuntimeRunSummary *value)
{
    if(value==NULL||value->structure_size!=sizeof(*value))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_test_runtime_text_is_valid(value->id,sizeof(value->id))||value->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_test_runtime_text_is_valid(value->detail,sizeof(value->detail)))return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
    }
UmiStatus umi_test_runtime_run_summary_set_detail(UmiTestRuntimeRunSummary *value,const char *detail)
{
    UmiStatus s;
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_test_runtime_copy_text(value->detail,sizeof(value->detail),detail);
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
UmiStatus umi_test_runtime_run_summary_set_passed_count(UmiTestRuntimeRunSummary *value,uint64_t number)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->passed_count=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
UmiStatus umi_test_runtime_run_summary_set_failed_count(UmiTestRuntimeRunSummary *value,uint64_t number)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->failed_count=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
bool umi_test_runtime_run_summary_same_identity(const UmiTestRuntimeRunSummary *left,const UmiTestRuntimeRunSummary *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
