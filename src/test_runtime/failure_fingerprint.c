/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/failure_fingerprint.c
 *
 * PURPOSE:
 *   Implement build stable fingerprints for recurring failure grouping.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/failure_fingerprint.h"
#include <string.h>
void umi_test_runtime_failure_fingerprint_init(UmiTestRuntimeFailureFingerprint *value,const char *id)
{
    if(value==NULL)return;
    memset(value,0,sizeof(*value));
    value->structure_size=(uint32_t)sizeof(*value);
    value->revision=1U;
    value->enabled=true;
    if(id!=NULL)(void)umi_test_runtime_copy_text(value->id,sizeof(value->id),id);
    }
UmiStatus umi_test_runtime_failure_fingerprint_validate(const UmiTestRuntimeFailureFingerprint *value)
{
    if(value==NULL||value->structure_size!=sizeof(*value))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_test_runtime_text_is_valid(value->id,sizeof(value->id))||value->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_test_runtime_text_is_valid(value->detail,sizeof(value->detail)))return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
    }
UmiStatus umi_test_runtime_failure_fingerprint_set_detail(UmiTestRuntimeFailureFingerprint *value,const char *detail)
{
    UmiStatus s;
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_test_runtime_copy_text(value->detail,sizeof(value->detail),detail);
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
UmiStatus umi_test_runtime_failure_fingerprint_set_hash(UmiTestRuntimeFailureFingerprint *value,uint64_t number)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->hash=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
UmiStatus umi_test_runtime_failure_fingerprint_set_occurrence_count(UmiTestRuntimeFailureFingerprint *value,uint64_t number)
{
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->occurrence_count=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
bool umi_test_runtime_failure_fingerprint_same_identity(const UmiTestRuntimeFailureFingerprint *left,const UmiTestRuntimeFailureFingerprint *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
