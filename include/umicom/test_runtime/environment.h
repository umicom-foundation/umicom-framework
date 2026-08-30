/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/environment.h
 *
 * PURPOSE:
 *   Retain the inherited and explicit process environment used to launch tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_ENVIRONMENT
#define UMICOM_TEST_RUNTIME_ENVIRONMENT

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestRuntimeEnvironment {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t entry_count;
    uint64_t generation;
    uint64_t revision;
    uint64_t updated_at_ms;
    bool enabled;
} UmiTestRuntimeEnvironment;

void umi_test_runtime_environment_init(UmiTestRuntimeEnvironment *value, const char *id);
UmiStatus umi_test_runtime_environment_validate(const UmiTestRuntimeEnvironment *value);
UmiStatus umi_test_runtime_environment_set_name(UmiTestRuntimeEnvironment *value, const char *name);
UmiStatus umi_test_runtime_environment_set_detail(UmiTestRuntimeEnvironment *value, const char *detail);
UmiStatus umi_test_runtime_environment_set_entry_count(UmiTestRuntimeEnvironment *value, uint64_t number);
UmiStatus umi_test_runtime_environment_set_generation(UmiTestRuntimeEnvironment *value, uint64_t number);
UmiStatus umi_test_runtime_environment_touch(UmiTestRuntimeEnvironment *value, uint64_t updated_at_ms);
bool umi_test_runtime_environment_same_identity(const UmiTestRuntimeEnvironment *left, const UmiTestRuntimeEnvironment *right);

#ifdef __cplusplus
}
#endif
#endif
