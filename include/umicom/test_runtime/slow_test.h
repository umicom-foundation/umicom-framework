/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/slow_test.h
 *
 * PURPOSE:
 *   Identify tests whose duration exceeds their historical operating envelope.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_SLOW_TEST
#define UMICOM_TEST_RUNTIME_SLOW_TEST
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeSlowTest {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t duration_ms;
    uint64_t threshold_ms;
    uint64_t revision;
    bool active;
} UmiTestRuntimeSlowTest;
void umi_test_runtime_slow_test_init(UmiTestRuntimeSlowTest *value,const char *id);
UmiStatus umi_test_runtime_slow_test_validate(const UmiTestRuntimeSlowTest *value);
UmiStatus umi_test_runtime_slow_test_set_category(UmiTestRuntimeSlowTest *value,const char *category);
UmiStatus umi_test_runtime_slow_test_set_detail(UmiTestRuntimeSlowTest *value,const char *detail);
UmiStatus umi_test_runtime_slow_test_set_duration_ms(UmiTestRuntimeSlowTest *value,uint64_t number);
UmiStatus umi_test_runtime_slow_test_set_threshold_ms(UmiTestRuntimeSlowTest *value,uint64_t number);
UmiStatus umi_test_runtime_slow_test_set_active(UmiTestRuntimeSlowTest *value,bool active);
bool umi_test_runtime_slow_test_same_identity(const UmiTestRuntimeSlowTest *left,const UmiTestRuntimeSlowTest *right);
#ifdef __cplusplus
}
#endif
#endif
