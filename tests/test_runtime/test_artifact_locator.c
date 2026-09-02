/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_artifact_locator.c
 *
 * PURPOSE:
 *   Verify the artifact locator contract, bounded text and revision behaviour.
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
#include "umicom/test_runtime/artifact_locator.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTestRuntimeArtifactLocator value;
    UmiTestRuntimeArtifactLocator same;
    uint64_t revision;
    umi_test_runtime_artifact_locator_init(&value, "test-runtime.artifact_locator");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_artifact_locator_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_artifact_locator_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_artifact_locator_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_artifact_locator_set_candidate_count(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_artifact_locator_set_resolved(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_artifact_locator_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.candidate_count == 7U);
    assert(value.resolved == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_artifact_locator_init(&same, "test-runtime.artifact_locator");
    assert(umi_test_runtime_artifact_locator_same_identity(&value, &same));
    assert(umi_test_runtime_artifact_locator_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
