/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/clean_machine.h
 *
 * PURPOSE:
 *   Evaluate whether an application can start without the source checkout.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_CLEAN_MACHINE
#define UMICOM_SDK_RUNTIME_CLEAN_MACHINE
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime clean machine data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimeCleanMachine {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t requirement_count;
    uint64_t missing_count;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeCleanMachine;
/**
 * Initialise sdk runtime clean machine from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_clean_machine_init(UmiSdkRuntimeCleanMachine *value, const char *id);
/**
 * Check that sdk runtime clean machine satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_clean_machine_validate(const UmiSdkRuntimeCleanMachine *value);
/**
 * Provide the sdk runtime clean machine set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_clean_machine_set_path(UmiSdkRuntimeCleanMachine *value, const char *path);
/**
 * Provide the sdk runtime clean machine set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_clean_machine_set_detail(UmiSdkRuntimeCleanMachine *value, const char *detail);
/**
 * Return the number of records represented by sdk runtime clean machine set requirement
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_clean_machine_set_requirement_count(UmiSdkRuntimeCleanMachine *value, uint64_t number);
/**
 * Return the number of records represented by sdk runtime clean machine set missing
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_clean_machine_set_missing_count(UmiSdkRuntimeCleanMachine *value, uint64_t number);
/**
 * Provide the sdk runtime clean machine set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_clean_machine_set_state(UmiSdkRuntimeCleanMachine *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime clean machine same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_clean_machine_same_identity(const UmiSdkRuntimeCleanMachine *left, const UmiSdkRuntimeCleanMachine *right);
#ifdef __cplusplus
}
#endif
#endif
