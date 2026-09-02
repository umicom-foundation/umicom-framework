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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/quarantine_record.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPluginExtensionHostQuarantineRecord r; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_plugin_extension_host_quarantine_record_enter(&r,"p","crash-loop",1U,2U,1)!=UMI_STATUS_OK||!r.active) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_plugin_extension_host_quarantine_record_clear(&r)!=UMI_STATUS_OK||r.active) return 2; return 0; }
