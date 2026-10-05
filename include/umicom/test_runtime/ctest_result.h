/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/ctest_result.h
 *
 * PURPOSE:
 *   Represent one CTest outcome with native process evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_CTEST_RESULT
#define UMICOM_TEST_RUNTIME_CTEST_RESULT
#include "umicom/test_runtime/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime ctest result data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeCtestResult {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t duration_ms;
    uint64_t native_status;
    uint64_t revision;
    bool active;
} UmiTestRuntimeCtestResult;
/**
 * Initialise test runtime ctest result from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_ctest_result_init(UmiTestRuntimeCtestResult *value,const char *id);
/**
 * Check that test runtime ctest result satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_ctest_result_validate(const UmiTestRuntimeCtestResult *value);
/**
 * Provide the test runtime ctest result set category operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_ctest_result_set_category(UmiTestRuntimeCtestResult *value,const char *category);
/**
 * Provide the test runtime ctest result set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_ctest_result_set_detail(UmiTestRuntimeCtestResult *value,const char *detail);
/**
 * Provide the test runtime ctest result set duration ms operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_ctest_result_set_duration_ms(UmiTestRuntimeCtestResult *value,uint64_t number);
/**
 * Provide the test runtime ctest result set native status operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_ctest_result_set_native_status(UmiTestRuntimeCtestResult *value,uint64_t number);
/**
 * Provide the test runtime ctest result set active operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_ctest_result_set_active(UmiTestRuntimeCtestResult *value,bool active);
/**
 * Provide the test runtime ctest result same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_ctest_result_same_identity(const UmiTestRuntimeCtestResult *left,const UmiTestRuntimeCtestResult *right);
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
UmiStatus umi_test_runtime_ctest_result_init_checked(UmiTestRuntimeCtestResult *value, const char *id);

/** Encode this value using Framework's portable archive ownership rules in
 * value_archive.h. NULL bytes with zero capacity measures the exact size.
 * Decoding checks the complete schema, checksum, text bounds and domain
 * validator before publishing. Structure size is rebuilt for the local host;
 * a saved revision is evidence, not authority to replace a live owner.
 * Keep source and destination storage separate. Neither call performs I/O. */
UmiStatus umi_test_runtime_ctest_result_archive_encode(const UmiTestRuntimeCtestResult *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_test_runtime_ctest_result_archive_decode(const void *bytes, size_t byte_count,
    UmiTestRuntimeCtestResult *value);

#ifdef __cplusplus
}
#endif
#endif
