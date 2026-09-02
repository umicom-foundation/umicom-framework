/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/diagnostics_merge.h
 *
 * PURPOSE:
 *   Merge diagnostic identities from several providers into one bounded Framework view.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_MERGE_H
#define UMICOM_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_MERGE_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_MERGE_API_VERSION 1U
/**
 * Represent the language intelligence diagnostics merge item data shared with callers of
 * this public contract.
 */
typedef struct UmiLanguageIntelligenceDiagnosticsMergeItem {
    char id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char label[UMI_LANGUAGE_INTELLIGENCE_TEXT_CAPACITY];
    uint64_t fingerprint;
    uint32_t priority;
    uint32_t flags;
    int enabled;
} UmiLanguageIntelligenceDiagnosticsMergeItem;
/**
 * Represent the language intelligence diagnostics merge data shared with callers of this
 * public contract.
 */
typedef struct UmiLanguageIntelligenceDiagnosticsMerge {
    uint32_t struct_size;
    uint32_t api_version;
    UmiLanguageIntelligenceDiagnosticsMergeItem items[UMI_LANGUAGE_INTELLIGENCE_ITEM_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiLanguageIntelligenceDiagnosticsMerge;
/**
 * Initialise language intelligence diagnostics merge from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_intelligence_diagnostics_merge_init(UmiLanguageIntelligenceDiagnosticsMerge *catalogue);
/**
 * Provide the language intelligence diagnostics merge upsert operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_intelligence_diagnostics_merge_upsert(
    UmiLanguageIntelligenceDiagnosticsMerge *catalogue,
    const UmiLanguageIntelligenceDiagnosticsMergeItem *item);
/**
 * Find language intelligence diagnostics merge while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiLanguageIntelligenceDiagnosticsMergeItem *umi_language_intelligence_diagnostics_merge_find(
    const UmiLanguageIntelligenceDiagnosticsMerge *catalogue,
    const char *id);
/**
 * Provide the language intelligence diagnostics merge best operation used by this module
 * and its client applications.
 */
const UmiLanguageIntelligenceDiagnosticsMergeItem *umi_language_intelligence_diagnostics_merge_best(
    const UmiLanguageIntelligenceDiagnosticsMerge *catalogue);
#ifdef __cplusplus
}
#endif
#endif
