/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/history.h
 *
 * PURPOSE:
 *   Retain regression-run history and baseline comparisons.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_HISTORY
#define UMICOM_TEST_RUNTIME_HISTORY

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestRuntimeHistory {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t run_count;
    uint64_t generation;
    uint64_t revision;
    uint64_t updated_at_ms;
    bool enabled;
} UmiTestRuntimeHistory;

void umi_test_runtime_history_init(UmiTestRuntimeHistory *value, const char *id);
UmiStatus umi_test_runtime_history_validate(const UmiTestRuntimeHistory *value);
UmiStatus umi_test_runtime_history_set_name(UmiTestRuntimeHistory *value, const char *name);
UmiStatus umi_test_runtime_history_set_detail(UmiTestRuntimeHistory *value, const char *detail);
UmiStatus umi_test_runtime_history_set_run_count(UmiTestRuntimeHistory *value, uint64_t number);
UmiStatus umi_test_runtime_history_set_generation(UmiTestRuntimeHistory *value, uint64_t number);
UmiStatus umi_test_runtime_history_touch(UmiTestRuntimeHistory *value, uint64_t updated_at_ms);
bool umi_test_runtime_history_same_identity(const UmiTestRuntimeHistory *left, const UmiTestRuntimeHistory *right);

#ifdef __cplusplus
}
#endif
#endif
