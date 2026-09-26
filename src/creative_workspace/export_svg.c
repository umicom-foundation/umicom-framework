/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/export_svg.c
 * PURPOSE: Render scene snapshots without external references, scripts or locale-dependent numbers.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
/* Animation coordinates are rounded to 0.001 document units. Formatting uses
 * integer digits and a literal decimal point, irrespective of the host locale. */
static void Coordinate(UmiCreativeWriter * w, double value)
{
    if (!isfinite(value) || value < 0.0 || value > UMI_CREATIVE_MAX_EXTENT){
        w -> status = UMI_STATUS_INVALID_ARGUMENT;
        return;
    }
    uint64_t scaled = (uint64_t)floor(value * 1000.0 + 0.5);
    UmiCreativeWriterNumber(w, scaled / 1000U);
    uint64_t fraction = scaled % 1000U;
    if (fraction!= 0U){
        char suffix[8];
        (void)snprintf(suffix, sizeof(suffix), ".%03" PRIu64, fraction);
        UmiCreativeWriterText(w, suffix);
    }
}

static void Attribute(UmiCreativeWriter * w, const char * name, double value)
{
    UmiCreativeWriterText(w, " ");
    UmiCreativeWriterText(w, name);
    UmiCreativeWriterText(w, "=\"");
    Coordinate(w, value);
    UmiCreativeWriterText(w, "\"");
}

static void Colour(UmiCreativeWriter * w, uint32_t rgb)
{
    char text[8];
    (void)snprintf(text, sizeof(text), "#%06" PRIx32, rgb);
    UmiCreativeWriterText(w, text);
}

UmiStatus UmiCreativeExportSvg(const UmiCreativeProject * p, uint32_t timeMs, UmiCreativeExport * out)
{
    UmiCreativeFrame frame;
    UmiStatus status = UmiCreativeSceneSample(p, timeMs, & frame);
    if (status!= UMI_STATUS_OK)return status;
    UmiCreativeWriter w;
    UmiCreativeWriterInit( & w, 256U * 1024U);
    UmiCreativeWriterText( & w, "<svg xmlns=\"http://www.w3.org/2000/svg\" role=\"img\" viewBox=\"0 0 ");
    UmiCreativeWriterNumber( & w, (uint64_t)p -> settings.width);
    UmiCreativeWriterText( & w, " ");
    UmiCreativeWriterNumber( & w, (uint64_t)p -> settings.height);
    UmiCreativeWriterText( & w, "\" width=\"");
    UmiCreativeWriterNumber( & w, (uint64_t)p -> settings.width);
    if (p -> settings.unit == UMI_CREATIVE_MILLIMETRES)UmiCreativeWriterText( & w, "mm");
    UmiCreativeWriterText( & w, "\" height=\"");
    UmiCreativeWriterNumber( & w, (uint64_t)p -> settings.height);
    if (p -> settings.unit == UMI_CREATIVE_MILLIMETRES)UmiCreativeWriterText( & w, "mm");
    UmiCreativeWriterText( & w, "\"><title>");
    UmiCreativeWriterXml( & w, p -> settings.title);
    UmiCreativeWriterText( & w, "</title><desc>Umicom project ");
    UmiCreativeWriterXml( & w, p -> id);
    UmiCreativeWriterText( & w, ", revision ");
    UmiCreativeWriterNumber( & w, p -> revision);
    UmiCreativeWriterText( & w, ", time ");
    UmiCreativeWriterNumber( & w, timeMs);
    UmiCreativeWriterText( & w, " milliseconds. Units: ");
    UmiCreativeWriterText( & w, UmiCreativeUnitText(p -> settings.unit));
    UmiCreativeWriterText( & w, ".</desc><rect width=\"100%\" height=\"100%\" fill=\"white\"/>\n");
    for (size_t i = 0U;i < frame.elementCount; ++ i){
        const UmiCreativeFrameElement * e = & frame.elements[i];
        if (!e -> visible)continue;
        UmiCreativeWriterText( & w, "<g data-element=\"");
        UmiCreativeWriterXml( & w, e -> id);
        UmiCreativeWriterText( & w, "\"><title>");
        UmiCreativeWriterXml( & w, e -> label);
        UmiCreativeWriterText( & w, "</title>");
        if (e -> kind!= UMI_CREATIVE_TEXT){
            UmiCreativeWriterText( & w, "<rect");
            Attribute( & w, "x", e -> x);
            Attribute( & w, "y", e -> y);
            Attribute( & w, "width", e -> width);
            Attribute( & w, "height", e -> height);
            UmiCreativeWriterText( & w, " fill=\"");
            Colour( & w, e -> colourRgb);
            UmiCreativeWriterText( & w, "\" stroke=\"#243a49\" stroke-width=\"1\"/>");
        }
        double font = (double)p -> settings.width / 45.0;
        if (font < 12.0)font = 12.0;
        if (font > 80.0)font = 80.0;
        if (font > e -> height * 0.5)font = e -> height * 0.5;
        /* Nested viewports clip long labels without document-global clip IDs. */
        UmiCreativeWriterText( & w, "<svg overflow=\"hidden\"");
        Attribute( & w, "x", e -> x);
        Attribute( & w, "y", e -> y);
        Attribute( & w, "width", e -> width);
        Attribute( & w, "height", e -> height);
        UmiCreativeWriterText( & w, "><text font-family=\"sans-serif\"");
        Attribute( & w, "x", font * 0.3);
        Attribute( & w, "y", font * 1.2);
        Attribute( & w, "font-size", font);
        UmiCreativeWriterText( & w, " fill=\"");
        Colour( & w, e -> kind == UMI_CREATIVE_TEXT ? e -> colourRgb : 0x102532U);
        UmiCreativeWriterText( & w, "\">");
        UmiCreativeWriterXml( & w, e -> label);
        UmiCreativeWriterText( & w, "</text></svg></g>\n");
    }
    UmiCreativeWriterText( & w, "</svg>\n");
    return UmiCreativeWriterFinish( & w, out);
}

