/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/diagnostic.h
 *
 * PURPOSE:
 *   Represent actionable SDK/runtime diagnostic evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_DIAGNOSTIC
#define UMICOM_SDK_RUNTIME_DIAGNOSTIC
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime diagnostic data shared with callers of this public contract.
 */
typedef struct UmiSdkRuntimeDiagnostic {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t severity;
    uint64_t sequence;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeDiagnostic;
/**
 * Initialise sdk runtime diagnostic from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_diagnostic_init(UmiSdkRuntimeDiagnostic *value, const char *id);
/**
 * Check that sdk runtime diagnostic satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_sdk_runtime_diagnostic_validate(const UmiSdkRuntimeDiagnostic *value);
/**
 * Provide the sdk runtime diagnostic set path operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_diagnostic_set_path(UmiSdkRuntimeDiagnostic *value, const char *path);
/**
 * Provide the sdk runtime diagnostic set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_diagnostic_set_detail(UmiSdkRuntimeDiagnostic *value, const char *detail);
/**
 * Provide the sdk runtime diagnostic set severity operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_diagnostic_set_severity(UmiSdkRuntimeDiagnostic *value, uint64_t number);
/**
 * Provide the sdk runtime diagnostic set sequence operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_diagnostic_set_sequence(UmiSdkRuntimeDiagnostic *value, uint64_t number);
/**
 * Provide the sdk runtime diagnostic set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_diagnostic_set_state(UmiSdkRuntimeDiagnostic *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime diagnostic same identity operation used by this module and its
 * client applications.
 */
bool umi_sdk_runtime_diagnostic_same_identity(const UmiSdkRuntimeDiagnostic *left, const UmiSdkRuntimeDiagnostic *right);
#ifdef __cplusplus
}
#endif
#endif
