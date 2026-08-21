/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_failure_classifier.c
 *
 * PURPOSE:
 *   Verify process-start, missing dependency, timeout and Windows fast-fail classification.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/test_runtime/failure_classifier.h"
int main(void)
{
    UmiTestRuntimeProcessOutcome outcome = {0};
    UmiTestRuntimeFailureEvidence evidence;
    outcome.executable_exists = true;
    outcome.working_directory_exists = true;
    assert(umi_test_runtime_classify_process_outcome(&outcome) == UMI_TEST_RUNTIME_FAILURE_BAD_COMMAND);
    outcome.process_started = true;
    outcome.runtime_dependency_missing = true;
    assert(umi_test_runtime_classify_process_outcome(&outcome) == UMI_TEST_RUNTIME_FAILURE_MISSING_RUNTIME_DEPENDENCY);
    outcome.runtime_dependency_missing = false;
    outcome.timed_out = true;
    assert(umi_test_runtime_classify_process_outcome(&outcome) == UMI_TEST_RUNTIME_FAILURE_TIMEOUT);
    outcome.timed_out = false;
    outcome.native_status = (int64_t)UINT64_C(0xC0000409);
    assert(umi_test_runtime_classify_process_outcome(&outcome) == UMI_TEST_RUNTIME_FAILURE_FAST_FAIL);
    assert(umi_test_runtime_build_failure_evidence("framework.example", "bin/example.exe", &outcome, &evidence) == UMI_STATUS_OK);
    assert(evidence.kind == UMI_TEST_RUNTIME_FAILURE_FAST_FAIL);
    return 0;
}
