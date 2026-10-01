/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/compiler_runtime.h
 *
 * PURPOSE:
 *   Describe compiler runtime libraries required by installed binaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_COMPILER_RUNTIME
#define UMICOM_SDK_RUNTIME_COMPILER_RUNTIME
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime compiler runtime data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimeCompilerRuntime
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t dependency_count;
    uint64_t missing_count;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
    } UmiSdkRuntimeCompilerRuntime;
/**
 * Initialise sdk runtime compiler runtime from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_compiler_runtime_init(UmiSdkRuntimeCompilerRuntime *value,const char *id);
/**
 * Check that sdk runtime compiler runtime satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_compiler_runtime_validate(const UmiSdkRuntimeCompilerRuntime *value);
/**
 * Provide the sdk runtime compiler runtime set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_compiler_runtime_set_path(UmiSdkRuntimeCompilerRuntime *value,const char *path);
/**
 * Provide the sdk runtime compiler runtime set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_compiler_runtime_set_detail(UmiSdkRuntimeCompilerRuntime *value,const char *detail);
/**
 * Return the number of records represented by sdk runtime compiler runtime set dependency
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_compiler_runtime_set_dependency_count(UmiSdkRuntimeCompilerRuntime *value,uint64_t number);
/**
 * Return the number of records represented by sdk runtime compiler runtime set missing
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_compiler_runtime_set_missing_count(UmiSdkRuntimeCompilerRuntime *value,uint64_t number);
/**
 * Provide the sdk runtime compiler runtime set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_compiler_runtime_set_state(UmiSdkRuntimeCompilerRuntime *value,UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime compiler runtime same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_compiler_runtime_same_identity(const UmiSdkRuntimeCompilerRuntime *left,const UmiSdkRuntimeCompilerRuntime *right);
/* Text setters publish a complete field and one revision together. Capacity,
 * invalid-input and exhausted-revision refusals preserve the record. Scalar
 * setters also refuse revision exhaustion. Call these on the value's owner;
 * they do not supply locking or persist changes to a storage service. */

/** Construct the usual default value and report invalid input.
 * A null or empty identity returns INVALID_ARGUMENT. An identity without a
 * terminator in sizeof(value->id) readable bytes returns CAPACITY_EXCEEDED;
 * a shorter C string is read only through its terminator. The existing domain
 * validator checks the staged defaults before publication. Any refusal leaves
 * the destination unchanged. id may refer to the destination's own text.
 * This initializes a new value, resetting its fields and revision to the
 * established defaults; do not use it as a live edit while observers retain
 * that identity. It owns no resources, allocates nothing and performs no I/O.
 * Existing void initialization remains available for compatibility. */
UmiStatus umi_sdk_runtime_compiler_runtime_init_checked(UmiSdkRuntimeCompilerRuntime *value, const char *id);

#ifdef __cplusplus
}
#endif
#endif
