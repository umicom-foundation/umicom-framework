/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_document_colour.c
 * PURPOSE: Focused regression test for document colour.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/document_colour.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceDocumentColour colour;
    umi_language_intelligence_document_colour_init(&colour);
    umi_language_intelligence_document_colour_set(&colour, 1.2, 0.5, -0.1, 1.0);
    CHECK(umi_language_intelligence_document_colour_validate(&colour) != 0);
    CHECK(((umi_language_intelligence_document_colour_rgba8(&colour) >> 24) & 0xffU) == 255U);
    return 0;
}
