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
void umi_sdk_runtime_consumer_evidence_init(UmiSdkRuntimeConsumerEvidence *value,const char *id);
UmiStatus umi_sdk_runtime_consumer_evidence_validate(const UmiSdkRuntimeConsumerEvidence *value);
UmiStatus umi_sdk_runtime_consumer_evidence_set_path(UmiSdkRuntimeConsumerEvidence *value,const char *path);
UmiStatus umi_sdk_runtime_consumer_evidence_set_detail(UmiSdkRuntimeConsumerEvidence *value,const char *detail);
UmiStatus umi_sdk_runtime_consumer_evidence_set_target_count(UmiSdkRuntimeConsumerEvidence *value,uint64_t number);
UmiStatus umi_sdk_runtime_consumer_evidence_set_runtime_file_count(UmiSdkRuntimeConsumerEvidence *value,uint64_t number);
UmiStatus umi_sdk_runtime_consumer_evidence_set_state(UmiSdkRuntimeConsumerEvidence *value,UmiSdkRuntimeState state);
bool umi_sdk_runtime_consumer_evidence_same_identity(const UmiSdkRuntimeConsumerEvidence *left,const UmiSdkRuntimeConsumerEvidence *right);
#ifdef __cplusplus
}
#endif
#endif
