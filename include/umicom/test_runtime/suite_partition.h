/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/suite_partition.h
 *
 * PURPOSE:
 *   Partition regression work by subsystem while preserving stable ordering.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_SUITE_PARTITION
#define UMICOM_TEST_RUNTIME_SUITE_PARTITION
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
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
void umi_test_runtime_suite_partition_init(UmiTestRuntimeSuitePartition *value,const char *id);
UmiStatus umi_test_runtime_suite_partition_validate(const UmiTestRuntimeSuitePartition *value);
UmiStatus umi_test_runtime_suite_partition_set_category(UmiTestRuntimeSuitePartition *value,const char *category);
UmiStatus umi_test_runtime_suite_partition_set_detail(UmiTestRuntimeSuitePartition *value,const char *detail);
UmiStatus umi_test_runtime_suite_partition_set_partition_count(UmiTestRuntimeSuitePartition *value,uint64_t number);
UmiStatus umi_test_runtime_suite_partition_set_member_count(UmiTestRuntimeSuitePartition *value,uint64_t number);
UmiStatus umi_test_runtime_suite_partition_set_active(UmiTestRuntimeSuitePartition *value,bool active);
bool umi_test_runtime_suite_partition_same_identity(const UmiTestRuntimeSuitePartition *left,const UmiTestRuntimeSuitePartition *right);
#ifdef __cplusplus
}
#endif
#endif
