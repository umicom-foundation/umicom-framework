/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/profile_health.h
 *
 * PURPOSE:
 *   Probe whether a configured DAP adapter executable is discoverable.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_PROFILE_HEALTH_H
#define UMICOM_DEBUG_RUNTIME_PROFILE_HEALTH_H
#include "umicom/debug/adapter_profile.h"
#include "umicom/debug_runtime/types.h"
#include "umicom/toolchain/discovery.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDebugRuntimeProfileHealth {
    char profile_id[UMI_DEBUG_RUNTIME_ID_CAPACITY];
    char executable[UMI_DEBUG_RUNTIME_PATH_CAPACITY];
    char resolved_path[UMI_DEBUG_RUNTIME_PATH_CAPACITY];
    int available;
    UmiStatus status;
} UmiDebugRuntimeProfileHealth;

UmiStatus umi_debug_runtime_profile_health_probe(
    const UmiDebugAdapterProfile *profile,
    UmiDebugRuntimeProfileHealth *out_health);
#ifdef __cplusplus
}
#endif
#endif
