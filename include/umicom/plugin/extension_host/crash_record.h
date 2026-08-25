/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/crash_record.h
 *
 * PURPOSE:
 *   Capture one extension crash with deterministic failure evidence.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_CRASH_RECORD_H
#define UMICOM_PLUGIN_EXTENSION_HOST_CRASH_RECORD_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostCrashRecord { char plugin_id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY]; uint64_t timestamp_ms; int32_t exit_code; uint32_t signal_code; uint32_t restart_count; uint64_t evidence; } UmiPluginExtensionHostCrashRecord;
void umi_plugin_extension_host_crash_record_init(UmiPluginExtensionHostCrashRecord *record);
UmiStatus umi_plugin_extension_host_crash_record_capture(UmiPluginExtensionHostCrashRecord *record, const char *plugin_id, uint64_t timestamp_ms, int32_t exit_code, uint32_t signal_code, uint32_t restart_count);
int umi_plugin_extension_host_crash_record_severe(const UmiPluginExtensionHostCrashRecord *record);

#ifdef __cplusplus
}
#endif

#endif
