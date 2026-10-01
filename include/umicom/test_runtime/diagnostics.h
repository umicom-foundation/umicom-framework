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


/** Publish a complete reviewed value while expected_revision still matches.
 * Copy the live record, edit that copy with the ordinary setters, and retain
 * the revision observed before editing. Both values must validate and have
 * the same ID. A stale review returns INVALID_STATE; exhausted revision space
 * returns CAPACITY_EXCEEDED. All failures leave the live value unchanged.
 * Success copies every proposal field and assigns one new live revision;
 * the proposal's own revision does not control publication. Self-assignment
 * is allowed and also advances once. Other overlapping storage is unsupported.
 * Serialize access on the owner. This value operation allocates nothing and
 * does not perform I/O, authenticate evidence or run the described workflow. */
UmiStatus umi_test_runtime_diagnostics_replace_if_current(UmiTestRuntimeDiagnostics *value,
    uint64_t expected_revision, const UmiTestRuntimeDiagnostics *proposal);

/** Construct the usual default value and report invalid input.
 * A null or empty identity returns INVALID_ARGUMENT. An identity without a
 * terminator in sizeof(value->id) readable bytes returns CAPACITY_EXCEEDED;
 * a shorter C string is read only through its terminator. The existing domain
 * validator checks the staged defaults before publication. Any refusal leaves
 * the destination unchanged. id may refer to the destination's own text.
 * This initializes a new value, resetting its fields and revision to the
 * established defaults; do not use it as a live edit while observers retain
 * that identity. It owns no resources, allocates nothing and performs no I/O.
 * Existing void initialization remains available for compatibility. */
UmiStatus umi_test_runtime_diagnostics_init_checked(UmiTestRuntimeDiagnostics *value, const char *id);

#ifdef __cplusplus
}
#endif
#endif
