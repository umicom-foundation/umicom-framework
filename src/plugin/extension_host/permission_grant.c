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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/permission_grant.h"
#include <string.h>

static void umi_plugin_extension_host_permission_grant_copy(char *destination, size_t capacity, const char *source)
{
    size_t i = 0U;
    if (destination == NULL || capacity == 0U) return;
    if (source != NULL) {
        while (i + 1U < capacity && source[i] != '\0') { destination[i] = source[i]; ++i; }
    }
    destination[i] = '\0';
}

void umi_plugin_extension_host_permission_grant_init(UmiPluginExtensionHostPermissionGrant *value)
{
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_PLUGIN_EXTENSION_HOST_API_VERSION;
}

UmiStatus umi_plugin_extension_host_permission_grant_configure(UmiPluginExtensionHostPermissionGrant *value, const char *id, const char *subject, uint32_t version, uint32_t risk, uint64_t flags)
{
    if (value == NULL || !umi_plugin_extension_host_text_valid(id, UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY) || risk > 100U) return UMI_STATUS_INVALID_ARGUMENT;
    umi_plugin_extension_host_permission_grant_init(value);
    umi_plugin_extension_host_permission_grant_copy(value->id, sizeof(value->id), id);
    umi_plugin_extension_host_permission_grant_copy(value->subject, sizeof(value->subject), subject);
    value->version = version; value->risk = risk; value->flags = flags; value->revision = 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_plugin_extension_host_permission_grant_validate(const UmiPluginExtensionHostPermissionGrant *value)
{
    if (value == NULL || value->struct_size != sizeof(*value) || value->api_version != UMI_PLUGIN_EXTENSION_HOST_API_VERSION) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_plugin_extension_host_text_valid(value->id, sizeof(value->id)) || value->risk > 100U) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

uint64_t umi_plugin_extension_host_permission_grant_fingerprint(const UmiPluginExtensionHostPermissionGrant *value)
{
    uint64_t result;
    if (umi_plugin_extension_host_permission_grant_validate(value) != UMI_STATUS_OK) return 0U;
    result = umi_plugin_extension_host_hash_text(value->id);
    result ^= umi_plugin_extension_host_hash_text(value->subject) + UINT64_C(0x9e3779b97f4a7c15) + (result << 6U) + (result >> 2U);
    result ^= ((uint64_t)value->version << 32U) ^ value->flags ^ value->risk;
    return result;
}
