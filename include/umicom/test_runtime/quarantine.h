/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/quarantine.h
 *
 * PURPOSE:
 *   Represent explicit temporary quarantine without silently disabling a test.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_QUARANTINE
#define UMICOM_TEST_RUNTIME_QUARANTINE
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime quarantine data shared with callers of this public contract.
 */
typedef struct UmiTestRuntimeQuarantine {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t expires_at_ms;
    uint64_t issue_count;
    uint64_t revision;
    bool active;
} UmiTestRuntimeQuarantine;
/**
 * Initialise test runtime quarantine from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_quarantine_init(UmiTestRuntimeQuarantine *value,const char *id);
/**
 * Check that test runtime quarantine satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_test_runtime_quarantine_validate(const UmiTestRuntimeQuarantine *value);
/**
 * Provide the test runtime quarantine set category operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_quarantine_set_category(UmiTestRuntimeQuarantine *value,const char *category);
/**
 * Provide the test runtime quarantine set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_quarantine_set_detail(UmiTestRuntimeQuarantine *value,const char *detail);
/**
 * Provide the test runtime quarantine set expires at ms operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_quarantine_set_expires_at_ms(UmiTestRuntimeQuarantine *value,uint64_t number);
/**
 * Return the number of records represented by test runtime quarantine set issue without
 * changing their state.
 */
UmiStatus umi_test_runtime_quarantine_set_issue_count(UmiTestRuntimeQuarantine *value,uint64_t number);
/**
 * Provide the test runtime quarantine set active operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_quarantine_set_active(UmiTestRuntimeQuarantine *value,bool active);
/**
 * Provide the test runtime quarantine same identity operation used by this module and its
 * client applications.
 */
bool umi_test_runtime_quarantine_same_identity(const UmiTestRuntimeQuarantine *left,const UmiTestRuntimeQuarantine *right);
#ifdef __cplusplus
}
#endif
#endif
