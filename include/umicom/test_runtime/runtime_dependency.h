/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/runtime_dependency.h
 *
 * PURPOSE:
 *   Describe one dynamic runtime dependency and where it was resolved.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_RUNTIME_DEPENDENCY
#define UMICOM_TEST_RUNTIME_RUNTIME_DEPENDENCY

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestRuntimeRuntimeDependency {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t required;
    uint64_t resolved;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeRuntimeDependency;

void umi_test_runtime_runtime_dependency_init(UmiTestRuntimeRuntimeDependency *value, const char *id);
UmiStatus umi_test_runtime_runtime_dependency_validate(const UmiTestRuntimeRuntimeDependency *value);
UmiStatus umi_test_runtime_runtime_dependency_set_name(UmiTestRuntimeRuntimeDependency *value, const char *name);
UmiStatus umi_test_runtime_runtime_dependency_set_detail(UmiTestRuntimeRuntimeDependency *value, const char *detail);
UmiStatus umi_test_runtime_runtime_dependency_set_required(UmiTestRuntimeRuntimeDependency *value, uint64_t number);
UmiStatus umi_test_runtime_runtime_dependency_set_resolved(UmiTestRuntimeRuntimeDependency *value, uint64_t number);
UmiStatus umi_test_runtime_runtime_dependency_touch(UmiTestRuntimeRuntimeDependency *value, uint64_t updated_at_ms);
bool umi_test_runtime_runtime_dependency_same_identity(const UmiTestRuntimeRuntimeDependency *left, const UmiTestRuntimeRuntimeDependency *right);

#ifdef __cplusplus
}
#endif
#endif
