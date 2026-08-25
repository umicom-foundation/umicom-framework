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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/host_session.h"
#include <string.h>

static void umi_plugin_extension_host_host_session_copy(char *destination, size_t capacity, const char *source)
{
    size_t i = 0U;
    if (destination == NULL || capacity == 0U) return;
    if (source != NULL) {
        while (i + 1U < capacity && source[i] != '\0') { destination[i] = source[i]; ++i; }
    }
    destination[i] = '\0';
}

void umi_plugin_extension_host_host_session_init(UmiPluginExtensionHostHostSession *s,const char *id) { if(s==NULL) return; memset(s,0,sizeof(*s)); umi_plugin_extension_host_host_session_copy(s->id,sizeof(s->id),id); s->state=UMI_PLUGIN_EXTENSION_HOST_HOST_SESSION_DISCONNECTED; }
UmiStatus umi_plugin_extension_host_host_session_transition(UmiPluginExtensionHostHostSession *s,UmiPluginExtensionHostHostSessionState next) { int ok=0; if(s==NULL) return UMI_STATUS_INVALID_ARGUMENT; if(s->state==UMI_PLUGIN_EXTENSION_HOST_HOST_SESSION_DISCONNECTED&&next==UMI_PLUGIN_EXTENSION_HOST_HOST_SESSION_CONNECTING) ok=1; else if(s->state==UMI_PLUGIN_EXTENSION_HOST_HOST_SESSION_CONNECTING&&(next==UMI_PLUGIN_EXTENSION_HOST_HOST_SESSION_READY||next==UMI_PLUGIN_EXTENSION_HOST_HOST_SESSION_CLOSED)) ok=1; else if(s->state==UMI_PLUGIN_EXTENSION_HOST_HOST_SESSION_READY&&(next==UMI_PLUGIN_EXTENSION_HOST_HOST_SESSION_DRAINING||next==UMI_PLUGIN_EXTENSION_HOST_HOST_SESSION_CLOSED)) ok=1; else if(s->state==UMI_PLUGIN_EXTENSION_HOST_HOST_SESSION_DRAINING&&next==UMI_PLUGIN_EXTENSION_HOST_HOST_SESSION_CLOSED) ok=1; if(!ok) return UMI_STATUS_INVALID_STATE; s->state=next; ++s->revision; return UMI_STATUS_OK; }
int umi_plugin_extension_host_host_session_ready(const UmiPluginExtensionHostHostSession *s) { return s!=NULL&&s->state==UMI_PLUGIN_EXTENSION_HOST_HOST_SESSION_READY; }
