/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/diagnostic.h
 *
 * PURPOSE:
 *   Represent actionable test-runtime diagnostic evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_DIAGNOSTIC
#define UMICOM_TEST_RUNTIME_DIAGNOSTIC

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestRuntimeDiagnostic {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t severity;
    uint64_t sequence;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeDiagnostic;

void umi_test_runtime_diagnostic_init(UmiTestRuntimeDiagnostic *value, const char *id);
UmiStatus umi_test_runtime_diagnostic_validate(const UmiTestRuntimeDiagnostic *value);
UmiStatus umi_test_runtime_diagnostic_set_name(UmiTestRuntimeDiagnostic *value, const char *name);
UmiStatus umi_test_runtime_diagnostic_set_detail(UmiTestRuntimeDiagnostic *value, const char *detail);
UmiStatus umi_test_runtime_diagnostic_set_severity(UmiTestRuntimeDiagnostic *value, uint64_t number);
UmiStatus umi_test_runtime_diagnostic_set_sequence(UmiTestRuntimeDiagnostic *value, uint64_t number);
UmiStatus umi_test_runtime_diagnostic_touch(UmiTestRuntimeDiagnostic *value, uint64_t updated_at_ms);
bool umi_test_runtime_diagnostic_same_identity(const UmiTestRuntimeDiagnostic *left, const UmiTestRuntimeDiagnostic *right);

#ifdef __cplusplus
}
#endif
#endif
