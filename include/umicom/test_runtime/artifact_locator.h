/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/artifact_locator.h
 *
 * PURPOSE:
 *   Resolve test executables, logs and generated evidence from build roots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_ARTIFACT_LOCATOR
#define UMICOM_TEST_RUNTIME_ARTIFACT_LOCATOR

#include "umicom/test_runtime/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime artifact locator data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeArtifactLocator {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t candidate_count;
    uint64_t resolved;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeArtifactLocator;

/**
 * Initialise test runtime artifact locator from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_artifact_locator_init(UmiTestRuntimeArtifactLocator *value, const char *id);
/**
 * Check that test runtime artifact locator satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_artifact_locator_validate(const UmiTestRuntimeArtifactLocator *value);
/**
 * Provide the test runtime artifact locator set name operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_artifact_locator_set_name(UmiTestRuntimeArtifactLocator *value, const char *name);
/**
 * Provide the test runtime artifact locator set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_artifact_locator_set_detail(UmiTestRuntimeArtifactLocator *value, const char *detail);
/**
 * Return the number of records represented by test runtime artifact locator set candidate
 * without changing their state.
 */
UmiStatus umi_test_runtime_artifact_locator_set_candidate_count(UmiTestRuntimeArtifactLocator *value, uint64_t number);
/**
 * Provide the test runtime artifact locator set resolved operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_artifact_locator_set_resolved(UmiTestRuntimeArtifactLocator *value, uint64_t number);
/**
 * Provide the test runtime artifact locator touch operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_artifact_locator_touch(UmiTestRuntimeArtifactLocator *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime artifact locator same identity operation used by this module
 * and its client applications.
 */
bool umi_test_runtime_artifact_locator_same_identity(const UmiTestRuntimeArtifactLocator *left, const UmiTestRuntimeArtifactLocator *right);


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
UmiStatus umi_test_runtime_artifact_locator_replace_if_current(UmiTestRuntimeArtifactLocator *value,
    uint64_t expected_revision, const UmiTestRuntimeArtifactLocator *proposal);

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
UmiStatus umi_test_runtime_artifact_locator_init_checked(UmiTestRuntimeArtifactLocator *value, const char *id);

/** Encode this value using Framework's portable archive ownership rules in
 * value_archive.h. NULL bytes with zero capacity measures the exact size.
 * Decoding checks the complete schema, checksum, text bounds and domain
 * validator before publishing. Structure size is rebuilt for the local host;
 * a saved revision is evidence, not authority to replace a live owner.
 * Keep source and destination storage separate. Neither call performs I/O. */
UmiStatus umi_test_runtime_artifact_locator_archive_encode(const UmiTestRuntimeArtifactLocator *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_test_runtime_artifact_locator_archive_decode(const void *bytes, size_t byte_count,
    UmiTestRuntimeArtifactLocator *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiTestRuntimeArtifactLocator object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_test_runtime_artifact_locator_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiTestRuntimeArtifactLocator *workspace, UmiTestRuntimeArtifactLocator *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the registry publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_test_runtime_artifact_locator_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiTestRuntimeArtifactLocator *expected, UmiTestRuntimeArtifactLocator *value);


#ifdef __cplusplus
}
#endif
#endif
