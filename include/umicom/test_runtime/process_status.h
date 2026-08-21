/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/process_status.h
 *
 * PURPOSE:
 *   Retain native process status independently from test assertion outcome.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_PROCESS_STATUS
#define UMICOM_TEST_RUNTIME_PROCESS_STATUS
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeProcessStatus {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t native_status;
    uint64_t exit_code;
    uint64_t revision;
    bool active;
} UmiTestRuntimeProcessStatus;
void umi_test_runtime_process_status_init(UmiTestRuntimeProcessStatus *value,const char *id);
UmiStatus umi_test_runtime_process_status_validate(const UmiTestRuntimeProcessStatus *value);
UmiStatus umi_test_runtime_process_status_set_category(UmiTestRuntimeProcessStatus *value,const char *category);
UmiStatus umi_test_runtime_process_status_set_detail(UmiTestRuntimeProcessStatus *value,const char *detail);
UmiStatus umi_test_runtime_process_status_set_native_status(UmiTestRuntimeProcessStatus *value,uint64_t number);
UmiStatus umi_test_runtime_process_status_set_exit_code(UmiTestRuntimeProcessStatus *value,uint64_t number);
UmiStatus umi_test_runtime_process_status_set_active(UmiTestRuntimeProcessStatus *value,bool active);
bool umi_test_runtime_process_status_same_identity(const UmiTestRuntimeProcessStatus *left,const UmiTestRuntimeProcessStatus *right);
#ifdef __cplusplus
}
#endif
#endif
