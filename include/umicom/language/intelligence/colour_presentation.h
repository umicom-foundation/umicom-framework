/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/colour_presentation.h
 *
 * PURPOSE:
 *   Represent provider-neutral colour presentation edits.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_COLOUR_PRESENTATION_H
#define UMICOM_LANGUAGE_INTELLIGENCE_COLOUR_PRESENTATION_H

#include "umicom/language/intelligence/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_INTELLIGENCE_COLOUR_PRESENTATION_API_VERSION 1U

typedef struct UmiLanguageIntelligenceColourPresentation {
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
} UmiLanguageIntelligenceColourPresentation;

void umi_language_intelligence_colour_presentation_init(
    UmiLanguageIntelligenceColourPresentation *value,
    const char *id);
UmiStatus umi_language_intelligence_colour_presentation_validate(
    const UmiLanguageIntelligenceColourPresentation *value);
UmiStatus umi_language_intelligence_colour_presentation_set_subject(
    UmiLanguageIntelligenceColourPresentation *value,
    const char *subject_id);
UmiStatus umi_language_intelligence_colour_presentation_set_detail(
    UmiLanguageIntelligenceColourPresentation *value,
    const char *detail);
void umi_language_intelligence_colour_presentation_touch(UmiLanguageIntelligenceColourPresentation *value);
int umi_language_intelligence_colour_presentation_same_identity(
    const UmiLanguageIntelligenceColourPresentation *left,
    const UmiLanguageIntelligenceColourPresentation *right);

#ifdef __cplusplus
}
#endif
#endif
