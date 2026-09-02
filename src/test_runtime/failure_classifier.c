/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/failure_classifier.c
 *
 * PURPOSE:
 *   Implement deterministic failure classification for CTest and native process evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/failure_classifier.h"
#include <string.h>

#define UMI_WINDOWS_STATUS_ACCESS_VIOLATION INT64_C(0xC0000005)
#define UMI_WINDOWS_STATUS_STACK_BUFFER_OVERRUN INT64_C(0xC0000409)
#define UMI_WINDOWS_STATUS_STACK_OVERFLOW INT64_C(0xC00000FD)
#define UMI_WINDOWS_STATUS_DLL_NOT_FOUND INT64_C(0xC0000135)

/*
 * Provide the test runtime classify process outcome operation used by this module and its
 * client applications.
 */
UmiTestRuntimeFailureKind umi_test_runtime_classify_process_outcome(const UmiTestRuntimeProcessOutcome *outcome)
{
    uint64_t status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (outcome == NULL) return UMI_TEST_RUNTIME_FAILURE_UNKNOWN;
    status = (uint64_t)outcome->native_status & UINT64_C(0xFFFFFFFF);
    /* Apply this operation only while the related capability or state is available. */
    if (!outcome->working_directory_exists) return UMI_TEST_RUNTIME_FAILURE_INVALID_WORKING_DIRECTORY;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!outcome->executable_exists) return UMI_TEST_RUNTIME_FAILURE_MISSING_EXECUTABLE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (outcome->runtime_dependency_missing || status == UINT64_C(0xC0000135)) return UMI_TEST_RUNTIME_FAILURE_MISSING_RUNTIME_DEPENDENCY;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!outcome->process_started) return UMI_TEST_RUNTIME_FAILURE_BAD_COMMAND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (outcome->timed_out) return UMI_TEST_RUNTIME_FAILURE_TIMEOUT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (outcome->cancelled) return UMI_TEST_RUNTIME_FAILURE_CONFIGURATION;
    /* Apply this branch only when its contract condition is satisfied. */
    if (outcome->sanitizer_reported) return UMI_TEST_RUNTIME_FAILURE_SANITIZER;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UINT64_C(0xC0000005)) return UMI_TEST_RUNTIME_FAILURE_ACCESS_VIOLATION;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UINT64_C(0xC00000FD)) return UMI_TEST_RUNTIME_FAILURE_STACK_OVERFLOW;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UINT64_C(0xC0000409)) return UMI_TEST_RUNTIME_FAILURE_FAST_FAIL;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((status & UINT64_C(0xC0000000)) == UINT64_C(0xC0000000)) return UMI_TEST_RUNTIME_FAILURE_CRASH;
    /* Apply this branch only when its contract condition is satisfied. */
    if (outcome->exit_code != 0) return UMI_TEST_RUNTIME_FAILURE_NONZERO_EXIT;
    return UMI_TEST_RUNTIME_FAILURE_NONE;
}

/*
 * Provide the test runtime build failure evidence operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_build_failure_evidence(const char *test_id,
                                                  const char *executable,
                                                  const UmiTestRuntimeProcessOutcome *outcome,
                                                  UmiTestRuntimeFailureEvidence *out_evidence)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (test_id == NULL || executable == NULL || outcome == NULL || out_evidence == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_evidence, 0, sizeof(*out_evidence));
    out_evidence->structure_size = (uint32_t)sizeof(*out_evidence);
    status = umi_test_runtime_copy_text(out_evidence->test_id, sizeof(out_evidence->test_id), test_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_test_runtime_copy_text(out_evidence->executable, sizeof(out_evidence->executable), executable);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_evidence->kind = umi_test_runtime_classify_process_outcome(outcome);
    out_evidence->native_status = outcome->native_status;
    out_evidence->exit_code = outcome->exit_code;
    out_evidence->revision = 1U;
    return UMI_STATUS_OK;
}
