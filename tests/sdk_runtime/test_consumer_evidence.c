/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_consumer_evidence.c
 *
 * PURPOSE:
 *   Verify the consumer evidence SDK/runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/sdk_runtime/consumer_evidence.h"
int main(void)
{
    UmiSdkRuntimeConsumerEvidence value,same;
    umi_sdk_runtime_consumer_evidence_init(&value,"sdk-runtime.consumer_evidence");
    assert(umi_sdk_runtime_consumer_evidence_validate(&value)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_consumer_evidence_set_path(&value,"lib/umicom")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_consumer_evidence_set_detail(&value,"installed evidence")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_consumer_evidence_set_target_count(&value,8U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_consumer_evidence_set_runtime_file_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_consumer_evidence_set_state(&value,UMI_SDK_RUNTIME_STATE_READY)==UMI_STATUS_OK);
    umi_sdk_runtime_consumer_evidence_init(&same,"sdk-runtime.consumer_evidence");
    assert(umi_sdk_runtime_consumer_evidence_same_identity(&value,&same));
    return 0;
    }
