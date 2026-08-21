/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/ctest_record.h
 *
 * PURPOSE:
 *   Represent one CTest registration independently of generated CTest files.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_CTEST_RECORD
#define UMICOM_TEST_RUNTIME_CTEST_RECORD
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
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
void umi_test_runtime_ctest_record_init(UmiTestRuntimeCtestRecord *value,const char *id);
UmiStatus umi_test_runtime_ctest_record_validate(const UmiTestRuntimeCtestRecord *value);
UmiStatus umi_test_runtime_ctest_record_set_category(UmiTestRuntimeCtestRecord *value,const char *category);
UmiStatus umi_test_runtime_ctest_record_set_detail(UmiTestRuntimeCtestRecord *value,const char *detail);
UmiStatus umi_test_runtime_ctest_record_set_processor_count(UmiTestRuntimeCtestRecord *value,uint64_t number);
UmiStatus umi_test_runtime_ctest_record_set_timeout_seconds(UmiTestRuntimeCtestRecord *value,uint64_t number);
UmiStatus umi_test_runtime_ctest_record_set_active(UmiTestRuntimeCtestRecord *value,bool active);
bool umi_test_runtime_ctest_record_same_identity(const UmiTestRuntimeCtestRecord *left,const UmiTestRuntimeCtestRecord *right);
#ifdef __cplusplus
}
#endif
#endif
