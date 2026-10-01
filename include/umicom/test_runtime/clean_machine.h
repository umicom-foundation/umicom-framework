/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/clean_machine.h
 *
 * PURPOSE:
 *   Evaluate whether installed runtime state is sufficient outside a source tree.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_CLEAN_MACHINE
#define UMICOM_TEST_RUNTIME_CLEAN_MACHINE

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime clean machine data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeCleanMachine {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t requirement_count;
    uint64_t missing_count;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeCleanMachine;

/**
 * Initialise test runtime clean machine from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_clean_machine_init(UmiTestRuntimeCleanMachine *value, const char *id);
/**
 * Check that test runtime clean machine satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_clean_machine_validate(const UmiTestRuntimeCleanMachine *value);
/**
 * Provide the test runtime clean machine set name operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_clean_machine_set_name(UmiTestRuntimeCleanMachine *value, const char *name);
/**
 * Provide the test runtime clean machine set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_clean_machine_set_detail(UmiTestRuntimeCleanMachine *value, const char *detail);
/**
 * Return the number of records represented by test runtime clean machine set requirement
 * without changing their state.
 */
UmiStatus umi_test_runtime_clean_machine_set_requirement_count(UmiTestRuntimeCleanMachine *value, uint64_t number);
/**
 * Return the number of records represented by test runtime clean machine set missing
 * without changing their state.
 */
UmiStatus umi_test_runtime_clean_machine_set_missing_count(UmiTestRuntimeCleanMachine *value, uint64_t number);
/**
 * Provide the test runtime clean machine touch operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_clean_machine_touch(UmiTestRuntimeCleanMachine *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime clean machine same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_clean_machine_same_identity(const UmiTestRuntimeCleanMachine *left, const UmiTestRuntimeCleanMachine *right);


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
UmiStatus umi_test_runtime_clean_machine_replace_if_current(UmiTestRuntimeCleanMachine *value,
    uint64_t expected_revision, const UmiTestRuntimeCleanMachine *proposal);

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
UmiStatus umi_test_runtime_clean_machine_init_checked(UmiTestRuntimeCleanMachine *value, const char *id);

#ifdef __cplusplus
}
#endif
#endif
