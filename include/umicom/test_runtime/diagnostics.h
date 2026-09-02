/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/diagnostics.h
 *
 * PURPOSE:
 *   Maintain a bounded collection of runtime diagnostics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_DIAGNOSTICS
#define UMICOM_TEST_RUNTIME_DIAGNOSTICS

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime diagnostics data shared with callers of this public contract.
 */
typedef struct UmiTestRuntimeDiagnostics {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t diagnostic_count;
    uint64_t generation;
    uint64_t revision;
    uint64_t updated_at_ms;
    bool enabled;
} UmiTestRuntimeDiagnostics;

/**
 * Initialise test runtime diagnostics from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_diagnostics_init(UmiTestRuntimeDiagnostics *value, const char *id);
/**
 * Check that test runtime diagnostics satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_test_runtime_diagnostics_validate(const UmiTestRuntimeDiagnostics *value);
/**
 * Provide the test runtime diagnostics set name operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_diagnostics_set_name(UmiTestRuntimeDiagnostics *value, const char *name);
/**
 * Provide the test runtime diagnostics set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_diagnostics_set_detail(UmiTestRuntimeDiagnostics *value, const char *detail);
/**
 * Return the number of records represented by test runtime diagnostics set diagnostic
 * without changing their state.
 */
UmiStatus umi_test_runtime_diagnostics_set_diagnostic_count(UmiTestRuntimeDiagnostics *value, uint64_t number);
/**
 * Provide the test runtime diagnostics set generation operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_diagnostics_set_generation(UmiTestRuntimeDiagnostics *value, uint64_t number);
/**
 * Provide the test runtime diagnostics touch operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_diagnostics_touch(UmiTestRuntimeDiagnostics *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime diagnostics same identity operation used by this module and its
 * client applications.
 */
bool umi_test_runtime_diagnostics_same_identity(const UmiTestRuntimeDiagnostics *left, const UmiTestRuntimeDiagnostics *right);

#ifdef __cplusplus
}
#endif
#endif
