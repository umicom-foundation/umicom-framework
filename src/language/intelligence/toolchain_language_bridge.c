/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/toolchain_language_bridge.c
 *
 * PURPOSE:
 *   Implement map language profiles to Framework developer-kit/toolchain identities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/toolchain_language_bridge.h"
#include <string.h>

/*
 * Initialise language intelligence toolchain language bridge from caller-provided values
 * so later operations receive a known state.
 */
void umi_language_intelligence_toolchain_language_bridge_init(UmiLanguageIntelligenceToolchainLanguageBridge *mapping)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (mapping == NULL) return;
    (void)memset(mapping, 0, sizeof(*mapping));
    mapping->struct_size = (uint32_t)sizeof(*mapping);
    mapping->api_version = UMI_LANGUAGE_INTELLIGENCE_TOOLCHAIN_LANGUAGE_BRIDGE_API_VERSION;
    mapping->enabled = 1;
    mapping->revision = 1U;
}

/*
 * Copy language intelligence toolchain language bridge into module-owned storage so
 * callers keep ownership of their input values.
 */
UmiStatus umi_language_intelligence_toolchain_language_bridge_set(
    UmiLanguageIntelligenceToolchainLanguageBridge *mapping,
    const char *source_id,
    const char *target_id,
    const char *scope_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (mapping == NULL || source_id == NULL || target_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_language_intelligence_copy_text(
        mapping->source_id, sizeof(mapping->source_id), source_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_language_intelligence_copy_text(
        mapping->target_id, sizeof(mapping->target_id), target_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    mapping->scope_id[0] = '\0';
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (scope_id != NULL && scope_id[0] != '\0') {
        status = umi_language_intelligence_copy_text(
            mapping->scope_id, sizeof(mapping->scope_id), scope_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (mapping->revision != UINT64_MAX) mapping->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Check that language intelligence toolchain language bridge satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_language_intelligence_toolchain_language_bridge_validate(
    const UmiLanguageIntelligenceToolchainLanguageBridge *mapping)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (mapping == NULL ||
        mapping->struct_size < sizeof(*mapping) ||
        mapping->api_version != UMI_LANGUAGE_INTELLIGENCE_TOOLCHAIN_LANGUAGE_BRIDGE_API_VERSION ||
        mapping->source_id[0] == '\0' || mapping->target_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/*
 * Provide the language intelligence toolchain language bridge matches operation used by
 * this module and its client applications.
 */
int umi_language_intelligence_toolchain_language_bridge_matches(
    const UmiLanguageIntelligenceToolchainLanguageBridge *mapping,
    const char *source_id,
    const char *scope_id)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_language_intelligence_toolchain_language_bridge_validate(mapping) != UMI_STATUS_OK ||
        source_id == NULL || strcmp(mapping->source_id, source_id) != 0 ||
        mapping->enabled == 0) return 0;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (mapping->scope_id[0] == '\0') return 1;
    return scope_id != NULL && strcmp(mapping->scope_id, scope_id) == 0;
}
