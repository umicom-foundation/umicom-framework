/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/parallel_budget.h
 *
 * PURPOSE:
 *   Define safe parallelism for constrained Windows and CI environments.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_PARALLEL_BUDGET
#define UMICOM_TEST_RUNTIME_PARALLEL_BUDGET
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeParallelBudget {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t maximum_parallel;
    uint64_t processor_cost;
    uint64_t revision;
    bool active;
} UmiTestRuntimeParallelBudget;
void umi_test_runtime_parallel_budget_init(UmiTestRuntimeParallelBudget *value,const char *id);
UmiStatus umi_test_runtime_parallel_budget_validate(const UmiTestRuntimeParallelBudget *value);
UmiStatus umi_test_runtime_parallel_budget_set_category(UmiTestRuntimeParallelBudget *value,const char *category);
UmiStatus umi_test_runtime_parallel_budget_set_detail(UmiTestRuntimeParallelBudget *value,const char *detail);
UmiStatus umi_test_runtime_parallel_budget_set_maximum_parallel(UmiTestRuntimeParallelBudget *value,uint64_t number);
UmiStatus umi_test_runtime_parallel_budget_set_processor_cost(UmiTestRuntimeParallelBudget *value,uint64_t number);
UmiStatus umi_test_runtime_parallel_budget_set_active(UmiTestRuntimeParallelBudget *value,bool active);
bool umi_test_runtime_parallel_budget_same_identity(const UmiTestRuntimeParallelBudget *left,const UmiTestRuntimeParallelBudget *right);
#ifdef __cplusplus
}
#endif
#endif
