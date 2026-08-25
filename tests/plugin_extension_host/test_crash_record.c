/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_crash_record.c
 *
 * PURPOSE:
 *   Exercise capture one extension crash with deterministic failure evidence.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/crash_record.h"
int main(void) { UmiPluginExtensionHostCrashRecord r; if(umi_plugin_extension_host_crash_record_capture(&r,"p",10U,-1,0U,0U)!=UMI_STATUS_OK) return 1; if(!umi_plugin_extension_host_crash_record_severe(&r)||r.evidence==0U) return 2; return 0; }
