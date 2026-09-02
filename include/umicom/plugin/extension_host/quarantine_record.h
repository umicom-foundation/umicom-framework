/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/quarantine_record.h
 *
 * PURPOSE:
 *   Capture quarantine state, reason and recovery eligibility.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_QUARANTINE_RECORD_H
#define UMICOM_PLUGIN_EXTENSION_HOST_QUARANTINE_RECORD_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the plugin extension host quarantine record data shared with callers of this
 * public contract.
 */
typedef struct UmiPluginExtensionHostQuarantineRecord { char plugin_id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY]; char reason[UMI_PLUGIN_EXTENSION_HOST_TEXT_CAPACITY]; uint64_t since_ms; uint64_t evidence; int active; int recovery_allowed; } UmiPluginExtensionHostQuarantineRecord;
/**
 * Initialise plugin extension host quarantine record from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_quarantine_record_init(UmiPluginExtensionHostQuarantineRecord *record);
/**
 * Provide the plugin extension host quarantine record enter operation used by this module
 * and its client applications.
 */
UmiStatus umi_plugin_extension_host_quarantine_record_enter(UmiPluginExtensionHostQuarantineRecord *record, const char *plugin_id, const char *reason, uint64_t since_ms, uint64_t evidence, int recovery_allowed);
/**
 * Release or reset state held by plugin extension host quarantine record so the same
 * storage can be reused safely.
 */
UmiStatus umi_plugin_extension_host_quarantine_record_clear(UmiPluginExtensionHostQuarantineRecord *record);

#ifdef __cplusplus
}
#endif

#endif
