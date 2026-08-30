/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/selection_range.c
 *
 * PURPOSE:
 *   Implement validate and compare hierarchical selection ranges.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/selection_range.h"
#include <string.h>
void umi_language_intelligence_selection_range_init(UmiLanguageIntelligenceSelectionRange *value, const char *uri)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_LANGUAGE_INTELLIGENCE_SELECTION_RANGE_API_VERSION;
    value->revision = 1U;
    if (uri != NULL) (void)umi_language_intelligence_copy_text(
        value->uri, sizeof(value->uri), uri);
}
UmiStatus umi_language_intelligence_selection_range_set_ranges(
    UmiLanguageIntelligenceSelectionRange *value,
    const UmiLanguageIntelligenceRange *primary,
    const UmiLanguageIntelligenceRange *parent)
{
    if (value == NULL || primary == NULL || parent == NULL ||
        !umi_language_intelligence_range_is_valid(primary) ||
        !umi_language_intelligence_range_is_valid(parent) ||
        !umi_language_intelligence_range_contains(parent, primary))
        return UMI_STATUS_INVALID_ARGUMENT;
    value->primary = *primary;
    value->parent = *parent;
    if (value->revision != UINT64_MAX) value->revision += 1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_language_intelligence_selection_range_validate(const UmiLanguageIntelligenceSelectionRange *value)
{
    if (value == NULL || value->uri[0] == '\0' ||
        !umi_language_intelligence_range_is_valid(&value->primary) ||
        !umi_language_intelligence_range_is_valid(&value->parent) ||
        !umi_language_intelligence_range_contains(&value->parent, &value->primary))
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
int umi_language_intelligence_selection_range_is_nested(const UmiLanguageIntelligenceSelectionRange *value)
{
    return umi_language_intelligence_selection_range_validate(value) == UMI_STATUS_OK &&
        (value->primary.start.line != value->parent.start.line ||
         value->primary.start.character != value->parent.start.character ||
         value->primary.end.line != value->parent.end.line ||
         value->primary.end.character != value->parent.end.character);
}
