/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/dependency_probe.h
 *
 * PURPOSE:
 *   Record runtime dependency discovery and missing-library evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_DEPENDENCY_PROBE
#define UMICOM_TEST_RUNTIME_DEPENDENCY_PROBE

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestRuntimeDependencyProbe {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t dependency_count;
    uint64_t missing_count;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeDependencyProbe;

void umi_test_runtime_dependency_probe_init(UmiTestRuntimeDependencyProbe *value, const char *id);
UmiStatus umi_test_runtime_dependency_probe_validate(const UmiTestRuntimeDependencyProbe *value);
UmiStatus umi_test_runtime_dependency_probe_set_name(UmiTestRuntimeDependencyProbe *value, const char *name);
UmiStatus umi_test_runtime_dependency_probe_set_detail(UmiTestRuntimeDependencyProbe *value, const char *detail);
UmiStatus umi_test_runtime_dependency_probe_set_dependency_count(UmiTestRuntimeDependencyProbe *value, uint64_t number);
UmiStatus umi_test_runtime_dependency_probe_set_missing_count(UmiTestRuntimeDependencyProbe *value, uint64_t number);
UmiStatus umi_test_runtime_dependency_probe_touch(UmiTestRuntimeDependencyProbe *value, uint64_t updated_at_ms);
bool umi_test_runtime_dependency_probe_same_identity(const UmiTestRuntimeDependencyProbe *left, const UmiTestRuntimeDependencyProbe *right);

#ifdef __cplusplus
}
#endif
#endif
