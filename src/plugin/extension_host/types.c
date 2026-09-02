/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/types.c
 *
 * PURPOSE:
 *   Define shared stable states, decisions, capacities and evidence helpers for the isolated plug-in extension host.
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
#include "umicom/plugin/extension_host/types.h"

/*
 * Provide the plugin extension host hash bytes operation used by this module and its
 * client applications.
 */
uint64_t umi_plugin_extension_host_hash_bytes(const void *data, size_t size)
{
    const unsigned char *bytes = (const unsigned char *)data;
    uint64_t value = UINT64_C(1469598103934665603);
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (data == NULL && size != 0U) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < size; ++i) { value ^= (uint64_t)bytes[i]; value *= UINT64_C(1099511628211); }
    return value;
}

/*
 * Provide the plugin extension host hash text operation used by this module and its client
 * applications.
 */
uint64_t umi_plugin_extension_host_hash_text(const char *text)
{
    uint64_t value = UINT64_C(1469598103934665603);
    size_t i = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return 0U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (text[i] != '\0') { value ^= (uint64_t)(unsigned char)text[i]; value *= UINT64_C(1099511628211); ++i; }
    return value;
}

/*
 * Check that plugin extension host text satisfies its contract before another service
 * relies on it.
 */
int umi_plugin_extension_host_text_valid(const char *text, size_t capacity)
{
    size_t i = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || capacity == 0U || text[0] == '\0') return 0;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (i < capacity && text[i] != '\0') ++i;
    return i < capacity;
}

/*
 * Provide the plugin extension host decision text operation used by this module and its
 * client applications.
 */
const char *umi_plugin_extension_host_decision_text(UmiPluginExtensionHostDecision decision)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (decision == UMI_PLUGIN_EXTENSION_HOST_ALLOW) return "allow";
    /* Apply this branch only when its contract condition is satisfied. */
    if (decision == UMI_PLUGIN_EXTENSION_HOST_REQUIRE_APPROVAL) return "require-approval";
    return "deny";
}
