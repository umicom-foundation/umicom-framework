/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/runtime_capability.c
 *
 * PURPOSE:
 *   Implement describe one capability required before a test can be launched.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/runtime_capability.h"
#include <string.h>
void umi_test_runtime_runtime_capability_init(UmiTestRuntimeRuntimeCapability *value,const char *id)
{
    if(value==NULL)return;
    memset(value,0,sizeof(*value));
    value->structure_size=(uint32_t)sizeof(*value);
    value->revision=1U;
    value->enabled=true;
    if(id!=NULL)(void)umi_test_runtime_copy_text(value->id,sizeof(value->id),id);
    }
UmiStatus umi_test_runtime_runtime_capability_validate(const UmiTestRuntimeRuntimeCapability *value)
{
    if(value==NULL||value->structure_size!=sizeof(*value))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_test_runtime_text_is_valid(value->id,sizeof(value->id))||value->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_test_runtime_text_is_valid(value->detail,sizeof(value->detail)))return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
    }
UmiStatus umi_test_runtime_runtime_capability_set_detail(UmiTestRuntimeRuntimeCapability *value,const char *detail)
{
    UmiStatus s;
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_test_runtime_copy_text(value->detail,sizeof(value->detail),detail);
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
UmiStatus umi_test_runtime_runtime_capability_set_required(UmiTestRuntimeRuntimeCapability *value,uint64_t number)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->required=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
UmiStatus umi_test_runtime_runtime_capability_set_available(UmiTestRuntimeRuntimeCapability *value,uint64_t number)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->available=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
bool umi_test_runtime_runtime_capability_same_identity(const UmiTestRuntimeRuntimeCapability *left,const UmiTestRuntimeRuntimeCapability *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
