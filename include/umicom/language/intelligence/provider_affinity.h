/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/provider_affinity.h
 *
 * PURPOSE:
 *   Bind languages, workspaces or projects to preferred provider identities.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable language-intelligence capability. Studio,
 *   Desk and every product remain thin consumers of Framework state/contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_PROVIDER_AFFINITY_H
#define UMICOM_LANGUAGE_INTELLIGENCE_PROVIDER_AFFINITY_H
#include "umicom/language/intelligence/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_PROVIDER_AFFINITY_API_VERSION 1U

/**
 * Represent the language intelligence provider affinity data shared with callers of this
 * public contract.
 */
typedef struct UmiLanguageIntelligenceProviderAffinity {
    uint32_t struct_size;
    uint32_t api_version;
    char source_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char target_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char scope_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    uint32_t priority;
    int enabled;
    uint64_t revision;
} UmiLanguageIntelligenceProviderAffinity;

/**
 * Initialise language intelligence provider affinity from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_intelligence_provider_affinity_init(UmiLanguageIntelligenceProviderAffinity *mapping);
/**
 * Copy language intelligence provider affinity into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_language_intelligence_provider_affinity_set(
    UmiLanguageIntelligenceProviderAffinity *mapping,
    const char *source_id,
    const char *target_id,
    const char *scope_id);
/**
 * Check that language intelligence provider affinity satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_language_intelligence_provider_affinity_validate(
    const UmiLanguageIntelligenceProviderAffinity *mapping);
/**
 * Provide the language intelligence provider affinity matches operation used by this
 * module and its client applications.
 */
int umi_language_intelligence_provider_affinity_matches(
    const UmiLanguageIntelligenceProviderAffinity *mapping,
    const char *source_id,
    const char *scope_id);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_language_intelligence_provider_affinity_archive_encode(const UmiLanguageIntelligenceProviderAffinity *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_language_intelligence_provider_affinity_archive_decode(const void *bytes, size_t byte_count,
    UmiLanguageIntelligenceProviderAffinity *value);

#ifdef __cplusplus
}
#endif
#endif
