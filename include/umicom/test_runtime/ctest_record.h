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
#ifdef __cplusplus
}
#endif
#endif
