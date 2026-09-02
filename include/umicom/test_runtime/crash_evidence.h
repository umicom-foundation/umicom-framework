/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/crash_evidence.h
 *
 * PURPOSE:
 *   Retain crash status, fault address and failure category evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_CRASH_EVIDENCE
#define UMICOM_TEST_RUNTIME_CRASH_EVIDENCE

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime crash evidence data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeCrashEvidence {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t native_status;
    uint64_t thread_id;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeCrashEvidence;

/**
 * Initialise test runtime crash evidence from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_crash_evidence_init(UmiTestRuntimeCrashEvidence *value, const char *id);
/**
 * Check that test runtime crash evidence satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_crash_evidence_validate(const UmiTestRuntimeCrashEvidence *value);
/**
 * Provide the test runtime crash evidence set name operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_crash_evidence_set_name(UmiTestRuntimeCrashEvidence *value, const char *name);
/**
 * Provide the test runtime crash evidence set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_crash_evidence_set_detail(UmiTestRuntimeCrashEvidence *value, const char *detail);
/**
 * Provide the test runtime crash evidence set native status operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_crash_evidence_set_native_status(UmiTestRuntimeCrashEvidence *value, uint64_t number);
/**
 * Provide the test runtime crash evidence set thread id operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_crash_evidence_set_thread_id(UmiTestRuntimeCrashEvidence *value, uint64_t number);
/**
 * Provide the test runtime crash evidence touch operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_crash_evidence_touch(UmiTestRuntimeCrashEvidence *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime crash evidence same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_crash_evidence_same_identity(const UmiTestRuntimeCrashEvidence *left, const UmiTestRuntimeCrashEvidence *right);

#ifdef __cplusplus
}
#endif
#endif
