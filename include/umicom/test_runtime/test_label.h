/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/test_label.h
 *
 * PURPOSE:
 *   Describe one stable label used by execution profiles and filtering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_TEST_LABEL
#define UMICOM_TEST_RUNTIME_TEST_LABEL
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime test label data shared with callers of this public contract.
 */
typedef struct UmiTestRuntimeTestLabel {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t member_count;
    uint64_t generation;
    uint64_t revision;
    bool active;
} UmiTestRuntimeTestLabel;
/**
 * Initialise test runtime test label from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_test_label_init(UmiTestRuntimeTestLabel *value,const char *id);
/**
 * Check that test runtime test label satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_test_runtime_test_label_validate(const UmiTestRuntimeTestLabel *value);
/**
 * Provide the test runtime test label set category operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_test_label_set_category(UmiTestRuntimeTestLabel *value,const char *category);
/**
 * Provide the test runtime test label set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_test_label_set_detail(UmiTestRuntimeTestLabel *value,const char *detail);
/**
 * Return the number of records represented by test runtime test label set member without
 * changing their state.
 */
UmiStatus umi_test_runtime_test_label_set_member_count(UmiTestRuntimeTestLabel *value,uint64_t number);
/**
 * Provide the test runtime test label set generation operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_test_label_set_generation(UmiTestRuntimeTestLabel *value,uint64_t number);
/**
 * Provide the test runtime test label set active operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_test_label_set_active(UmiTestRuntimeTestLabel *value,bool active);
/**
 * Provide the test runtime test label same identity operation used by this module and its
 * client applications.
 */
bool umi_test_runtime_test_label_same_identity(const UmiTestRuntimeTestLabel *left,const UmiTestRuntimeTestLabel *right);
#ifdef __cplusplus
}
#endif
#endif
