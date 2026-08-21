/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/compatibility_matrix.h
 *
 * PURPOSE:
 *   Retain component-to-ABI compatibility evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_COMPATIBILITY_MATRIX
#define UMICOM_SDK_RUNTIME_COMPATIBILITY_MATRIX
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeCompatibilityMatrix
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t row_count;
    uint64_t failure_count;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
    } UmiSdkRuntimeCompatibilityMatrix;
void umi_sdk_runtime_compatibility_matrix_init(UmiSdkRuntimeCompatibilityMatrix *value,const char *id);
UmiStatus umi_sdk_runtime_compatibility_matrix_validate(const UmiSdkRuntimeCompatibilityMatrix *value);
UmiStatus umi_sdk_runtime_compatibility_matrix_set_path(UmiSdkRuntimeCompatibilityMatrix *value,const char *path);
UmiStatus umi_sdk_runtime_compatibility_matrix_set_detail(UmiSdkRuntimeCompatibilityMatrix *value,const char *detail);
UmiStatus umi_sdk_runtime_compatibility_matrix_set_row_count(UmiSdkRuntimeCompatibilityMatrix *value,uint64_t number);
UmiStatus umi_sdk_runtime_compatibility_matrix_set_failure_count(UmiSdkRuntimeCompatibilityMatrix *value,uint64_t number);
UmiStatus umi_sdk_runtime_compatibility_matrix_set_state(UmiSdkRuntimeCompatibilityMatrix *value,UmiSdkRuntimeState state);
bool umi_sdk_runtime_compatibility_matrix_same_identity(const UmiSdkRuntimeCompatibilityMatrix *left,const UmiSdkRuntimeCompatibilityMatrix *right);
#ifdef __cplusplus
}
#endif
#endif
