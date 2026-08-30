/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/linked_editing.h
 *
 * PURPOSE:
 *   Represent linked-editing ranges and containment semantics.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_LINKED_EDITING_H
#define UMICOM_LANGUAGE_INTELLIGENCE_LINKED_EDITING_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_LINKED_EDITING_API_VERSION 1U
typedef struct UmiLanguageIntelligenceLinkedEditing {
    uint32_t struct_size;
    uint32_t api_version;
    char uri[UMI_LANGUAGE_INTELLIGENCE_PATH_CAPACITY];
    UmiLanguageIntelligenceRange primary;
    UmiLanguageIntelligenceRange parent;
    uint32_t depth;
    uint64_t revision;
} UmiLanguageIntelligenceLinkedEditing;
void umi_language_intelligence_linked_editing_init(UmiLanguageIntelligenceLinkedEditing *value, const char *uri);
UmiStatus umi_language_intelligence_linked_editing_set_ranges(
    UmiLanguageIntelligenceLinkedEditing *value,
    const UmiLanguageIntelligenceRange *primary,
    const UmiLanguageIntelligenceRange *parent);
UmiStatus umi_language_intelligence_linked_editing_validate(const UmiLanguageIntelligenceLinkedEditing *value);
int umi_language_intelligence_linked_editing_is_nested(const UmiLanguageIntelligenceLinkedEditing *value);
#ifdef __cplusplus
}
#endif
#endif
