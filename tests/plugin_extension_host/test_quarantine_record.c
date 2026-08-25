/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_quarantine_record.c
 *
 * PURPOSE:
 *   Exercise capture quarantine state, reason and recovery eligibility.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/quarantine_record.h"
int main(void) { UmiPluginExtensionHostQuarantineRecord r; if(umi_plugin_extension_host_quarantine_record_enter(&r,"p","crash-loop",1U,2U,1)!=UMI_STATUS_OK||!r.active) return 1; if(umi_plugin_extension_host_quarantine_record_clear(&r)!=UMI_STATUS_OK||r.active) return 2; return 0; }
