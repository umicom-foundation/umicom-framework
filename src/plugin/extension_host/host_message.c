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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/host_message.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/*
 * Copy plugin extension host host message into module-owned storage so callers keep
 * ownership of their input values.
 */
static void umi_plugin_extension_host_host_message_copy(char *destination, size_t capacity, const char *source)
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
 * Initialise plugin extension host host message from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_host_message_init(UmiPluginExtensionHostHostMessage *m) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(m!=NULL) memset(m,0,sizeof(*m)); }
/*
 * Provide the plugin extension host host message build operation used by this module and
 * its client applications.
 */
UmiStatus umi_plugin_extension_host_host_message_build(UmiPluginExtensionHostHostMessage *m,uint32_t v,uint32_t type,uint64_t seq,const char *sid,const char *payload) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||v==0U||seq==0U||!umi_plugin_extension_host_text_valid(sid,UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY)) return UMI_STATUS_INVALID_ARGUMENT; umi_plugin_extension_host_host_message_init(m); m->protocol_version=v; m->type=type; m->sequence=seq; umi_plugin_extension_host_host_message_copy(m->session_id,sizeof(m->session_id),sid); umi_plugin_extension_host_host_message_copy(m->payload,sizeof(m->payload),payload); m->fingerprint=umi_plugin_extension_host_hash_text(m->session_id)^umi_plugin_extension_host_hash_text(m->payload)^seq^type; return UMI_STATUS_OK; }
/*
 * Check that plugin extension host host message satisfies its contract before another
 * service relies on it.
 */
int umi_plugin_extension_host_host_message_valid(const UmiPluginExtensionHostHostMessage *m) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (m == NULL) return 0;
    if (memchr(m->session_id, '\0', sizeof(m->session_id)) == NULL) return 0;
    if (memchr(m->payload, '\0', sizeof(m->payload)) == NULL) return 0;
 return m!=NULL&&m->protocol_version!=0U&&m->sequence!=0U&&m->fingerprint==(umi_plugin_extension_host_hash_text(m->session_id)^umi_plugin_extension_host_hash_text(m->payload)^m->sequence^m->type); }

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiPluginExtensionHostHostMessageArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x9ff8dc9693975bce);
    schema = (schema ^ (uint64_t)sizeof(((UmiPluginExtensionHostHostMessage *)0)->session_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPluginExtensionHostHostMessage *)0)->payload)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiPluginExtensionHostHostMessageArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U + sizeof(((UmiPluginExtensionHostHostMessage *)0)->session_id) - 1U +
        8U + sizeof(((UmiPluginExtensionHostHostMessage *)0)->payload) - 1U +
        8U;
}
static void UmiPluginExtensionHostHostMessageArchiveWrite(UmiArchiveWriter *writer, const UmiPluginExtensionHostHostMessage *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->protocol_version);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->type);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->sequence);
    UmiArchiveWriteText(writer, value->session_id, sizeof(value->session_id));
    UmiArchiveWriteText(writer, value->payload, sizeof(value->payload));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->fingerprint);
}
static void UmiPluginExtensionHostHostMessageArchiveRead(UmiArchiveReader *reader, UmiPluginExtensionHostHostMessage *value)
{
    value->protocol_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->type = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    UmiArchiveReadText(reader, value->session_id, sizeof(value->session_id));
    UmiArchiveReadText(reader, value->payload, sizeof(value->payload));
    value->fingerprint = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiPluginExtensionHostHostMessageArchiveValidate(const UmiPluginExtensionHostHostMessage *value)
{
    return umi_plugin_extension_host_host_message_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_plugin_extension_host_host_message_archive_encode, umi_plugin_extension_host_host_message_archive_decode,
    UmiPluginExtensionHostHostMessage, UmiPluginExtensionHostHostMessageArchiveSchema, UmiPluginExtensionHostHostMessageArchiveBound, UmiPluginExtensionHostHostMessageArchiveWrite, UmiPluginExtensionHostHostMessageArchiveRead, UmiPluginExtensionHostHostMessageArchiveValidate)
