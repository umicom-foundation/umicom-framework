/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/consumer_evidence.h
 *
 * PURPOSE:
 *   Retain how one application resolved its Framework dependency.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_CONSUMER_EVIDENCE
#define UMICOM_SDK_RUNTIME_CONSUMER_EVIDENCE
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime consumer evidence data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimeConsumerEvidence
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t target_count;
    uint64_t runtime_file_count;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
    } UmiSdkRuntimeConsumerEvidence;
/**
 * Initialise sdk runtime consumer evidence from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_consumer_evidence_init(UmiSdkRuntimeConsumerEvidence *value,const char *id);
/**
 * Check that sdk runtime consumer evidence satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_consumer_evidence_validate(const UmiSdkRuntimeConsumerEvidence *value);
/**
 * Provide the sdk runtime consumer evidence set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_consumer_evidence_set_path(UmiSdkRuntimeConsumerEvidence *value,const char *path);
/**
 * Provide the sdk runtime consumer evidence set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_consumer_evidence_set_detail(UmiSdkRuntimeConsumerEvidence *value,const char *detail);
/**
 * Return the number of records represented by sdk runtime consumer evidence set target
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_consumer_evidence_set_target_count(UmiSdkRuntimeConsumerEvidence *value,uint64_t number);
/**
 * Return the number of records represented by sdk runtime consumer evidence set runtime
 * file without changing their state.
 */
UmiStatus umi_sdk_runtime_consumer_evidence_set_runtime_file_count(UmiSdkRuntimeConsumerEvidence *value,uint64_t number);
/**
 * Provide the sdk runtime consumer evidence set state operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_consumer_evidence_set_state(UmiSdkRuntimeConsumerEvidence *value,UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime consumer evidence same identity operation used by this module
 * and its client applications.
 */
bool umi_sdk_runtime_consumer_evidence_same_identity(const UmiSdkRuntimeConsumerEvidence *left,const UmiSdkRuntimeConsumerEvidence *right);
#ifdef __cplusplus
}
#endif
#endif
