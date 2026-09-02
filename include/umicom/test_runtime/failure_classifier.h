/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/failure_classifier.h
 *
 * PURPOSE:
 *   Classify CTest and native Windows process outcomes without conflating launch failures with assertion failures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_FAILURE_CLASSIFIER_H
#define UMICOM_TEST_RUNTIME_FAILURE_CLASSIFIER_H
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime process outcome data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeProcessOutcome {
    bool process_started;
    bool timed_out;
    bool cancelled;
    bool executable_exists;
    bool working_directory_exists;
    bool runtime_dependency_missing;
    bool sanitizer_reported;
    int exit_code;
    int64_t native_status;
} UmiTestRuntimeProcessOutcome;

/**
 * Provide the test runtime classify process outcome operation used by this module and its
 * client applications.
 */
UmiTestRuntimeFailureKind umi_test_runtime_classify_process_outcome(const UmiTestRuntimeProcessOutcome *outcome);
/**
 * Provide the test runtime build failure evidence operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_build_failure_evidence(const char *test_id,
                                                  const char *executable,
                                                  const UmiTestRuntimeProcessOutcome *outcome,
                                                  UmiTestRuntimeFailureEvidence *out_evidence);
#ifdef __cplusplus
}
#endif
#endif
