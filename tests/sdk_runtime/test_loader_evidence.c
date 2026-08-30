/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_loader_evidence.c
 *
 * PURPOSE:
 *   Verify the loader evidence contract and revision behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/sdk_runtime/loader_evidence.h"
int main(void)
{
    UmiSdkRuntimeLoaderEvidence value; UmiSdkRuntimeLoaderEvidence same; uint64_t revision;
    umi_sdk_runtime_loader_evidence_init(&value, "sdk-runtime.loader_evidence");
    assert(umi_sdk_runtime_loader_evidence_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_sdk_runtime_loader_evidence_set_path(&value, "share/umicom/runtime") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_loader_evidence_set_detail(&value, "validated runtime evidence") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_loader_evidence_set_attempt_count(&value, 3U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_loader_evidence_set_resolved_count(&value, 5U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_loader_evidence_set_state(&value, UMI_SDK_RUNTIME_STATE_READY) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.attempt_count == 3U && value.resolved_count == 5U);
    umi_sdk_runtime_loader_evidence_init(&same, "sdk-runtime.loader_evidence");
    assert(umi_sdk_runtime_loader_evidence_same_identity(&value, &same));
    assert(strcmp(value.path, "share/umicom/runtime") == 0);
    return 0;
}
