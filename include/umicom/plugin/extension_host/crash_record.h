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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the plugin extension host crash record data shared with callers of this public
 * contract.
 */
typedef struct UmiPluginExtensionHostCrashRecord { char plugin_id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY]; uint64_t timestamp_ms; int32_t exit_code; uint32_t signal_code; uint32_t restart_count; uint64_t evidence; } UmiPluginExtensionHostCrashRecord;
/**
 * Initialise plugin extension host crash record from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_crash_record_init(UmiPluginExtensionHostCrashRecord *record);
/**
 * Provide the plugin extension host crash record capture operation used by this module and
 * its client applications.
 */
UmiStatus umi_plugin_extension_host_crash_record_capture(UmiPluginExtensionHostCrashRecord *record, const char *plugin_id, uint64_t timestamp_ms, int32_t exit_code, uint32_t signal_code, uint32_t restart_count);
/**
 * Provide the plugin extension host crash record severe operation used by this module and
 * its client applications.
 */
int umi_plugin_extension_host_crash_record_severe(const UmiPluginExtensionHostCrashRecord *record);

#ifdef __cplusplus
}
#endif

#endif
