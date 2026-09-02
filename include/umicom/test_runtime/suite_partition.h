/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/suite_partition.h
 *
 * PURPOSE:
 *   Partition regression work by subsystem while preserving stable ordering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_SUITE_PARTITION
#define UMICOM_TEST_RUNTIME_SUITE_PARTITION
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime suite partition data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeSuitePartition {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t partition_count;
    uint64_t member_count;
    uint64_t revision;
    bool active;
} UmiTestRuntimeSuitePartition;
/**
 * Initialise test runtime suite partition from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_suite_partition_init(UmiTestRuntimeSuitePartition *value,const char *id);
/**
 * Check that test runtime suite partition satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_suite_partition_validate(const UmiTestRuntimeSuitePartition *value);
/**
 * Provide the test runtime suite partition set category operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_suite_partition_set_category(UmiTestRuntimeSuitePartition *value,const char *category);
/**
 * Provide the test runtime suite partition set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_suite_partition_set_detail(UmiTestRuntimeSuitePartition *value,const char *detail);
/**
 * Return the number of records represented by test runtime suite partition set partition
 * without changing their state.
 */
UmiStatus umi_test_runtime_suite_partition_set_partition_count(UmiTestRuntimeSuitePartition *value,uint64_t number);
/**
 * Return the number of records represented by test runtime suite partition set member
 * without changing their state.
 */
UmiStatus umi_test_runtime_suite_partition_set_member_count(UmiTestRuntimeSuitePartition *value,uint64_t number);
/**
 * Provide the test runtime suite partition set active operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_suite_partition_set_active(UmiTestRuntimeSuitePartition *value,bool active);
/**
 * Provide the test runtime suite partition same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_suite_partition_same_identity(const UmiTestRuntimeSuitePartition *left,const UmiTestRuntimeSuitePartition *right);
#ifdef __cplusplus
}
#endif
#endif
