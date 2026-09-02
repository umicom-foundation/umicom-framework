/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/failure_classification.h
 *
 * PURPOSE:
 *   Classify CTest start failures, crashes, timeouts and ordinary exits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_FAILURE_CLASSIFICATION
#define UMICOM_TEST_RUNTIME_FAILURE_CLASSIFICATION

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime failure classification data shared with callers of this
 * public contract.
 */
typedef struct UmiTestRuntimeFailureClassification {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t native_status;
    uint64_t exit_code;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeFailureClassification;

/**
 * Initialise test runtime failure classification from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_failure_classification_init(UmiTestRuntimeFailureClassification *value, const char *id);
/**
 * Check that test runtime failure classification satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_test_runtime_failure_classification_validate(const UmiTestRuntimeFailureClassification *value);
/**
 * Provide the test runtime failure classification set name operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_failure_classification_set_name(UmiTestRuntimeFailureClassification *value, const char *name);
/**
 * Provide the test runtime failure classification set detail operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_failure_classification_set_detail(UmiTestRuntimeFailureClassification *value, const char *detail);
/**
 * Provide the test runtime failure classification set native status operation used by this
 * module and its client applications.
 */
UmiStatus umi_test_runtime_failure_classification_set_native_status(UmiTestRuntimeFailureClassification *value, uint64_t number);
/**
 * Provide the test runtime failure classification set exit code operation used by this
 * module and its client applications.
 */
UmiStatus umi_test_runtime_failure_classification_set_exit_code(UmiTestRuntimeFailureClassification *value, uint64_t number);
/**
 * Provide the test runtime failure classification touch operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_failure_classification_touch(UmiTestRuntimeFailureClassification *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime failure classification same identity operation used by this
 * module and its client applications.
 */
bool umi_test_runtime_failure_classification_same_identity(const UmiTestRuntimeFailureClassification *left, const UmiTestRuntimeFailureClassification *right);

#ifdef __cplusplus
}
#endif
#endif
