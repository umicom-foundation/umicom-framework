/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/ctest_record.h
 *
 * PURPOSE:
 *   Represent one CTest registration independently of generated CTest files.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_CTEST_RECORD
#define UMICOM_TEST_RUNTIME_CTEST_RECORD
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime ctest record data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeCtestRecord {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t processor_count;
    uint64_t timeout_seconds;
    uint64_t revision;
    bool active;
} UmiTestRuntimeCtestRecord;
/**
 * Initialise test runtime ctest record from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_ctest_record_init(UmiTestRuntimeCtestRecord *value,const char *id);
/**
 * Check that test runtime ctest record satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_ctest_record_validate(const UmiTestRuntimeCtestRecord *value);
/**
 * Provide the test runtime ctest record set category operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_ctest_record_set_category(UmiTestRuntimeCtestRecord *value,const char *category);
/**
 * Provide the test runtime ctest record set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_ctest_record_set_detail(UmiTestRuntimeCtestRecord *value,const char *detail);
/**
 * Return the number of records represented by test runtime ctest record set processor
 * without changing their state.
 */
UmiStatus umi_test_runtime_ctest_record_set_processor_count(UmiTestRuntimeCtestRecord *value,uint64_t number);
/**
 * Provide the test runtime ctest record set timeout seconds operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_ctest_record_set_timeout_seconds(UmiTestRuntimeCtestRecord *value,uint64_t number);
/**
 * Provide the test runtime ctest record set active operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_ctest_record_set_active(UmiTestRuntimeCtestRecord *value,bool active);
/**
 * Provide the test runtime ctest record same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_ctest_record_same_identity(const UmiTestRuntimeCtestRecord *left,const UmiTestRuntimeCtestRecord *right);
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
UmiStatus umi_test_runtime_ctest_record_init_checked(UmiTestRuntimeCtestRecord *value, const char *id);

#ifdef __cplusplus
}
#endif
#endif
