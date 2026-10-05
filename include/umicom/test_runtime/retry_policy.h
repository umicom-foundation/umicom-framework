/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/retry_policy.h
 *
 * PURPOSE:
 *   Define bounded retries only for explicitly retryable infrastructure failures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_RETRY_POLICY
#define UMICOM_TEST_RUNTIME_RETRY_POLICY

#include "umicom/test_runtime/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime retry policy data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeRetryPolicy {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t maximum_attempts;
    uint64_t backoff_ms;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeRetryPolicy;

/**
 * Initialise test runtime retry policy from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_retry_policy_init(UmiTestRuntimeRetryPolicy *value, const char *id);
/**
 * Check that test runtime retry policy satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_retry_policy_validate(const UmiTestRuntimeRetryPolicy *value);
/**
 * Provide the test runtime retry policy set name operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_retry_policy_set_name(UmiTestRuntimeRetryPolicy *value, const char *name);
/**
 * Provide the test runtime retry policy set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_retry_policy_set_detail(UmiTestRuntimeRetryPolicy *value, const char *detail);
/**
 * Provide the test runtime retry policy set maximum attempts operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_retry_policy_set_maximum_attempts(UmiTestRuntimeRetryPolicy *value, uint64_t number);
/**
 * Provide the test runtime retry policy set backoff ms operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_retry_policy_set_backoff_ms(UmiTestRuntimeRetryPolicy *value, uint64_t number);
/**
 * Provide the test runtime retry policy touch operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_retry_policy_touch(UmiTestRuntimeRetryPolicy *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime retry policy same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_retry_policy_same_identity(const UmiTestRuntimeRetryPolicy *left, const UmiTestRuntimeRetryPolicy *right);


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
UmiStatus umi_test_runtime_retry_policy_replace_if_current(UmiTestRuntimeRetryPolicy *value,
    uint64_t expected_revision, const UmiTestRuntimeRetryPolicy *proposal);

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
UmiStatus umi_test_runtime_retry_policy_init_checked(UmiTestRuntimeRetryPolicy *value, const char *id);

/** Encode this value using Framework's portable archive ownership rules in
 * value_archive.h. NULL bytes with zero capacity measures the exact size.
 * Decoding checks the complete schema, checksum, text bounds and domain
 * validator before publishing. Structure size is rebuilt for the local host;
 * a saved revision is evidence, not authority to replace a live owner.
 * Keep source and destination storage separate. Neither call performs I/O. */
UmiStatus umi_test_runtime_retry_policy_archive_encode(const UmiTestRuntimeRetryPolicy *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_test_runtime_retry_policy_archive_decode(const void *bytes, size_t byte_count,
    UmiTestRuntimeRetryPolicy *value);

#ifdef __cplusplus
}
#endif
#endif
