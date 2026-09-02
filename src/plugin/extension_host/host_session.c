/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/host_session.c
 *
 * PURPOSE:
 *   Track one extension-host session through connect, ready, draining and closed states.
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
#include "umicom/plugin/extension_host/host_session.h"
#include <string.h>

/*
 * Copy plugin extension host host session into module-owned storage so callers keep
 * ownership of their input values.
 */
static void umi_plugin_extension_host_host_session_copy(char *destination, size_t capacity, const char *source)
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
 * Initialise plugin extension host host session from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_host_session_init(UmiPluginExtensionHostHostSession *s,const char *id) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL) return; memset(s,0,sizeof(*s)); umi_plugin_extension_host_host_session_copy(s->id,sizeof(s->id),id); s->state=UMI_PLUGIN_EXTENSION_HOST_HOST_SESSION_DISCONNECTED; }
/*
 * Provide the plugin extension host host session transition operation used by this module
 * and its client applications.
 */
UmiStatus umi_plugin_extension_host_host_session_transition(UmiPluginExtensionHostHostSession *s,UmiPluginExtensionHostHostSessionState next) { int ok=0; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL) return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->state==UMI_PLUGIN_EXTENSION_HOST_HOST_SESSION_DISCONNECTED&&next==UMI_PLUGIN_EXTENSION_HOST_HOST_SESSION_CONNECTING) ok=1; else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->state==UMI_PLUGIN_EXTENSION_HOST_HOST_SESSION_CONNECTING&&(next==UMI_PLUGIN_EXTENSION_HOST_HOST_SESSION_READY||next==UMI_PLUGIN_EXTENSION_HOST_HOST_SESSION_CLOSED)) ok=1; else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->state==UMI_PLUGIN_EXTENSION_HOST_HOST_SESSION_READY&&(next==UMI_PLUGIN_EXTENSION_HOST_HOST_SESSION_DRAINING||next==UMI_PLUGIN_EXTENSION_HOST_HOST_SESSION_CLOSED)) ok=1; else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->state==UMI_PLUGIN_EXTENSION_HOST_HOST_SESSION_DRAINING&&next==UMI_PLUGIN_EXTENSION_HOST_HOST_SESSION_CLOSED) ok=1; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(!ok) return UMI_STATUS_INVALID_STATE; s->state=next; ++s->revision; return UMI_STATUS_OK; }
/*
 * Provide the plugin extension host host session ready operation used by this module and
 * its client applications.
 */
int umi_plugin_extension_host_host_session_ready(const UmiPluginExtensionHostHostSession *s) { return s!=NULL&&s->state==UMI_PLUGIN_EXTENSION_HOST_HOST_SESSION_READY; }
