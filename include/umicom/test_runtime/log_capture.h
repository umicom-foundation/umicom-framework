/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/log_capture.h
 *
 * PURPOSE:
 *   Retain stdout/stderr capture metadata and truncation evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_LOG_CAPTURE
#define UMICOM_TEST_RUNTIME_LOG_CAPTURE
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime log capture data shared with callers of this public contract.
 */
typedef struct UmiTestRuntimeLogCapture {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t byte_count;
    uint64_t truncated;
    uint64_t revision;
    bool active;
} UmiTestRuntimeLogCapture;
/**
 * Initialise test runtime log capture from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_log_capture_init(UmiTestRuntimeLogCapture *value,const char *id);
/**
 * Check that test runtime log capture satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_test_runtime_log_capture_validate(const UmiTestRuntimeLogCapture *value);
/**
 * Provide the test runtime log capture set category operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_log_capture_set_category(UmiTestRuntimeLogCapture *value,const char *category);
/**
 * Provide the test runtime log capture set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_log_capture_set_detail(UmiTestRuntimeLogCapture *value,const char *detail);
/**
 * Return the number of records represented by test runtime log capture set byte without
 * changing their state.
 */
UmiStatus umi_test_runtime_log_capture_set_byte_count(UmiTestRuntimeLogCapture *value,uint64_t number);
/**
 * Provide the test runtime log capture set truncated operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_log_capture_set_truncated(UmiTestRuntimeLogCapture *value,uint64_t number);
/**
 * Provide the test runtime log capture set active operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_log_capture_set_active(UmiTestRuntimeLogCapture *value,bool active);
/**
 * Provide the test runtime log capture same identity operation used by this module and its
 * client applications.
 */
bool umi_test_runtime_log_capture_same_identity(const UmiTestRuntimeLogCapture *left,const UmiTestRuntimeLogCapture *right);
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
UmiStatus umi_test_runtime_log_capture_init_checked(UmiTestRuntimeLogCapture *value, const char *id);

#ifdef __cplusplus
}
#endif
#endif
