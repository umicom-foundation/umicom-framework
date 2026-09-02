/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/failure_fingerprint.h
 *
 * PURPOSE:
 *   Build stable fingerprints for recurring failure grouping.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_FAILURE_FINGERPRINT
#define UMICOM_TEST_RUNTIME_FAILURE_FINGERPRINT
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime failure fingerprint data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeFailureFingerprint
{
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t hash;
    uint64_t occurrence_count;
    uint64_t revision;
    bool enabled;
    } UmiTestRuntimeFailureFingerprint;
/**
 * Initialise test runtime failure fingerprint from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_failure_fingerprint_init(UmiTestRuntimeFailureFingerprint *value,const char *id);
/**
 * Check that test runtime failure fingerprint satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_test_runtime_failure_fingerprint_validate(const UmiTestRuntimeFailureFingerprint *value);
/**
 * Provide the test runtime failure fingerprint set detail operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_failure_fingerprint_set_detail(UmiTestRuntimeFailureFingerprint *value,const char *detail);
/**
 * Provide the test runtime failure fingerprint set hash operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_failure_fingerprint_set_hash(UmiTestRuntimeFailureFingerprint *value,uint64_t number);
/**
 * Return the number of records represented by test runtime failure fingerprint set
 * occurrence without changing their state.
 */
UmiStatus umi_test_runtime_failure_fingerprint_set_occurrence_count(UmiTestRuntimeFailureFingerprint *value,uint64_t number);
/**
 * Provide the test runtime failure fingerprint same identity operation used by this module
 * and its client applications.
 */
bool umi_test_runtime_failure_fingerprint_same_identity(const UmiTestRuntimeFailureFingerprint *left,const UmiTestRuntimeFailureFingerprint *right);
#ifdef __cplusplus
}
#endif
#endif
