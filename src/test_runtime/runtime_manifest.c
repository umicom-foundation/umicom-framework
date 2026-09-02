/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/runtime_manifest.c
 *
 * PURPOSE:
 *   Implement describe the expected runtime files for one test executable.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/runtime_manifest.h"
#include <string.h>
/*
 * Initialise test runtime runtime manifest from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_runtime_manifest_init(UmiTestRuntimeRuntimeManifest *value,const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return;
    memset(value,0,sizeof(*value));
    value->structure_size=(uint32_t)sizeof(*value);
    value->active=true;
    value->revision=1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(id!=NULL)(void)umi_test_runtime_copy_text(value->id,sizeof(value->id),id);
    }
/*
 * Check that test runtime runtime manifest satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_runtime_manifest_validate(const UmiTestRuntimeRuntimeManifest *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL||value->structure_size!=sizeof(*value))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_test_runtime_text_is_valid(value->id,sizeof(value->id))||value->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_test_runtime_text_is_valid(value->category,sizeof(value->category))||!umi_test_runtime_text_is_valid(value->detail,sizeof(value->detail)))return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime runtime manifest set category operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_runtime_manifest_set_category(UmiTestRuntimeRuntimeManifest *value,const char *category)
{
    UmiStatus s;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_test_runtime_copy_text(value->category,sizeof(value->category),category);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
/*
 * Provide the test runtime runtime manifest set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_runtime_manifest_set_detail(UmiTestRuntimeRuntimeManifest *value,const char *detail)
{
    UmiStatus s;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_test_runtime_copy_text(value->detail,sizeof(value->detail),detail);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
/*
 * Return the number of records represented by test runtime runtime manifest set file
 * without changing their state.
 */
UmiStatus umi_test_runtime_runtime_manifest_set_file_count(UmiTestRuntimeRuntimeManifest *value,uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->file_count=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime runtime manifest set generation operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_runtime_manifest_set_generation(UmiTestRuntimeRuntimeManifest *value,uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->generation=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime runtime manifest set active operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_runtime_manifest_set_active(UmiTestRuntimeRuntimeManifest *value,bool active)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->active=active;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime runtime manifest same identity operation used by this module
 * and its client applications.
 */
bool umi_test_runtime_runtime_manifest_same_identity(const UmiTestRuntimeRuntimeManifest *left,const UmiTestRuntimeRuntimeManifest *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
