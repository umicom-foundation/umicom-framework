/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/execution_gate.c
 *
 * PURPOSE:
 *   Implement prevent launch when required runtime evidence is incomplete.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/execution_gate.h"
#include <string.h>
void umi_test_runtime_execution_gate_init(UmiTestRuntimeExecutionGate *value,const char *id)
{
    if(value==NULL)return;
    memset(value,0,sizeof(*value));
    value->structure_size=(uint32_t)sizeof(*value);
    value->revision=1U;
    value->enabled=true;
    if(id!=NULL)(void)umi_test_runtime_copy_text(value->id,sizeof(value->id),id);
    }
UmiStatus umi_test_runtime_execution_gate_validate(const UmiTestRuntimeExecutionGate *value)
{
    if(value==NULL||value->structure_size!=sizeof(*value))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_test_runtime_text_is_valid(value->id,sizeof(value->id))||value->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_test_runtime_text_is_valid(value->detail,sizeof(value->detail)))return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
    }
UmiStatus umi_test_runtime_execution_gate_set_detail(UmiTestRuntimeExecutionGate *value,const char *detail)
{
    UmiStatus s;
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_test_runtime_copy_text(value->detail,sizeof(value->detail),detail);
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
UmiStatus umi_test_runtime_execution_gate_set_required_count(UmiTestRuntimeExecutionGate *value,uint64_t number)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->required_count=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
UmiStatus umi_test_runtime_execution_gate_set_blocked_count(UmiTestRuntimeExecutionGate *value,uint64_t number)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->blocked_count=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
bool umi_test_runtime_execution_gate_same_identity(const UmiTestRuntimeExecutionGate *left,const UmiTestRuntimeExecutionGate *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
