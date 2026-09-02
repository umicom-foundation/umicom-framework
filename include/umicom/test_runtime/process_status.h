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
#ifdef __cplusplus
}
#endif
#endif
