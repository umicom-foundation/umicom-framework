/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/environment_snapshot.h
 *
 * PURPOSE:
 *   Capture deterministic environment evidence for regression reproduction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_ENVIRONMENT_SNAPSHOT
#define UMICOM_TEST_RUNTIME_ENVIRONMENT_SNAPSHOT
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeEnvironmentSnapshot {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t entry_count;
    uint64_t generation;
    uint64_t revision;
    bool active;
} UmiTestRuntimeEnvironmentSnapshot;
void umi_test_runtime_environment_snapshot_init(UmiTestRuntimeEnvironmentSnapshot *value,const char *id);
UmiStatus umi_test_runtime_environment_snapshot_validate(const UmiTestRuntimeEnvironmentSnapshot *value);
UmiStatus umi_test_runtime_environment_snapshot_set_category(UmiTestRuntimeEnvironmentSnapshot *value,const char *category);
UmiStatus umi_test_runtime_environment_snapshot_set_detail(UmiTestRuntimeEnvironmentSnapshot *value,const char *detail);
UmiStatus umi_test_runtime_environment_snapshot_set_entry_count(UmiTestRuntimeEnvironmentSnapshot *value,uint64_t number);
UmiStatus umi_test_runtime_environment_snapshot_set_generation(UmiTestRuntimeEnvironmentSnapshot *value,uint64_t number);
UmiStatus umi_test_runtime_environment_snapshot_set_active(UmiTestRuntimeEnvironmentSnapshot *value,bool active);
bool umi_test_runtime_environment_snapshot_same_identity(const UmiTestRuntimeEnvironmentSnapshot *left,const UmiTestRuntimeEnvironmentSnapshot *right);
#ifdef __cplusplus
}
#endif
#endif
