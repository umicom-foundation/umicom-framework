/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/trend.h
 *
 * PURPOSE:
 *   Retain pass rate, duration and failure-category trends across runs.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_TREND
#define UMICOM_TEST_RUNTIME_TREND
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeTrend {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t sample_count;
    uint64_t generation;
    uint64_t revision;
    bool active;
} UmiTestRuntimeTrend;
void umi_test_runtime_trend_init(UmiTestRuntimeTrend *value,const char *id);
UmiStatus umi_test_runtime_trend_validate(const UmiTestRuntimeTrend *value);
UmiStatus umi_test_runtime_trend_set_category(UmiTestRuntimeTrend *value,const char *category);
UmiStatus umi_test_runtime_trend_set_detail(UmiTestRuntimeTrend *value,const char *detail);
UmiStatus umi_test_runtime_trend_set_sample_count(UmiTestRuntimeTrend *value,uint64_t number);
UmiStatus umi_test_runtime_trend_set_generation(UmiTestRuntimeTrend *value,uint64_t number);
UmiStatus umi_test_runtime_trend_set_active(UmiTestRuntimeTrend *value,bool active);
bool umi_test_runtime_trend_same_identity(const UmiTestRuntimeTrend *left,const UmiTestRuntimeTrend *right);
#ifdef __cplusplus
}
#endif
#endif
