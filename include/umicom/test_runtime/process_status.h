/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/process_status.h
 *
 * PURPOSE:
 *   Retain native process status independently from test assertion outcome.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_PROCESS_STATUS
#define UMICOM_TEST_RUNTIME_PROCESS_STATUS
#include "umicom/test_runtime/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime process status data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeProcessStatus {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t native_status;
    uint64_t exit_code;
    uint64_t revision;
    bool active;
} UmiTestRuntimeProcessStatus;
/**
 * Initialise test runtime process status from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_process_status_init(UmiTestRuntimeProcessStatus *value,const char *id);
/**
 * Check that test runtime process status satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_process_status_validate(const UmiTestRuntimeProcessStatus *value);
/**
 * Provide the test runtime process status set category operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_process_status_set_category(UmiTestRuntimeProcessStatus *value,const char *category);
/**
 * Provide the test runtime process status set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_process_status_set_detail(UmiTestRuntimeProcessStatus *value,const char *detail);
/**
 * Provide the test runtime process status set native status operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_process_status_set_native_status(UmiTestRuntimeProcessStatus *value,uint64_t number);
/**
 * Provide the test runtime process status set exit code operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_process_status_set_exit_code(UmiTestRuntimeProcessStatus *value,uint64_t number);
/**
 * Provide the test runtime process status set active operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_process_status_set_active(UmiTestRuntimeProcessStatus *value,bool active);
/**
 * Provide the test runtime process status same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_process_status_same_identity(const UmiTestRuntimeProcessStatus *left,const UmiTestRuntimeProcessStatus *right);
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
UmiStatus umi_test_runtime_process_status_init_checked(UmiTestRuntimeProcessStatus *value, const char *id);

/** Encode this value using Framework's portable archive ownership rules in
 * value_archive.h. NULL bytes with zero capacity measures the exact size.
 * Decoding checks the complete schema, checksum, text bounds and domain
 * validator before publishing. Structure size is rebuilt for the local host;
 * a saved revision is evidence, not authority to replace a live owner.
 * Keep source and destination storage separate. Neither call performs I/O. */
UmiStatus umi_test_runtime_process_status_archive_encode(const UmiTestRuntimeProcessStatus *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_test_runtime_process_status_archive_decode(const void *bytes, size_t byte_count,
    UmiTestRuntimeProcessStatus *value);

#ifdef __cplusplus
}
#endif
#endif
