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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_SELECTION_RANGE_H
#define UMICOM_LANGUAGE_INTELLIGENCE_SELECTION_RANGE_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_SELECTION_RANGE_API_VERSION 1U
typedef struct UmiLanguageIntelligenceSelectionRange {
    uint32_t struct_size;
    uint32_t api_version;
    char uri[UMI_LANGUAGE_INTELLIGENCE_PATH_CAPACITY];
    UmiLanguageIntelligenceRange primary;
    UmiLanguageIntelligenceRange parent;
    uint32_t depth;
    uint64_t revision;
} UmiLanguageIntelligenceSelectionRange;
void umi_language_intelligence_selection_range_init(UmiLanguageIntelligenceSelectionRange *value, const char *uri);
UmiStatus umi_language_intelligence_selection_range_set_ranges(
    UmiLanguageIntelligenceSelectionRange *value,
    const UmiLanguageIntelligenceRange *primary,
    const UmiLanguageIntelligenceRange *parent);
UmiStatus umi_language_intelligence_selection_range_validate(const UmiLanguageIntelligenceSelectionRange *value);
int umi_language_intelligence_selection_range_is_nested(const UmiLanguageIntelligenceSelectionRange *value);
#ifdef __cplusplus
}
#endif
#endif
