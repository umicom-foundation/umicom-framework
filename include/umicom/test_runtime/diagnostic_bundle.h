/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/diagnostic_bundle.h
 *
 * PURPOSE:
 *   Group logs, environment, executable and dependency evidence for a failure.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_DIAGNOSTIC_BUNDLE
#define UMICOM_TEST_RUNTIME_DIAGNOSTIC_BUNDLE
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeDiagnosticBundle {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t artifact_count;
    uint64_t generation;
    uint64_t revision;
    bool active;
} UmiTestRuntimeDiagnosticBundle;
void umi_test_runtime_diagnostic_bundle_init(UmiTestRuntimeDiagnosticBundle *value,const char *id);
UmiStatus umi_test_runtime_diagnostic_bundle_validate(const UmiTestRuntimeDiagnosticBundle *value);
UmiStatus umi_test_runtime_diagnostic_bundle_set_category(UmiTestRuntimeDiagnosticBundle *value,const char *category);
UmiStatus umi_test_runtime_diagnostic_bundle_set_detail(UmiTestRuntimeDiagnosticBundle *value,const char *detail);
UmiStatus umi_test_runtime_diagnostic_bundle_set_artifact_count(UmiTestRuntimeDiagnosticBundle *value,uint64_t number);
UmiStatus umi_test_runtime_diagnostic_bundle_set_generation(UmiTestRuntimeDiagnosticBundle *value,uint64_t number);
UmiStatus umi_test_runtime_diagnostic_bundle_set_active(UmiTestRuntimeDiagnosticBundle *value,bool active);
bool umi_test_runtime_diagnostic_bundle_same_identity(const UmiTestRuntimeDiagnosticBundle *left,const UmiTestRuntimeDiagnosticBundle *right);
#ifdef __cplusplus
}
#endif
#endif
