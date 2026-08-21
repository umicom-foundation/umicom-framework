/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/duration_budget.h
 *
 * PURPOSE:
 *   Retain expected and observed duration budgets for one test family.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_DURATION_BUDGET
#define UMICOM_TEST_RUNTIME_DURATION_BUDGET
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeDurationBudget {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t expected_ms;
    uint64_t maximum_ms;
    uint64_t revision;
    bool active;
} UmiTestRuntimeDurationBudget;
void umi_test_runtime_duration_budget_init(UmiTestRuntimeDurationBudget *value,const char *id);
UmiStatus umi_test_runtime_duration_budget_validate(const UmiTestRuntimeDurationBudget *value);
UmiStatus umi_test_runtime_duration_budget_set_category(UmiTestRuntimeDurationBudget *value,const char *category);
UmiStatus umi_test_runtime_duration_budget_set_detail(UmiTestRuntimeDurationBudget *value,const char *detail);
UmiStatus umi_test_runtime_duration_budget_set_expected_ms(UmiTestRuntimeDurationBudget *value,uint64_t number);
UmiStatus umi_test_runtime_duration_budget_set_maximum_ms(UmiTestRuntimeDurationBudget *value,uint64_t number);
UmiStatus umi_test_runtime_duration_budget_set_active(UmiTestRuntimeDurationBudget *value,bool active);
bool umi_test_runtime_duration_budget_same_identity(const UmiTestRuntimeDurationBudget *left,const UmiTestRuntimeDurationBudget *right);
#ifdef __cplusplus
}
#endif
#endif
