/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/install_probe.h
 *
 * PURPOSE:
 *   Probe an installed prefix and collect missing component evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_INSTALL_PROBE
#define UMICOM_SDK_RUNTIME_INSTALL_PROBE
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeInstallProbe {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t checked_count;
    uint64_t missing_count;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeInstallProbe;
void umi_sdk_runtime_install_probe_init(UmiSdkRuntimeInstallProbe *value, const char *id);
UmiStatus umi_sdk_runtime_install_probe_validate(const UmiSdkRuntimeInstallProbe *value);
UmiStatus umi_sdk_runtime_install_probe_set_path(UmiSdkRuntimeInstallProbe *value, const char *path);
UmiStatus umi_sdk_runtime_install_probe_set_detail(UmiSdkRuntimeInstallProbe *value, const char *detail);
UmiStatus umi_sdk_runtime_install_probe_set_checked_count(UmiSdkRuntimeInstallProbe *value, uint64_t number);
UmiStatus umi_sdk_runtime_install_probe_set_missing_count(UmiSdkRuntimeInstallProbe *value, uint64_t number);
UmiStatus umi_sdk_runtime_install_probe_set_state(UmiSdkRuntimeInstallProbe *value, UmiSdkRuntimeState state);
bool umi_sdk_runtime_install_probe_same_identity(const UmiSdkRuntimeInstallProbe *left, const UmiSdkRuntimeInstallProbe *right);
#ifdef __cplusplus
}
#endif
#endif
