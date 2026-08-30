/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/execution_batch.h
 *
 * PURPOSE:
 *   Track one bounded group of tests scheduled together.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_EXECUTION_BATCH
#define UMICOM_TEST_RUNTIME_EXECUTION_BATCH

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestRuntimeExecutionBatch {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t planned_count;
    uint64_t completed_count;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeExecutionBatch;

void umi_test_runtime_execution_batch_init(UmiTestRuntimeExecutionBatch *value, const char *id);
UmiStatus umi_test_runtime_execution_batch_validate(const UmiTestRuntimeExecutionBatch *value);
UmiStatus umi_test_runtime_execution_batch_set_name(UmiTestRuntimeExecutionBatch *value, const char *name);
UmiStatus umi_test_runtime_execution_batch_set_detail(UmiTestRuntimeExecutionBatch *value, const char *detail);
UmiStatus umi_test_runtime_execution_batch_set_planned_count(UmiTestRuntimeExecutionBatch *value, uint64_t number);
UmiStatus umi_test_runtime_execution_batch_set_completed_count(UmiTestRuntimeExecutionBatch *value, uint64_t number);
UmiStatus umi_test_runtime_execution_batch_touch(UmiTestRuntimeExecutionBatch *value, uint64_t updated_at_ms);
bool umi_test_runtime_execution_batch_same_identity(const UmiTestRuntimeExecutionBatch *left, const UmiTestRuntimeExecutionBatch *right);

#ifdef __cplusplus
}
#endif
#endif
