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
/**
 * Represent the test runtime exit interpretation data shared with callers of this public
 * contract.
 */
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
/**
 * Initialise test runtime exit interpretation from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_exit_interpretation_init(UmiTestRuntimeExitInterpretation *value,const char *id);
/**
 * Check that test runtime exit interpretation satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_test_runtime_exit_interpretation_validate(const UmiTestRuntimeExitInterpretation *value);
/**
 * Provide the test runtime exit interpretation set category operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_exit_interpretation_set_category(UmiTestRuntimeExitInterpretation *value,const char *category);
/**
 * Provide the test runtime exit interpretation set detail operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_exit_interpretation_set_detail(UmiTestRuntimeExitInterpretation *value,const char *detail);
/**
 * Provide the test runtime exit interpretation set exit code operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_exit_interpretation_set_exit_code(UmiTestRuntimeExitInterpretation *value,uint64_t number);
/**
 * Provide the test runtime exit interpretation set native status operation used by this
 * module and its client applications.
 */
UmiStatus umi_test_runtime_exit_interpretation_set_native_status(UmiTestRuntimeExitInterpretation *value,uint64_t number);
/**
 * Provide the test runtime exit interpretation set active operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_exit_interpretation_set_active(UmiTestRuntimeExitInterpretation *value,bool active);
/**
 * Provide the test runtime exit interpretation same identity operation used by this module
 * and its client applications.
 */
bool umi_test_runtime_exit_interpretation_same_identity(const UmiTestRuntimeExitInterpretation *left,const UmiTestRuntimeExitInterpretation *right);
#ifdef __cplusplus
}
#endif
#endif
