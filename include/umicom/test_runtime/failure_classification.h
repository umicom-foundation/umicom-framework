/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/failure_classification.h
 *
 * PURPOSE:
 *   Classify CTest start failures, crashes, timeouts and ordinary exits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_FAILURE_CLASSIFICATION
#define UMICOM_TEST_RUNTIME_FAILURE_CLASSIFICATION

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestRuntimeFailureClassification {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t native_status;
    uint64_t exit_code;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeFailureClassification;

void umi_test_runtime_failure_classification_init(UmiTestRuntimeFailureClassification *value, const char *id);
UmiStatus umi_test_runtime_failure_classification_validate(const UmiTestRuntimeFailureClassification *value);
UmiStatus umi_test_runtime_failure_classification_set_name(UmiTestRuntimeFailureClassification *value, const char *name);
UmiStatus umi_test_runtime_failure_classification_set_detail(UmiTestRuntimeFailureClassification *value, const char *detail);
UmiStatus umi_test_runtime_failure_classification_set_native_status(UmiTestRuntimeFailureClassification *value, uint64_t number);
UmiStatus umi_test_runtime_failure_classification_set_exit_code(UmiTestRuntimeFailureClassification *value, uint64_t number);
UmiStatus umi_test_runtime_failure_classification_touch(UmiTestRuntimeFailureClassification *value, uint64_t updated_at_ms);
bool umi_test_runtime_failure_classification_same_identity(const UmiTestRuntimeFailureClassification *left, const UmiTestRuntimeFailureClassification *right);

#ifdef __cplusplus
}
#endif
#endif
