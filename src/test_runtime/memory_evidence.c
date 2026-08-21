/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/memory_evidence.c
 *
 * PURPOSE:
 *   Implement retain allocation, bounds and ownership diagnostics for test failures.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/memory_evidence.h"
#include <string.h>
void umi_test_runtime_memory_evidence_init(UmiTestRuntimeMemoryEvidence *value,const char *id)
{
    if(value==NULL)return;
    memset(value,0,sizeof(*value));
    value->structure_size=(uint32_t)sizeof(*value);
    value->active=true;
    value->revision=1U;
    if(id!=NULL)(void)umi_test_runtime_copy_text(value->id,sizeof(value->id),id);
    }
UmiStatus umi_test_runtime_memory_evidence_validate(const UmiTestRuntimeMemoryEvidence *value)
{
    if(value==NULL||value->structure_size!=sizeof(*value))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_test_runtime_text_is_valid(value->id,sizeof(value->id))||value->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_test_runtime_text_is_valid(value->category,sizeof(value->category))||!umi_test_runtime_text_is_valid(value->detail,sizeof(value->detail)))return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
    }
UmiStatus umi_test_runtime_memory_evidence_set_category(UmiTestRuntimeMemoryEvidence *value,const char *category)
{
    UmiStatus s;
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_test_runtime_copy_text(value->category,sizeof(value->category),category);
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
UmiStatus umi_test_runtime_memory_evidence_set_detail(UmiTestRuntimeMemoryEvidence *value,const char *detail)
{
    UmiStatus s;
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_test_runtime_copy_text(value->detail,sizeof(value->detail),detail);
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
UmiStatus umi_test_runtime_memory_evidence_set_allocation_count(UmiTestRuntimeMemoryEvidence *value,uint64_t number)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->allocation_count=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
UmiStatus umi_test_runtime_memory_evidence_set_failure_count(UmiTestRuntimeMemoryEvidence *value,uint64_t number)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->failure_count=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
UmiStatus umi_test_runtime_memory_evidence_set_active(UmiTestRuntimeMemoryEvidence *value,bool active)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->active=active;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
bool umi_test_runtime_memory_evidence_same_identity(const UmiTestRuntimeMemoryEvidence *left,const UmiTestRuntimeMemoryEvidence *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
