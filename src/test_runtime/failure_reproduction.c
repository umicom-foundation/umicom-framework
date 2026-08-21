/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/failure_reproduction.c
 *
 * PURPOSE:
 *   Implement retain the exact runtime context required to reproduce a failure.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/failure_reproduction.h"
#include <string.h>
void umi_test_runtime_failure_reproduction_init(UmiTestRuntimeFailureReproduction *value,const char *id)
{
    if(value==NULL)return;
    memset(value,0,sizeof(*value));
    value->structure_size=(uint32_t)sizeof(*value);
    value->revision=1U;
    value->enabled=true;
    if(id!=NULL)(void)umi_test_runtime_copy_text(value->id,sizeof(value->id),id);
    }
UmiStatus umi_test_runtime_failure_reproduction_validate(const UmiTestRuntimeFailureReproduction *value)
{
    if(value==NULL||value->structure_size!=sizeof(*value))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_test_runtime_text_is_valid(value->id,sizeof(value->id))||value->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_test_runtime_text_is_valid(value->detail,sizeof(value->detail)))return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
    }
UmiStatus umi_test_runtime_failure_reproduction_set_detail(UmiTestRuntimeFailureReproduction *value,const char *detail)
{
    UmiStatus s;
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_test_runtime_copy_text(value->detail,sizeof(value->detail),detail);
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
UmiStatus umi_test_runtime_failure_reproduction_set_evidence_count(UmiTestRuntimeFailureReproduction *value,uint64_t number)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->evidence_count=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
UmiStatus umi_test_runtime_failure_reproduction_set_generation(UmiTestRuntimeFailureReproduction *value,uint64_t number)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->generation=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
bool umi_test_runtime_failure_reproduction_same_identity(const UmiTestRuntimeFailureReproduction *left,const UmiTestRuntimeFailureReproduction *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
