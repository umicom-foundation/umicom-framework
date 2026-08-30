/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/inline_value.h
 *
 * PURPOSE:
 *   Represent debug/language inline values with stable document ranges.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_INLINE_VALUE_H
#define UMICOM_LANGUAGE_INTELLIGENCE_INLINE_VALUE_H

#include "umicom/language/intelligence/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_INTELLIGENCE_INLINE_VALUE_API_VERSION 1U

typedef struct UmiLanguageIntelligenceInlineValue {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char subject_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char detail[UMI_LANGUAGE_INTELLIGENCE_TEXT_CAPACITY];
    UmiLanguageIntelligenceRange range;
    uint64_t revision;
    uint32_t priority;
    uint32_t flags;
    int enabled;
} UmiLanguageIntelligenceInlineValue;

void umi_language_intelligence_inline_value_init(
    UmiLanguageIntelligenceInlineValue *value,
    const char *id);
UmiStatus umi_language_intelligence_inline_value_validate(
    const UmiLanguageIntelligenceInlineValue *value);
UmiStatus umi_language_intelligence_inline_value_set_subject(
    UmiLanguageIntelligenceInlineValue *value,
    const char *subject_id);
UmiStatus umi_language_intelligence_inline_value_set_detail(
    UmiLanguageIntelligenceInlineValue *value,
    const char *detail);
void umi_language_intelligence_inline_value_touch(UmiLanguageIntelligenceInlineValue *value);
int umi_language_intelligence_inline_value_same_identity(
    const UmiLanguageIntelligenceInlineValue *left,
    const UmiLanguageIntelligenceInlineValue *right);

#ifdef __cplusplus
}
#endif
#endif
