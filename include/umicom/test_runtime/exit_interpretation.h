/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/exit_interpretation.h
 *
 * PURPOSE:
 *   Interpret normal exits separately from native crash status.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_EXIT_INTERPRETATION
#define UMICOM_TEST_RUNTIME_EXIT_INTERPRETATION
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeExitInterpretation {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t exit_code;
    uint64_t native_status;
    uint64_t revision;
    bool active;
} UmiTestRuntimeExitInterpretation;
void umi_test_runtime_exit_interpretation_init(UmiTestRuntimeExitInterpretation *value,const char *id);
UmiStatus umi_test_runtime_exit_interpretation_validate(const UmiTestRuntimeExitInterpretation *value);
UmiStatus umi_test_runtime_exit_interpretation_set_category(UmiTestRuntimeExitInterpretation *value,const char *category);
UmiStatus umi_test_runtime_exit_interpretation_set_detail(UmiTestRuntimeExitInterpretation *value,const char *detail);
UmiStatus umi_test_runtime_exit_interpretation_set_exit_code(UmiTestRuntimeExitInterpretation *value,uint64_t number);
UmiStatus umi_test_runtime_exit_interpretation_set_native_status(UmiTestRuntimeExitInterpretation *value,uint64_t number);
UmiStatus umi_test_runtime_exit_interpretation_set_active(UmiTestRuntimeExitInterpretation *value,bool active);
bool umi_test_runtime_exit_interpretation_same_identity(const UmiTestRuntimeExitInterpretation *left,const UmiTestRuntimeExitInterpretation *right);
#ifdef __cplusplus
}
#endif
#endif
