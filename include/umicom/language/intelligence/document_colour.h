/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/document_colour.h
 *
 * PURPOSE:
 *   Represent parsed document colours using clamped RGBA values.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_DOCUMENT_COLOUR_H
#define UMICOM_LANGUAGE_INTELLIGENCE_DOCUMENT_COLOUR_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_DOCUMENT_COLOUR_API_VERSION 1U
typedef struct UmiLanguageIntelligenceDocumentColour {
    uint32_t struct_size;
    uint32_t api_version;
    double red;
    double green;
    double blue;
    double alpha;
    UmiLanguageIntelligenceRange range;
} UmiLanguageIntelligenceDocumentColour;
void umi_language_intelligence_document_colour_init(UmiLanguageIntelligenceDocumentColour *colour);
void umi_language_intelligence_document_colour_set(
    UmiLanguageIntelligenceDocumentColour *colour,
    double red, double green, double blue, double alpha);
int umi_language_intelligence_document_colour_validate(const UmiLanguageIntelligenceDocumentColour *colour);
uint32_t umi_language_intelligence_document_colour_rgba8(const UmiLanguageIntelligenceDocumentColour *colour);
#ifdef __cplusplus
}
#endif
#endif
