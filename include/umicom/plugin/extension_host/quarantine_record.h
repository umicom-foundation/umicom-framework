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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiPluginExtensionHostQuarantineRecord { char plugin_id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY]; char reason[UMI_PLUGIN_EXTENSION_HOST_TEXT_CAPACITY]; uint64_t since_ms; uint64_t evidence; int active; int recovery_allowed; } UmiPluginExtensionHostQuarantineRecord;
void umi_plugin_extension_host_quarantine_record_init(UmiPluginExtensionHostQuarantineRecord *record);
UmiStatus umi_plugin_extension_host_quarantine_record_enter(UmiPluginExtensionHostQuarantineRecord *record, const char *plugin_id, const char *reason, uint64_t since_ms, uint64_t evidence, int recovery_allowed);
UmiStatus umi_plugin_extension_host_quarantine_record_clear(UmiPluginExtensionHostQuarantineRecord *record);

#ifdef __cplusplus
}
#endif

#endif
