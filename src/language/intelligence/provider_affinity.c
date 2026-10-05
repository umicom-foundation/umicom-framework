/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/provider_affinity.c
 *
 * PURPOSE:
 *   Implement bind languages, workspaces or projects to preferred provider identities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/provider_affinity.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/*
 * Initialise language intelligence provider affinity from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_intelligence_provider_affinity_init(UmiLanguageIntelligenceProviderAffinity *mapping)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (mapping == NULL) return;
    (void)memset(mapping, 0, sizeof(*mapping));
    mapping->struct_size = (uint32_t)sizeof(*mapping);
    mapping->api_version = UMI_LANGUAGE_INTELLIGENCE_PROVIDER_AFFINITY_API_VERSION;
    mapping->enabled = 1;
    mapping->revision = 1U;
}

/*
 * Copy language intelligence provider affinity into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_language_intelligence_provider_affinity_set(
    UmiLanguageIntelligenceProviderAffinity *mapping,
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
 * Check that language intelligence provider affinity satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_language_intelligence_provider_affinity_validate(
    const UmiLanguageIntelligenceProviderAffinity *mapping)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (mapping == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(mapping->source_id, '\0', sizeof(mapping->source_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(mapping->target_id, '\0', sizeof(mapping->target_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(mapping->scope_id, '\0', sizeof(mapping->scope_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (mapping == NULL ||
        mapping->struct_size < sizeof(*mapping) ||
        mapping->api_version != UMI_LANGUAGE_INTELLIGENCE_PROVIDER_AFFINITY_API_VERSION ||
        mapping->source_id[0] == '\0' || mapping->target_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/*
 * Provide the language intelligence provider affinity matches operation used by this
 * module and its client applications.
 */
int umi_language_intelligence_provider_affinity_matches(
    const UmiLanguageIntelligenceProviderAffinity *mapping,
    const char *source_id,
    const char *scope_id)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_language_intelligence_provider_affinity_validate(mapping) != UMI_STATUS_OK ||
        source_id == NULL || strcmp(mapping->source_id, source_id) != 0 ||
        mapping->enabled == 0) return 0;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (mapping->scope_id[0] == '\0') return 1;
    return scope_id != NULL && strcmp(mapping->scope_id, scope_id) == 0;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiLanguageIntelligenceProviderAffinityArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xb90f79412670e332);
    schema = (schema ^ (uint64_t)sizeof(((UmiLanguageIntelligenceProviderAffinity *)0)->source_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiLanguageIntelligenceProviderAffinity *)0)->target_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiLanguageIntelligenceProviderAffinity *)0)->scope_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiLanguageIntelligenceProviderAffinityArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U + sizeof(((UmiLanguageIntelligenceProviderAffinity *)0)->source_id) - 1U +
        8U + sizeof(((UmiLanguageIntelligenceProviderAffinity *)0)->target_id) - 1U +
        8U + sizeof(((UmiLanguageIntelligenceProviderAffinity *)0)->scope_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiLanguageIntelligenceProviderAffinityArchiveWrite(UmiArchiveWriter *writer, const UmiLanguageIntelligenceProviderAffinity *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->api_version);
    UmiArchiveWriteText(writer, value->source_id, sizeof(value->source_id));
    UmiArchiveWriteText(writer, value->target_id, sizeof(value->target_id));
    UmiArchiveWriteText(writer, value->scope_id, sizeof(value->scope_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->priority);
    UmiArchiveWriteSigned(writer, (int64_t)value->enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiLanguageIntelligenceProviderAffinityArchiveRead(UmiArchiveReader *reader, UmiLanguageIntelligenceProviderAffinity *value)
{
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    UmiArchiveReadText(reader, value->source_id, sizeof(value->source_id));
    UmiArchiveReadText(reader, value->target_id, sizeof(value->target_id));
    UmiArchiveReadText(reader, value->scope_id, sizeof(value->scope_id));
    value->priority = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->enabled = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiLanguageIntelligenceProviderAffinityArchiveValidate(const UmiLanguageIntelligenceProviderAffinity *value)
{
    return umi_language_intelligence_provider_affinity_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_language_intelligence_provider_affinity_archive_encode, umi_language_intelligence_provider_affinity_archive_decode,
    UmiLanguageIntelligenceProviderAffinity, UmiLanguageIntelligenceProviderAffinityArchiveSchema, UmiLanguageIntelligenceProviderAffinityArchiveBound, UmiLanguageIntelligenceProviderAffinityArchiveWrite, UmiLanguageIntelligenceProviderAffinityArchiveRead, UmiLanguageIntelligenceProviderAffinityArchiveValidate)
