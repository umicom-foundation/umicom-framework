/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_dependency_evidence.c
 *
 * PURPOSE:
 *   Verify the dependency evidence contract and revision behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/sdk_runtime/dependency_evidence.h"
int main(void)
{
    UmiSdkRuntimeDependencyEvidence value; UmiSdkRuntimeDependencyEvidence same; uint64_t revision;
    umi_sdk_runtime_dependency_evidence_init(&value, "sdk-runtime.dependency_evidence");
    assert(umi_sdk_runtime_dependency_evidence_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_sdk_runtime_dependency_evidence_set_path(&value, "share/umicom/runtime") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_dependency_evidence_set_detail(&value, "validated runtime evidence") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_dependency_evidence_set_required(&value, 3U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_dependency_evidence_set_resolved(&value, 5U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_dependency_evidence_set_state(&value, UMI_SDK_RUNTIME_STATE_READY) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.required == 3U && value.resolved == 5U);
    umi_sdk_runtime_dependency_evidence_init(&same, "sdk-runtime.dependency_evidence");
    assert(umi_sdk_runtime_dependency_evidence_same_identity(&value, &same));
    assert(strcmp(value.path, "share/umicom/runtime") == 0);
    return 0;
}
