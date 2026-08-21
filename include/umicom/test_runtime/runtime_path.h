/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/runtime_path.h
 *
 * PURPOSE:
 *   Retain one normalised runtime search path with provenance.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_RUNTIME_PATH
#define UMICOM_TEST_RUNTIME_RUNTIME_PATH
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeRuntimePath {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t priority;
    uint64_t source_kind;
    uint64_t revision;
    bool active;
} UmiTestRuntimeRuntimePath;
void umi_test_runtime_runtime_path_init(UmiTestRuntimeRuntimePath *value,const char *id);
UmiStatus umi_test_runtime_runtime_path_validate(const UmiTestRuntimeRuntimePath *value);
UmiStatus umi_test_runtime_runtime_path_set_category(UmiTestRuntimeRuntimePath *value,const char *category);
UmiStatus umi_test_runtime_runtime_path_set_detail(UmiTestRuntimeRuntimePath *value,const char *detail);
UmiStatus umi_test_runtime_runtime_path_set_priority(UmiTestRuntimeRuntimePath *value,uint64_t number);
UmiStatus umi_test_runtime_runtime_path_set_source_kind(UmiTestRuntimeRuntimePath *value,uint64_t number);
UmiStatus umi_test_runtime_runtime_path_set_active(UmiTestRuntimeRuntimePath *value,bool active);
bool umi_test_runtime_runtime_path_same_identity(const UmiTestRuntimeRuntimePath *left,const UmiTestRuntimeRuntimePath *right);
#ifdef __cplusplus
}
#endif
#endif
