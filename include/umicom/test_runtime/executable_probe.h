/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/executable_probe.h
 *
 * PURPOSE:
 *   Record executable discovery, file existence and launch readiness.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_EXECUTABLE_PROBE
#define UMICOM_TEST_RUNTIME_EXECUTABLE_PROBE

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestRuntimeExecutableProbe {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t exists;
    uint64_t launchable;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeExecutableProbe;

void umi_test_runtime_executable_probe_init(UmiTestRuntimeExecutableProbe *value, const char *id);
UmiStatus umi_test_runtime_executable_probe_validate(const UmiTestRuntimeExecutableProbe *value);
UmiStatus umi_test_runtime_executable_probe_set_name(UmiTestRuntimeExecutableProbe *value, const char *name);
UmiStatus umi_test_runtime_executable_probe_set_detail(UmiTestRuntimeExecutableProbe *value, const char *detail);
UmiStatus umi_test_runtime_executable_probe_set_exists(UmiTestRuntimeExecutableProbe *value, uint64_t number);
UmiStatus umi_test_runtime_executable_probe_set_launchable(UmiTestRuntimeExecutableProbe *value, uint64_t number);
UmiStatus umi_test_runtime_executable_probe_touch(UmiTestRuntimeExecutableProbe *value, uint64_t updated_at_ms);
bool umi_test_runtime_executable_probe_same_identity(const UmiTestRuntimeExecutableProbe *left, const UmiTestRuntimeExecutableProbe *right);

#ifdef __cplusplus
}
#endif
#endif
