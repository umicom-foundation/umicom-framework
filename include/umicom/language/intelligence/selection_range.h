/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/selection_range.h
 *
 * PURPOSE:
 *   Validate and compare hierarchical selection ranges.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_SELECTION_RANGE_H
#define UMICOM_LANGUAGE_INTELLIGENCE_SELECTION_RANGE_H
#include "umicom/language/intelligence/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_SELECTION_RANGE_API_VERSION 1U
/**
 * Represent the language intelligence selection range data shared with callers of this
 * public contract.
 */
typedef struct UmiLanguageIntelligenceSelectionRange {
    uint32_t struct_size;
    uint32_t api_version;
    char uri[UMI_LANGUAGE_INTELLIGENCE_PATH_CAPACITY];
    UmiLanguageIntelligenceRange primary;
    UmiLanguageIntelligenceRange parent;
    uint32_t depth;
    uint64_t revision;
} UmiLanguageIntelligenceSelectionRange;
/**
 * Initialise language intelligence selection range from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_intelligence_selection_range_init(UmiLanguageIntelligenceSelectionRange *value, const char *uri);
/**
 * Provide the language intelligence selection range set ranges operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_intelligence_selection_range_set_ranges(
    UmiLanguageIntelligenceSelectionRange *value,
    const UmiLanguageIntelligenceRange *primary,
    const UmiLanguageIntelligenceRange *parent);
/**
 * Check that language intelligence selection range satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_language_intelligence_selection_range_validate(const UmiLanguageIntelligenceSelectionRange *value);
/**
 * Provide the language intelligence selection range is nested operation used by this
 * module and its client applications.
 */
int umi_language_intelligence_selection_range_is_nested(const UmiLanguageIntelligenceSelectionRange *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_language_intelligence_selection_range_archive_encode(const UmiLanguageIntelligenceSelectionRange *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_language_intelligence_selection_range_archive_decode(const void *bytes, size_t byte_count,
    UmiLanguageIntelligenceSelectionRange *value);

#ifdef __cplusplus
}
#endif
#endif
