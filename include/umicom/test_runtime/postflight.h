/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/postflight.h
 *
 * PURPOSE:
 *   Verify cleanup, evidence capture and resource release after tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_POSTFLIGHT
#define UMICOM_TEST_RUNTIME_POSTFLIGHT
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimePostflight
{
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t check_count;
    uint64_t failure_count;
    uint64_t revision;
    bool enabled;
    } UmiTestRuntimePostflight;
void umi_test_runtime_postflight_init(UmiTestRuntimePostflight *value,const char *id);
UmiStatus umi_test_runtime_postflight_validate(const UmiTestRuntimePostflight *value);
UmiStatus umi_test_runtime_postflight_set_detail(UmiTestRuntimePostflight *value,const char *detail);
UmiStatus umi_test_runtime_postflight_set_check_count(UmiTestRuntimePostflight *value,uint64_t number);
UmiStatus umi_test_runtime_postflight_set_failure_count(UmiTestRuntimePostflight *value,uint64_t number);
bool umi_test_runtime_postflight_same_identity(const UmiTestRuntimePostflight *left,const UmiTestRuntimePostflight *right);
#ifdef __cplusplus
}
#endif
#endif
