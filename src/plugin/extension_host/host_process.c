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

/*
 * Copy plugin extension host host process into module-owned storage so callers keep
 * ownership of their input values.
 */
static void umi_plugin_extension_host_host_process_copy(char *destination, size_t capacity, const char *source)
{
    size_t i = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source != NULL) {
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (i + 1U < capacity && source[i] != '\0') { destination[i] = source[i]; ++i; }
    }
    destination[i] = '\0';
}

/*
 * Initialise plugin extension host host process from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_host_process_init(UmiPluginExtensionHostHostProcess *p) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p!=NULL) { memset(p,0,sizeof(*p)); p->lifecycle=UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_STOPPED; } }
/*
 * Provide the plugin extension host host process start operation used by this module and
 * its client applications.
 */
UmiStatus umi_plugin_extension_host_host_process_start(UmiPluginExtensionHostHostProcess *p,const char *plugin_id,uint64_t process_id,uint64_t now_ms) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||process_id==0U||!umi_plugin_extension_host_text_valid(plugin_id,UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY)) return UMI_STATUS_INVALID_ARGUMENT; umi_plugin_extension_host_host_process_copy(p->plugin_id,sizeof(p->plugin_id),plugin_id); p->process_id=process_id; p->started_ms=now_ms; p->last_heartbeat_ms=now_ms; p->lifecycle=UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_RUNNING; return UMI_STATUS_OK; }
/*
 * Provide the plugin extension host host process heartbeat operation used by this module
 * and its client applications.
 */
void umi_plugin_extension_host_host_process_heartbeat(UmiPluginExtensionHostHostProcess *p,uint64_t now_ms) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p!=NULL&&p->lifecycle==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_RUNNING) p->last_heartbeat_ms=now_ms; }
/*
 * Provide the plugin extension host host process crashed operation used by this module and
 * its client applications.
 */
void umi_plugin_extension_host_host_process_crashed(UmiPluginExtensionHostHostProcess *p) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p!=NULL) { ++p->crash_count; p->process_id=0U; p->lifecycle=UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_FAILED; } }
