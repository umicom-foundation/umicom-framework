/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/host_process.c
 *
 * PURPOSE:
 *   Track one isolated extension-host process and lifecycle evidence.
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
#include "umicom/plugin/extension_host/host_process.h"
#include <string.h>

static void umi_plugin_extension_host_host_process_copy(char *destination, size_t capacity, const char *source)
{
    size_t i = 0U;
    if (destination == NULL || capacity == 0U) return;
    if (source != NULL) {
        while (i + 1U < capacity && source[i] != '\0') { destination[i] = source[i]; ++i; }
    }
    destination[i] = '\0';
}

void umi_plugin_extension_host_host_process_init(UmiPluginExtensionHostHostProcess *p) { if(p!=NULL) { memset(p,0,sizeof(*p)); p->lifecycle=UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_STOPPED; } }
UmiStatus umi_plugin_extension_host_host_process_start(UmiPluginExtensionHostHostProcess *p,const char *plugin_id,uint64_t process_id,uint64_t now_ms) { if(p==NULL||process_id==0U||!umi_plugin_extension_host_text_valid(plugin_id,UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY)) return UMI_STATUS_INVALID_ARGUMENT; umi_plugin_extension_host_host_process_copy(p->plugin_id,sizeof(p->plugin_id),plugin_id); p->process_id=process_id; p->started_ms=now_ms; p->last_heartbeat_ms=now_ms; p->lifecycle=UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_RUNNING; return UMI_STATUS_OK; }
void umi_plugin_extension_host_host_process_heartbeat(UmiPluginExtensionHostHostProcess *p,uint64_t now_ms) { if(p!=NULL&&p->lifecycle==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_RUNNING) p->last_heartbeat_ms=now_ms; }
void umi_plugin_extension_host_host_process_crashed(UmiPluginExtensionHostHostProcess *p) { if(p!=NULL) { ++p->crash_count; p->process_id=0U; p->lifecycle=UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_FAILED; } }
