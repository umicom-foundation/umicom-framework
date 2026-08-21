/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/log_capture.h
 *
 * PURPOSE:
 *   Retain stdout/stderr capture metadata and truncation evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_LOG_CAPTURE
#define UMICOM_TEST_RUNTIME_LOG_CAPTURE
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeLogCapture {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t byte_count;
    uint64_t truncated;
    uint64_t revision;
    bool active;
} UmiTestRuntimeLogCapture;
void umi_test_runtime_log_capture_init(UmiTestRuntimeLogCapture *value,const char *id);
UmiStatus umi_test_runtime_log_capture_validate(const UmiTestRuntimeLogCapture *value);
UmiStatus umi_test_runtime_log_capture_set_category(UmiTestRuntimeLogCapture *value,const char *category);
UmiStatus umi_test_runtime_log_capture_set_detail(UmiTestRuntimeLogCapture *value,const char *detail);
UmiStatus umi_test_runtime_log_capture_set_byte_count(UmiTestRuntimeLogCapture *value,uint64_t number);
UmiStatus umi_test_runtime_log_capture_set_truncated(UmiTestRuntimeLogCapture *value,uint64_t number);
UmiStatus umi_test_runtime_log_capture_set_active(UmiTestRuntimeLogCapture *value,bool active);
bool umi_test_runtime_log_capture_same_identity(const UmiTestRuntimeLogCapture *left,const UmiTestRuntimeLogCapture *right);
#ifdef __cplusplus
}
#endif
#endif
