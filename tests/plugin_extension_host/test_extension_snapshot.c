/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_extension_snapshot.c
 *
 * PURPOSE:
 *   Exercise capture a machine-readable point-in-time extension platform health snapshot.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/extension_snapshot.h"
int main(void) { UmiPluginExtensionHostExtensionSnapshot s; if(umi_plugin_extension_host_extension_snapshot_capture(&s,10U,4U,3U,1U,0U,2U)!=UMI_STATUS_OK) return 1; if(s.health!=UMI_PLUGIN_EXTENSION_HOST_HEALTH_DEGRADED||umi_plugin_extension_host_extension_snapshot_fingerprint(&s)==0U) return 2; return 0; }
