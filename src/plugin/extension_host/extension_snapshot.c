/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/extension_snapshot.c
 *
 * PURPOSE:
 *   Capture a machine-readable point-in-time extension platform health snapshot.
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
#include "umicom/plugin/extension_host/extension_snapshot.h"
#include <string.h>
void umi_plugin_extension_host_extension_snapshot_init(UmiPluginExtensionHostExtensionSnapshot *s) { if(s!=NULL) memset(s,0,sizeof(*s)); }
UmiStatus umi_plugin_extension_host_extension_snapshot_capture(UmiPluginExtensionHostExtensionSnapshot *s,uint64_t ts,uint32_t installed,uint32_t running,uint32_t failed,uint32_t quarantined,uint32_t approvals) { if(s==NULL||running>installed||failed>installed||quarantined>installed) return UMI_STATUS_INVALID_ARGUMENT; umi_plugin_extension_host_extension_snapshot_init(s); s->timestamp_ms=ts; s->installed=installed; s->running=running; s->failed=failed; s->quarantined=quarantined; s->pending_approvals=approvals; s->health=quarantined>0U?UMI_PLUGIN_EXTENSION_HOST_HEALTH_UNHEALTHY:(failed>0U?UMI_PLUGIN_EXTENSION_HOST_HEALTH_DEGRADED:UMI_PLUGIN_EXTENSION_HOST_HEALTH_HEALTHY); s->evidence=ts^((uint64_t)installed<<48U)^((uint64_t)running<<32U)^((uint64_t)failed<<16U)^quarantined^approvals; return UMI_STATUS_OK; }
uint64_t umi_plugin_extension_host_extension_snapshot_fingerprint(const UmiPluginExtensionHostExtensionSnapshot *s) { return s==NULL?0U:s->evidence; }
