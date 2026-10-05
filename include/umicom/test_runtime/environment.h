/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/environment.h
 *
 * PURPOSE:
 *   Retain the inherited and explicit process environment used to launch tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_ENVIRONMENT
#define UMICOM_TEST_RUNTIME_ENVIRONMENT

#include "umicom/test_runtime/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime environment data shared with callers of this public contract.
 */
typedef struct UmiTestRuntimeEnvironment {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t entry_count;
    uint64_t generation;
    uint64_t revision;
    uint64_t updated_at_ms;
    bool enabled;
} UmiTestRuntimeEnvironment;

/**
 * Initialise test runtime environment from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_environment_init(UmiTestRuntimeEnvironment *value, const char *id);
/**
 * Check that test runtime environment satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_test_runtime_environment_validate(const UmiTestRuntimeEnvironment *value);
/**
 * Provide the test runtime environment set name operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_environment_set_name(UmiTestRuntimeEnvironment *value, const char *name);
/**
 * Provide the test runtime environment set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_environment_set_detail(UmiTestRuntimeEnvironment *value, const char *detail);
/**
 * Return the number of records represented by test runtime environment set entry without
 * changing their state.
 */
UmiStatus umi_test_runtime_environment_set_entry_count(UmiTestRuntimeEnvironment *value, uint64_t number);
/**
 * Provide the test runtime environment set generation operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_environment_set_generation(UmiTestRuntimeEnvironment *value, uint64_t number);
/**
 * Provide the test runtime environment touch operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_environment_touch(UmiTestRuntimeEnvironment *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime environment same identity operation used by this module and its
 * client applications.
 */
bool umi_test_runtime_environment_same_identity(const UmiTestRuntimeEnvironment *left, const UmiTestRuntimeEnvironment *right);


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
UmiStatus umi_test_runtime_environment_replace_if_current(UmiTestRuntimeEnvironment *value,
    uint64_t expected_revision, const UmiTestRuntimeEnvironment *proposal);

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
UmiStatus umi_test_runtime_environment_init_checked(UmiTestRuntimeEnvironment *value, const char *id);

/** Encode this value using Framework's portable archive ownership rules in
 * value_archive.h. NULL bytes with zero capacity measures the exact size.
 * Decoding checks the complete schema, checksum, text bounds and domain
 * validator before publishing. Structure size is rebuilt for the local host;
 * a saved revision is evidence, not authority to replace a live owner.
 * Keep source and destination storage separate. Neither call performs I/O. */
UmiStatus umi_test_runtime_environment_archive_encode(const UmiTestRuntimeEnvironment *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_test_runtime_environment_archive_decode(const void *bytes, size_t byte_count,
    UmiTestRuntimeEnvironment *value);

#ifdef __cplusplus
}
#endif
#endif
