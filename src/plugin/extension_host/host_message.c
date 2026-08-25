/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/host_message.c
 *
 * PURPOSE:
 *   Describe one versioned extension-host protocol message.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/host_message.h"
#include <string.h>

static void umi_plugin_extension_host_host_message_copy(char *destination, size_t capacity, const char *source)
{
    size_t i = 0U;
    if (destination == NULL || capacity == 0U) return;
    if (source != NULL) {
        while (i + 1U < capacity && source[i] != '\0') { destination[i] = source[i]; ++i; }
    }
    destination[i] = '\0';
}

void umi_plugin_extension_host_host_message_init(UmiPluginExtensionHostHostMessage *m) { if(m!=NULL) memset(m,0,sizeof(*m)); }
UmiStatus umi_plugin_extension_host_host_message_build(UmiPluginExtensionHostHostMessage *m,uint32_t v,uint32_t type,uint64_t seq,const char *sid,const char *payload) { if(m==NULL||v==0U||seq==0U||!umi_plugin_extension_host_text_valid(sid,UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY)) return UMI_STATUS_INVALID_ARGUMENT; umi_plugin_extension_host_host_message_init(m); m->protocol_version=v; m->type=type; m->sequence=seq; umi_plugin_extension_host_host_message_copy(m->session_id,sizeof(m->session_id),sid); umi_plugin_extension_host_host_message_copy(m->payload,sizeof(m->payload),payload); m->fingerprint=umi_plugin_extension_host_hash_text(m->session_id)^umi_plugin_extension_host_hash_text(m->payload)^seq^type; return UMI_STATUS_OK; }
int umi_plugin_extension_host_host_message_valid(const UmiPluginExtensionHostHostMessage *m) { return m!=NULL&&m->protocol_version!=0U&&m->sequence!=0U&&m->fingerprint==(umi_plugin_extension_host_hash_text(m->session_id)^umi_plugin_extension_host_hash_text(m->payload)^m->sequence^m->type); }
