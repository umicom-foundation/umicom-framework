/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/start_failure.h
 *
 * PURPOSE:
 *   Describe process-creation failures before application code begins.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_START_FAILURE
#define UMICOM_TEST_RUNTIME_START_FAILURE
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeStartFailure {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t native_status;
    uint64_t attempt_count;
    uint64_t revision;
    bool active;
} UmiTestRuntimeStartFailure;
void umi_test_runtime_start_failure_init(UmiTestRuntimeStartFailure *value,const char *id);
UmiStatus umi_test_runtime_start_failure_validate(const UmiTestRuntimeStartFailure *value);
UmiStatus umi_test_runtime_start_failure_set_category(UmiTestRuntimeStartFailure *value,const char *category);
UmiStatus umi_test_runtime_start_failure_set_detail(UmiTestRuntimeStartFailure *value,const char *detail);
UmiStatus umi_test_runtime_start_failure_set_native_status(UmiTestRuntimeStartFailure *value,uint64_t number);
UmiStatus umi_test_runtime_start_failure_set_attempt_count(UmiTestRuntimeStartFailure *value,uint64_t number);
UmiStatus umi_test_runtime_start_failure_set_active(UmiTestRuntimeStartFailure *value,bool active);
bool umi_test_runtime_start_failure_same_identity(const UmiTestRuntimeStartFailure *left,const UmiTestRuntimeStartFailure *right);
#ifdef __cplusplus
}
#endif
#endif
