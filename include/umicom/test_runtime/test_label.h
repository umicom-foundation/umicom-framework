/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/test_label.h
 *
 * PURPOSE:
 *   Describe one stable label used by execution profiles and filtering.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_TEST_LABEL
#define UMICOM_TEST_RUNTIME_TEST_LABEL
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeTestLabel {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t member_count;
    uint64_t generation;
    uint64_t revision;
    bool active;
} UmiTestRuntimeTestLabel;
void umi_test_runtime_test_label_init(UmiTestRuntimeTestLabel *value,const char *id);
UmiStatus umi_test_runtime_test_label_validate(const UmiTestRuntimeTestLabel *value);
UmiStatus umi_test_runtime_test_label_set_category(UmiTestRuntimeTestLabel *value,const char *category);
UmiStatus umi_test_runtime_test_label_set_detail(UmiTestRuntimeTestLabel *value,const char *detail);
UmiStatus umi_test_runtime_test_label_set_member_count(UmiTestRuntimeTestLabel *value,uint64_t number);
UmiStatus umi_test_runtime_test_label_set_generation(UmiTestRuntimeTestLabel *value,uint64_t number);
UmiStatus umi_test_runtime_test_label_set_active(UmiTestRuntimeTestLabel *value,bool active);
bool umi_test_runtime_test_label_same_identity(const UmiTestRuntimeTestLabel *left,const UmiTestRuntimeTestLabel *right);
#ifdef __cplusplus
}
#endif
#endif
