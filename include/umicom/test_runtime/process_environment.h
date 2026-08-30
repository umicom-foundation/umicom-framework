/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/process_environment.h
 *
 * PURPOSE:
 *   Build the exact environment overlay inherited by child processes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_PROCESS_ENVIRONMENT
#define UMICOM_TEST_RUNTIME_PROCESS_ENVIRONMENT

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestRuntimeProcessEnvironment {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t entry_count;
    uint64_t path_entry_count;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeProcessEnvironment;

void umi_test_runtime_process_environment_init(UmiTestRuntimeProcessEnvironment *value, const char *id);
UmiStatus umi_test_runtime_process_environment_validate(const UmiTestRuntimeProcessEnvironment *value);
UmiStatus umi_test_runtime_process_environment_set_name(UmiTestRuntimeProcessEnvironment *value, const char *name);
UmiStatus umi_test_runtime_process_environment_set_detail(UmiTestRuntimeProcessEnvironment *value, const char *detail);
UmiStatus umi_test_runtime_process_environment_set_entry_count(UmiTestRuntimeProcessEnvironment *value, uint64_t number);
UmiStatus umi_test_runtime_process_environment_set_path_entry_count(UmiTestRuntimeProcessEnvironment *value, uint64_t number);
UmiStatus umi_test_runtime_process_environment_touch(UmiTestRuntimeProcessEnvironment *value, uint64_t updated_at_ms);
bool umi_test_runtime_process_environment_same_identity(const UmiTestRuntimeProcessEnvironment *left, const UmiTestRuntimeProcessEnvironment *right);

#ifdef __cplusplus
}
#endif
#endif
