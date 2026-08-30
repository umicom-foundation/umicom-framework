/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/database_requirement.c
 *
 * PURPOSE:
 *   Implement describe database provider and isolation requirements for a test.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/database_requirement.h"
#include <string.h>
void umi_test_runtime_database_requirement_init(UmiTestRuntimeDatabaseRequirement *value,const char *id)
{
    if(value==NULL)return;
    memset(value,0,sizeof(*value));
    value->structure_size=(uint32_t)sizeof(*value);
    value->revision=1U;
    value->enabled=true;
    if(id!=NULL)(void)umi_test_runtime_copy_text(value->id,sizeof(value->id),id);
    }
UmiStatus umi_test_runtime_database_requirement_validate(const UmiTestRuntimeDatabaseRequirement *value)
{
    if(value==NULL||value->structure_size!=sizeof(*value))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_test_runtime_text_is_valid(value->id,sizeof(value->id))||value->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_test_runtime_text_is_valid(value->detail,sizeof(value->detail)))return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
    }
UmiStatus umi_test_runtime_database_requirement_set_detail(UmiTestRuntimeDatabaseRequirement *value,const char *detail)
{
    UmiStatus s;
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_test_runtime_copy_text(value->detail,sizeof(value->detail),detail);
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
UmiStatus umi_test_runtime_database_requirement_set_required(UmiTestRuntimeDatabaseRequirement *value,uint64_t number)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->required=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
UmiStatus umi_test_runtime_database_requirement_set_available(UmiTestRuntimeDatabaseRequirement *value,uint64_t number)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->available=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
bool umi_test_runtime_database_requirement_same_identity(const UmiTestRuntimeDatabaseRequirement *left,const UmiTestRuntimeDatabaseRequirement *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
