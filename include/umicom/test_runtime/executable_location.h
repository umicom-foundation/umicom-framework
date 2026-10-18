/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/executable_location.h
 *
 * PURPOSE:
 *   Represent one candidate test executable location and selection evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_EXECUTABLE_LOCATION
#define UMICOM_TEST_RUNTIME_EXECUTABLE_LOCATION
#include "umicom/test_runtime/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime executable location data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeExecutableLocation {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t priority;
    uint64_t exists;
    uint64_t revision;
    bool active;
} UmiTestRuntimeExecutableLocation;
/**
 * Initialise test runtime executable location from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_executable_location_init(UmiTestRuntimeExecutableLocation *value,const char *id);
/**
 * Check that test runtime executable location satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_test_runtime_executable_location_validate(const UmiTestRuntimeExecutableLocation *value);
/**
 * Provide the test runtime executable location set category operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_executable_location_set_category(UmiTestRuntimeExecutableLocation *value,const char *category);
/**
 * Provide the test runtime executable location set detail operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_executable_location_set_detail(UmiTestRuntimeExecutableLocation *value,const char *detail);
/**
 * Provide the test runtime executable location set priority operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_executable_location_set_priority(UmiTestRuntimeExecutableLocation *value,uint64_t number);
/**
 * Provide the test runtime executable location set exists operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_executable_location_set_exists(UmiTestRuntimeExecutableLocation *value,uint64_t number);
/**
 * Provide the test runtime executable location set active operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_executable_location_set_active(UmiTestRuntimeExecutableLocation *value,bool active);
/**
 * Provide the test runtime executable location same identity operation used by this module
 * and its client applications.
 */
bool umi_test_runtime_executable_location_same_identity(const UmiTestRuntimeExecutableLocation *left,const UmiTestRuntimeExecutableLocation *right);
/* Text setters publish a complete field and one revision together. Capacity,
 * invalid-input and exhausted-revision refusals preserve the record. Scalar
 * setters also refuse revision exhaustion. Call these on the value's owner;
 * they do not supply locking or persist changes to a storage service. */

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
UmiStatus umi_test_runtime_executable_location_init_checked(UmiTestRuntimeExecutableLocation *value, const char *id);

/** Encode this value using Framework's portable archive ownership rules in
 * value_archive.h. NULL bytes with zero capacity measures the exact size.
 * Decoding checks the complete schema, checksum, text bounds and domain
 * validator before publishing. Structure size is rebuilt for the local host;
 * a saved revision is evidence, not authority to replace a live owner.
 * Keep source and destination storage separate. Neither call performs I/O. */
UmiStatus umi_test_runtime_executable_location_archive_encode(const UmiTestRuntimeExecutableLocation *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_test_runtime_executable_location_archive_decode(const void *bytes, size_t byte_count,
    UmiTestRuntimeExecutableLocation *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiTestRuntimeExecutableLocation object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_test_runtime_executable_location_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiTestRuntimeExecutableLocation *workspace, UmiTestRuntimeExecutableLocation *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the registry publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_test_runtime_executable_location_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiTestRuntimeExecutableLocation *expected, UmiTestRuntimeExecutableLocation *value);


#ifdef __cplusplus
}
#endif
#endif
