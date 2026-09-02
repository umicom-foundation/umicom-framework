/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/manifest_evidence.c
 *
 * PURPOSE:
 *   Capture immutable evidence derived from a parsed extension manifest.
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
#include "umicom/plugin/extension_host/manifest_evidence.h"
#include <string.h>

/*
 * Copy plugin extension host manifest evidence into module-owned storage so callers keep
 * ownership of their input values.
 */
static void umi_plugin_extension_host_manifest_evidence_copy(char *destination, size_t capacity, const char *source)
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
 * Initialise plugin extension host manifest evidence from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_manifest_evidence_init(UmiPluginExtensionHostManifestEvidence *value)
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
 * Provide the plugin extension host manifest evidence configure operation used by this
 * module and its client applications.
 */
UmiStatus umi_plugin_extension_host_manifest_evidence_configure(UmiPluginExtensionHostManifestEvidence *value, const char *id, const char *subject, uint32_t version, uint32_t risk, uint64_t flags)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || !umi_plugin_extension_host_text_valid(id, UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY) || risk > 100U) return UMI_STATUS_INVALID_ARGUMENT;
    umi_plugin_extension_host_manifest_evidence_init(value);
    umi_plugin_extension_host_manifest_evidence_copy(value->id, sizeof(value->id), id);
    umi_plugin_extension_host_manifest_evidence_copy(value->subject, sizeof(value->subject), subject);
    value->version = version; value->risk = risk; value->flags = flags; value->revision = 1U;
    return UMI_STATUS_OK;
}

/*
 * Check that plugin extension host manifest evidence satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_plugin_extension_host_manifest_evidence_validate(const UmiPluginExtensionHostManifestEvidence *value)
{
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
 * Provide the plugin extension host manifest evidence fingerprint operation used by this
 * module and its client applications.
 */
uint64_t umi_plugin_extension_host_manifest_evidence_fingerprint(const UmiPluginExtensionHostManifestEvidence *value)
{
    uint64_t result;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_plugin_extension_host_manifest_evidence_validate(value) != UMI_STATUS_OK) return 0U;
    result = umi_plugin_extension_host_hash_text(value->id);
    result ^= umi_plugin_extension_host_hash_text(value->subject) + UINT64_C(0x9e3779b97f4a7c15) + (result << 6U) + (result >> 2U);
    result ^= ((uint64_t)value->version << 32U) ^ value->flags ^ value->risk;
    return result;
}
