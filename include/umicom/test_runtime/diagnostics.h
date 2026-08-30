/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/diagnostics.h
 *
 * PURPOSE:
 *   Maintain a bounded collection of runtime diagnostics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_DIAGNOSTICS
#define UMICOM_TEST_RUNTIME_DIAGNOSTICS

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestRuntimeDiagnostics {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t diagnostic_count;
    uint64_t generation;
    uint64_t revision;
    uint64_t updated_at_ms;
    bool enabled;
} UmiTestRuntimeDiagnostics;

void umi_test_runtime_diagnostics_init(UmiTestRuntimeDiagnostics *value, const char *id);
UmiStatus umi_test_runtime_diagnostics_validate(const UmiTestRuntimeDiagnostics *value);
UmiStatus umi_test_runtime_diagnostics_set_name(UmiTestRuntimeDiagnostics *value, const char *name);
UmiStatus umi_test_runtime_diagnostics_set_detail(UmiTestRuntimeDiagnostics *value, const char *detail);
UmiStatus umi_test_runtime_diagnostics_set_diagnostic_count(UmiTestRuntimeDiagnostics *value, uint64_t number);
UmiStatus umi_test_runtime_diagnostics_set_generation(UmiTestRuntimeDiagnostics *value, uint64_t number);
UmiStatus umi_test_runtime_diagnostics_touch(UmiTestRuntimeDiagnostics *value, uint64_t updated_at_ms);
bool umi_test_runtime_diagnostics_same_identity(const UmiTestRuntimeDiagnostics *left, const UmiTestRuntimeDiagnostics *right);

#ifdef __cplusplus
}
#endif
#endif
