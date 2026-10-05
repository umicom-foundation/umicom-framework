/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/permission_grant.c
 *
 * PURPOSE:
 *   Describe one persisted permission grant and its decision provenance.
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
#include "umicom/plugin/extension_host/permission_grant.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/*
 * Copy plugin extension host permission grant into module-owned storage so callers keep
 * ownership of their input values.
 */
static void umi_plugin_extension_host_permission_grant_copy(char *destination, size_t capacity, const char *source)
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
 * Initialise plugin extension host permission grant from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_permission_grant_init(UmiPluginExtensionHostPermissionGrant *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_PLUGIN_EXTENSION_HOST_API_VERSION;
}

/*
 * Provide the plugin extension host permission grant configure operation used by this
 * module and its client applications.
 */
UmiStatus umi_plugin_extension_host_permission_grant_configure(UmiPluginExtensionHostPermissionGrant *value, const char *id, const char *subject, uint32_t version, uint32_t risk, uint64_t flags)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || !umi_plugin_extension_host_text_valid(id, UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY) || risk > 100U) return UMI_STATUS_INVALID_ARGUMENT;
    umi_plugin_extension_host_permission_grant_init(value);
    umi_plugin_extension_host_permission_grant_copy(value->id, sizeof(value->id), id);
    umi_plugin_extension_host_permission_grant_copy(value->subject, sizeof(value->subject), subject);
    value->version = version; value->risk = risk; value->flags = flags; value->revision = 1U;
    return UMI_STATUS_OK;
}

/*
 * Check that plugin extension host permission grant satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_plugin_extension_host_permission_grant_validate(const UmiPluginExtensionHostPermissionGrant *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->subject, '\0', sizeof(value->subject)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->struct_size != sizeof(*value) || value->api_version != UMI_PLUGIN_EXTENSION_HOST_API_VERSION) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_plugin_extension_host_text_valid(value->id, sizeof(value->id)) || value->risk > 100U) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/*
 * Provide the plugin extension host permission grant fingerprint operation used by this
 * module and its client applications.
 */
uint64_t umi_plugin_extension_host_permission_grant_fingerprint(const UmiPluginExtensionHostPermissionGrant *value)
{
    uint64_t result;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_plugin_extension_host_permission_grant_validate(value) != UMI_STATUS_OK) return 0U;
    result = umi_plugin_extension_host_hash_text(value->id);
    result ^= umi_plugin_extension_host_hash_text(value->subject) + UINT64_C(0x9e3779b97f4a7c15) + (result << 6U) + (result >> 2U);
    result ^= ((uint64_t)value->version << 32U) ^ value->flags ^ value->risk;
    return result;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiPluginExtensionHostPermissionGrantArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x4891e2ab0aefd91a);
    schema = (schema ^ (uint64_t)sizeof(((UmiPluginExtensionHostPermissionGrant *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPluginExtensionHostPermissionGrant *)0)->subject)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiPluginExtensionHostPermissionGrantArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U + sizeof(((UmiPluginExtensionHostPermissionGrant *)0)->id) - 1U +
        8U + sizeof(((UmiPluginExtensionHostPermissionGrant *)0)->subject) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiPluginExtensionHostPermissionGrantArchiveWrite(UmiArchiveWriter *writer, const UmiPluginExtensionHostPermissionGrant *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->api_version);
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->subject, sizeof(value->subject));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->version);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->risk);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->flags);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiPluginExtensionHostPermissionGrantArchiveRead(UmiArchiveReader *reader, UmiPluginExtensionHostPermissionGrant *value)
{
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->subject, sizeof(value->subject));
    value->version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->risk = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->flags = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiPluginExtensionHostPermissionGrantArchiveValidate(const UmiPluginExtensionHostPermissionGrant *value)
{
    return umi_plugin_extension_host_permission_grant_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_plugin_extension_host_permission_grant_archive_encode, umi_plugin_extension_host_permission_grant_archive_decode,
    UmiPluginExtensionHostPermissionGrant, UmiPluginExtensionHostPermissionGrantArchiveSchema, UmiPluginExtensionHostPermissionGrantArchiveBound, UmiPluginExtensionHostPermissionGrantArchiveWrite, UmiPluginExtensionHostPermissionGrantArchiveRead, UmiPluginExtensionHostPermissionGrantArchiveValidate)
