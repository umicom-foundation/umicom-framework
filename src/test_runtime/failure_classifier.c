/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/failure_classifier.c
 *
 * PURPOSE:
 *   Implement deterministic failure classification for CTest and native process evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/failure_classifier.h"
#include <string.h>

#define UMI_WINDOWS_STATUS_ACCESS_VIOLATION INT64_C(0xC0000005)
#define UMI_WINDOWS_STATUS_STACK_BUFFER_OVERRUN INT64_C(0xC0000409)
#define UMI_WINDOWS_STATUS_STACK_OVERFLOW INT64_C(0xC00000FD)
#define UMI_WINDOWS_STATUS_DLL_NOT_FOUND INT64_C(0xC0000135)

UmiTestRuntimeFailureKind umi_test_runtime_classify_process_outcome(const UmiTestRuntimeProcessOutcome *outcome)
{
    uint64_t status;
    if (outcome == NULL) return UMI_TEST_RUNTIME_FAILURE_UNKNOWN;
    status = (uint64_t)outcome->native_status & UINT64_C(0xFFFFFFFF);
    if (!outcome->working_directory_exists) return UMI_TEST_RUNTIME_FAILURE_INVALID_WORKING_DIRECTORY;
    if (!outcome->executable_exists) return UMI_TEST_RUNTIME_FAILURE_MISSING_EXECUTABLE;
    if (outcome->runtime_dependency_missing || status == UINT64_C(0xC0000135)) return UMI_TEST_RUNTIME_FAILURE_MISSING_RUNTIME_DEPENDENCY;
    if (!outcome->process_started) return UMI_TEST_RUNTIME_FAILURE_BAD_COMMAND;
    if (outcome->timed_out) return UMI_TEST_RUNTIME_FAILURE_TIMEOUT;
    if (outcome->cancelled) return UMI_TEST_RUNTIME_FAILURE_CONFIGURATION;
    if (outcome->sanitizer_reported) return UMI_TEST_RUNTIME_FAILURE_SANITIZER;
    if (status == UINT64_C(0xC0000005)) return UMI_TEST_RUNTIME_FAILURE_ACCESS_VIOLATION;
    if (status == UINT64_C(0xC00000FD)) return UMI_TEST_RUNTIME_FAILURE_STACK_OVERFLOW;
    if (status == UINT64_C(0xC0000409)) return UMI_TEST_RUNTIME_FAILURE_FAST_FAIL;
    if ((status & UINT64_C(0xC0000000)) == UINT64_C(0xC0000000)) return UMI_TEST_RUNTIME_FAILURE_CRASH;
    if (outcome->exit_code != 0) return UMI_TEST_RUNTIME_FAILURE_NONZERO_EXIT;
    return UMI_TEST_RUNTIME_FAILURE_NONE;
}

UmiStatus umi_test_runtime_build_failure_evidence(const char *test_id,
                                                  const char *executable,
                                                  const UmiTestRuntimeProcessOutcome *outcome,
                                                  UmiTestRuntimeFailureEvidence *out_evidence)
{
    UmiStatus status;
    if (test_id == NULL || executable == NULL || outcome == NULL || out_evidence == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_evidence, 0, sizeof(*out_evidence));
    out_evidence->structure_size = (uint32_t)sizeof(*out_evidence);
    status = umi_test_runtime_copy_text(out_evidence->test_id, sizeof(out_evidence->test_id), test_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_test_runtime_copy_text(out_evidence->executable, sizeof(out_evidence->executable), executable);
    if (status != UMI_STATUS_OK) return status;
    out_evidence->kind = umi_test_runtime_classify_process_outcome(outcome);
    out_evidence->native_status = outcome->native_status;
    out_evidence->exit_code = outcome->exit_code;
    out_evidence->revision = 1U;
    return UMI_STATUS_OK;
}
