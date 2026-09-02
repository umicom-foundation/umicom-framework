/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/environment_entry.h
 *
 * PURPOSE:
 *   Represent one inherited or overridden test environment variable.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_ENVIRONMENT_ENTRY
#define UMICOM_TEST_RUNTIME_ENVIRONMENT_ENTRY
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime environment entry data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeEnvironmentEntry {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t required;
    uint64_t redacted;
    uint64_t revision;
    bool active;
} UmiTestRuntimeEnvironmentEntry;
/**
 * Initialise test runtime environment entry from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_environment_entry_init(UmiTestRuntimeEnvironmentEntry *value,const char *id);
/**
 * Check that test runtime environment entry satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_environment_entry_validate(const UmiTestRuntimeEnvironmentEntry *value);
/**
 * Provide the test runtime environment entry set category operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_environment_entry_set_category(UmiTestRuntimeEnvironmentEntry *value,const char *category);
/**
 * Provide the test runtime environment entry set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_environment_entry_set_detail(UmiTestRuntimeEnvironmentEntry *value,const char *detail);
/**
 * Provide the test runtime environment entry set required operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_environment_entry_set_required(UmiTestRuntimeEnvironmentEntry *value,uint64_t number);
/**
 * Provide the test runtime environment entry set redacted operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_environment_entry_set_redacted(UmiTestRuntimeEnvironmentEntry *value,uint64_t number);
/**
 * Provide the test runtime environment entry set active operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_environment_entry_set_active(UmiTestRuntimeEnvironmentEntry *value,bool active);
/**
 * Provide the test runtime environment entry same identity operation used by this module
 * and its client applications.
 */
bool umi_test_runtime_environment_entry_same_identity(const UmiTestRuntimeEnvironmentEntry *left,const UmiTestRuntimeEnvironmentEntry *right);
#ifdef __cplusplus
}
#endif
#endif
