/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/execution_snapshot.h
 *
 * PURPOSE:
 *   Capture immutable launch, runtime and result evidence for one test.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_EXECUTION_SNAPSHOT
#define UMICOM_TEST_RUNTIME_EXECUTION_SNAPSHOT
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeExecutionSnapshot {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t evidence_count;
    uint64_t generation;
    uint64_t revision;
    bool active;
} UmiTestRuntimeExecutionSnapshot;
void umi_test_runtime_execution_snapshot_init(UmiTestRuntimeExecutionSnapshot *value,const char *id);
UmiStatus umi_test_runtime_execution_snapshot_validate(const UmiTestRuntimeExecutionSnapshot *value);
UmiStatus umi_test_runtime_execution_snapshot_set_category(UmiTestRuntimeExecutionSnapshot *value,const char *category);
UmiStatus umi_test_runtime_execution_snapshot_set_detail(UmiTestRuntimeExecutionSnapshot *value,const char *detail);
UmiStatus umi_test_runtime_execution_snapshot_set_evidence_count(UmiTestRuntimeExecutionSnapshot *value,uint64_t number);
UmiStatus umi_test_runtime_execution_snapshot_set_generation(UmiTestRuntimeExecutionSnapshot *value,uint64_t number);
UmiStatus umi_test_runtime_execution_snapshot_set_active(UmiTestRuntimeExecutionSnapshot *value,bool active);
bool umi_test_runtime_execution_snapshot_same_identity(const UmiTestRuntimeExecutionSnapshot *left,const UmiTestRuntimeExecutionSnapshot *right);
#ifdef __cplusplus
}
#endif
#endif
