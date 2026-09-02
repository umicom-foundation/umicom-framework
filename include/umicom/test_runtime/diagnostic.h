/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/diagnostic.h
 *
 * PURPOSE:
 *   Represent actionable test-runtime diagnostic evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_DIAGNOSTIC
#define UMICOM_TEST_RUNTIME_DIAGNOSTIC

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime diagnostic data shared with callers of this public contract.
 */
typedef struct UmiTestRuntimeDiagnostic {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t severity;
    uint64_t sequence;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeDiagnostic;

/**
 * Initialise test runtime diagnostic from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_diagnostic_init(UmiTestRuntimeDiagnostic *value, const char *id);
/**
 * Check that test runtime diagnostic satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_test_runtime_diagnostic_validate(const UmiTestRuntimeDiagnostic *value);
/**
 * Provide the test runtime diagnostic set name operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_diagnostic_set_name(UmiTestRuntimeDiagnostic *value, const char *name);
/**
 * Provide the test runtime diagnostic set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_diagnostic_set_detail(UmiTestRuntimeDiagnostic *value, const char *detail);
/**
 * Provide the test runtime diagnostic set severity operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_diagnostic_set_severity(UmiTestRuntimeDiagnostic *value, uint64_t number);
/**
 * Provide the test runtime diagnostic set sequence operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_diagnostic_set_sequence(UmiTestRuntimeDiagnostic *value, uint64_t number);
/**
 * Provide the test runtime diagnostic touch operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_diagnostic_touch(UmiTestRuntimeDiagnostic *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime diagnostic same identity operation used by this module and its
 * client applications.
 */
bool umi_test_runtime_diagnostic_same_identity(const UmiTestRuntimeDiagnostic *left, const UmiTestRuntimeDiagnostic *right);

#ifdef __cplusplus
}
#endif
#endif
