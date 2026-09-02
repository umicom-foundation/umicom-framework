/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/process_identity.h
 *
 * PURPOSE:
 *   Retain process, parent-process and execution correlation identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_PROCESS_IDENTITY
#define UMICOM_TEST_RUNTIME_PROCESS_IDENTITY
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime process identity data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeProcessIdentity {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t process_id;
    uint64_t parent_process_id;
    uint64_t revision;
    bool active;
} UmiTestRuntimeProcessIdentity;
/**
 * Initialise test runtime process identity from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_process_identity_init(UmiTestRuntimeProcessIdentity *value,const char *id);
/**
 * Check that test runtime process identity satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_process_identity_validate(const UmiTestRuntimeProcessIdentity *value);
/**
 * Provide the test runtime process identity set category operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_process_identity_set_category(UmiTestRuntimeProcessIdentity *value,const char *category);
/**
 * Provide the test runtime process identity set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_process_identity_set_detail(UmiTestRuntimeProcessIdentity *value,const char *detail);
/**
 * Provide the test runtime process identity set process id operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_process_identity_set_process_id(UmiTestRuntimeProcessIdentity *value,uint64_t number);
/**
 * Provide the test runtime process identity set parent process id operation used by this
 * module and its client applications.
 */
UmiStatus umi_test_runtime_process_identity_set_parent_process_id(UmiTestRuntimeProcessIdentity *value,uint64_t number);
/**
 * Provide the test runtime process identity set active operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_process_identity_set_active(UmiTestRuntimeProcessIdentity *value,bool active);
/**
 * Provide the test runtime process identity same identity operation used by this module
 * and its client applications.
 */
bool umi_test_runtime_process_identity_same_identity(const UmiTestRuntimeProcessIdentity *left,const UmiTestRuntimeProcessIdentity *right);
#ifdef __cplusplus
}
#endif
#endif
