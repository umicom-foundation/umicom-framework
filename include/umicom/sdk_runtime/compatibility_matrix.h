/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/compatibility_matrix.h
 *
 * PURPOSE:
 *   Retain component-to-ABI compatibility evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_COMPATIBILITY_MATRIX
#define UMICOM_SDK_RUNTIME_COMPATIBILITY_MATRIX
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime compatibility matrix data shared with callers of this public
 * contract.
 */
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
/**
 * Initialise sdk runtime compatibility matrix from caller-provided values so later
 * operations receive a known state.
 */
void umi_sdk_runtime_compatibility_matrix_init(UmiSdkRuntimeCompatibilityMatrix *value,const char *id);
/**
 * Check that sdk runtime compatibility matrix satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_sdk_runtime_compatibility_matrix_validate(const UmiSdkRuntimeCompatibilityMatrix *value);
/**
 * Provide the sdk runtime compatibility matrix set path operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_compatibility_matrix_set_path(UmiSdkRuntimeCompatibilityMatrix *value,const char *path);
/**
 * Provide the sdk runtime compatibility matrix set detail operation used by this module
 * and its client applications.
 */
UmiStatus umi_sdk_runtime_compatibility_matrix_set_detail(UmiSdkRuntimeCompatibilityMatrix *value,const char *detail);
/**
 * Return the number of records represented by sdk runtime compatibility matrix set row
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_compatibility_matrix_set_row_count(UmiSdkRuntimeCompatibilityMatrix *value,uint64_t number);
/**
 * Return the number of records represented by sdk runtime compatibility matrix set failure
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_compatibility_matrix_set_failure_count(UmiSdkRuntimeCompatibilityMatrix *value,uint64_t number);
/**
 * Provide the sdk runtime compatibility matrix set state operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_compatibility_matrix_set_state(UmiSdkRuntimeCompatibilityMatrix *value,UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime compatibility matrix same identity operation used by this module
 * and its client applications.
 */
bool umi_sdk_runtime_compatibility_matrix_same_identity(const UmiSdkRuntimeCompatibilityMatrix *left,const UmiSdkRuntimeCompatibilityMatrix *right);
#ifdef __cplusplus
}
#endif
#endif
