/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/diagnostic_bundle.h
 *
 * PURPOSE:
 *   Group logs, environment, executable and dependency evidence for a failure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_DIAGNOSTIC_BUNDLE
#define UMICOM_TEST_RUNTIME_DIAGNOSTIC_BUNDLE
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime diagnostic bundle data shared with callers of this public
 * contract.
 */
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
/**
 * Initialise test runtime diagnostic bundle from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_diagnostic_bundle_init(UmiTestRuntimeDiagnosticBundle *value,const char *id);
/**
 * Check that test runtime diagnostic bundle satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_diagnostic_bundle_validate(const UmiTestRuntimeDiagnosticBundle *value);
/**
 * Provide the test runtime diagnostic bundle set category operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_diagnostic_bundle_set_category(UmiTestRuntimeDiagnosticBundle *value,const char *category);
/**
 * Provide the test runtime diagnostic bundle set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_diagnostic_bundle_set_detail(UmiTestRuntimeDiagnosticBundle *value,const char *detail);
/**
 * Return the number of records represented by test runtime diagnostic bundle set artifact
 * without changing their state.
 */
UmiStatus umi_test_runtime_diagnostic_bundle_set_artifact_count(UmiTestRuntimeDiagnosticBundle *value,uint64_t number);
/**
 * Provide the test runtime diagnostic bundle set generation operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_diagnostic_bundle_set_generation(UmiTestRuntimeDiagnosticBundle *value,uint64_t number);
/**
 * Provide the test runtime diagnostic bundle set active operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_diagnostic_bundle_set_active(UmiTestRuntimeDiagnosticBundle *value,bool active);
/**
 * Provide the test runtime diagnostic bundle same identity operation used by this module
 * and its client applications.
 */
bool umi_test_runtime_diagnostic_bundle_same_identity(const UmiTestRuntimeDiagnosticBundle *left,const UmiTestRuntimeDiagnosticBundle *right);
#ifdef __cplusplus
}
#endif
#endif
