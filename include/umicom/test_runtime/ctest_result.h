/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/ctest_result.h
 *
 * PURPOSE:
 *   Represent one CTest outcome with native process evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_CTEST_RESULT
#define UMICOM_TEST_RUNTIME_CTEST_RESULT
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeCtestResult {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t duration_ms;
    uint64_t native_status;
    uint64_t revision;
    bool active;
} UmiTestRuntimeCtestResult;
void umi_test_runtime_ctest_result_init(UmiTestRuntimeCtestResult *value,const char *id);
UmiStatus umi_test_runtime_ctest_result_validate(const UmiTestRuntimeCtestResult *value);
UmiStatus umi_test_runtime_ctest_result_set_category(UmiTestRuntimeCtestResult *value,const char *category);
UmiStatus umi_test_runtime_ctest_result_set_detail(UmiTestRuntimeCtestResult *value,const char *detail);
UmiStatus umi_test_runtime_ctest_result_set_duration_ms(UmiTestRuntimeCtestResult *value,uint64_t number);
UmiStatus umi_test_runtime_ctest_result_set_native_status(UmiTestRuntimeCtestResult *value,uint64_t number);
UmiStatus umi_test_runtime_ctest_result_set_active(UmiTestRuntimeCtestResult *value,bool active);
bool umi_test_runtime_ctest_result_same_identity(const UmiTestRuntimeCtestResult *left,const UmiTestRuntimeCtestResult *right);
#ifdef __cplusplus
}
#endif
#endif
