/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/document_colour.c
 *
 * PURPOSE:
 *   Implement represent parsed document colours using clamped RGBA values.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/document_colour.h"
#include <string.h>
/* Provide the clamp unit operation used by this module and its client applications. */
static double clamp_unit(double value)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (value < 0.0) return 0.0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (value > 1.0) return 1.0;
    return value;
}
/*
 * Initialise language intelligence document colour from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_intelligence_document_colour_init(UmiLanguageIntelligenceDocumentColour *colour)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (colour == NULL) return;
    (void)memset(colour, 0, sizeof(*colour));
    colour->struct_size = (uint32_t)sizeof(*colour);
    colour->api_version = UMI_LANGUAGE_INTELLIGENCE_DOCUMENT_COLOUR_API_VERSION;
    colour->alpha = 1.0;
}
/*
 * Copy language intelligence document colour into module-owned storage so callers keep
 * ownership of their input values.
 */
void umi_language_intelligence_document_colour_set(
    UmiLanguageIntelligenceDocumentColour *colour,
    double red, double green, double blue, double alpha)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (colour == NULL) return;
    colour->red = clamp_unit(red);
    colour->green = clamp_unit(green);
    colour->blue = clamp_unit(blue);
    colour->alpha = clamp_unit(alpha);
}
/*
 * Check that language intelligence document colour satisfies its contract before another
 * service relies on it.
 */
int umi_language_intelligence_document_colour_validate(const UmiLanguageIntelligenceDocumentColour *colour)
{
    return colour != NULL &&
        colour->red >= 0.0 && colour->red <= 1.0 &&
        colour->green >= 0.0 && colour->green <= 1.0 &&
        colour->blue >= 0.0 && colour->blue <= 1.0 &&
        colour->alpha >= 0.0 && colour->alpha <= 1.0;
}
/*
 * Provide the language intelligence document colour rgba8 operation used by this module
 * and its client applications.
 */
uint32_t umi_language_intelligence_document_colour_rgba8(const UmiLanguageIntelligenceDocumentColour *colour)
{
    uint32_t r, g, b, a;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_language_intelligence_document_colour_validate(colour)) return 0U;
    r = (uint32_t)(colour->red * 255.0 + 0.5);
    g = (uint32_t)(colour->green * 255.0 + 0.5);
    b = (uint32_t)(colour->blue * 255.0 + 0.5);
    a = (uint32_t)(colour->alpha * 255.0 + 0.5);
    return (r << 24) | (g << 16) | (b << 8) | a;
}
