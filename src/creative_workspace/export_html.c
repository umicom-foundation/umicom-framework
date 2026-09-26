/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/export_html.c
 * PURPOSE: Make offline visual prototypes and three-frame storyboards from actual scene samples.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
static void List(UmiCreativeWriter * w, const UmiCreativeProject * p)
{
    UmiCreativeWriterText(w, "<h2>Objects in this design</h2><ol>");
    for (size_t i = 0U;i < p -> elementCount; ++ i){
        const UmiCreativeElement * e = & p -> elements[i];
        UmiCreativeWriterText(w, "<li><strong>");
        UmiCreativeWriterXml(w, e -> label);
        UmiCreativeWriterText(w, "</strong> — ");
        UmiCreativeWriterText(w, UmiCreativeElementKindText(e -> kind));
        UmiCreativeWriterText(w, ", ");
        UmiCreativeWriterNumber(w, (uint64_t)e -> width);
        UmiCreativeWriterText(w, " × ");
        UmiCreativeWriterNumber(w, (uint64_t)e -> height);
        UmiCreativeWriterText(w, " ");
        UmiCreativeWriterText(w, UmiCreativeUnitText(p -> settings.unit));
        if (!e -> visible)UmiCreativeWriterText(w, " (hidden)");
        UmiCreativeWriterText(w, ".</li>");
    }
    UmiCreativeWriterText(w, "</ol>");
}

UmiStatus UmiCreativeExportHtml(const UmiCreativeProject * p, uint32_t timeMs, bool storyboard, UmiCreativeExport * out)
{
    UmiCreativeWriter w;
    UmiCreativeWriterInit( & w, 1024U * 1024U);
    UmiCreativeWriterText( & w, "<!doctype html><html lang=\"en-GB\"><head><meta charset=\"utf-8\"><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
    "<meta http-equiv=\"Content-Security-Policy\" content=\"default-src 'none'; img-src data:; style-src 'unsafe-inline'\"><title>");
    UmiCreativeWriterXml( & w, p -> settings.title);
    UmiCreativeWriterText( & w, "</title><link rel=\"icon\" href=\"");
    UmiCreativeWriterBrandIcon( & w);
    UmiCreativeWriterText( & w, "\"><style>body{margin:0;background:#f2f5f7;color:#193043;font:17px/1.65 system-ui,sans-serif}"
    "header{background:#112c3d;padding:18px 5vw}header img{width:170px;height:auto}main{max-width:1080px;margin:auto;padding:28px 5vw}"
    "h1{line-height:1.18}figure{margin:24px 0;background:white;border:1px solid #b8cbd5;padding:14px;border-radius:10px}"
    "figure>svg{display:block;width:100%;height:auto}figcaption{padding:12px 0 2px}li{margin:8px 0}footer{border-top:1px solid #b8cbd5;margin-top:32px;padding-top:16px}"
    "@media print{body{background:white}figure{break-inside:avoid}}</style></head><body><header><img alt=\"Umicom\" src=\"");
    UmiCreativeWriterBrandLogo( & w);
    UmiCreativeWriterText( & w, "\"></header><main><h1>");
    UmiCreativeWriterXml( & w, p -> settings.title);
    UmiCreativeWriterText( & w, "</h1><p>");
    UmiCreativeWriterText( & w, storyboard ? "Three frames from the saved animation. This is a storyboard, not an encoded video." :
    "An offline visual prototype. Resize the browser to fit the drawing to the available space. This page does not submit forms or run an application.");
    UmiCreativeWriterText( & w, "</p>");
    uint32_t times[3] = {0U, p -> settings.durationMs / 2U, p -> settings.durationMs};
    size_t count = storyboard ? 3U : 1U;
    for (size_t i = 0U;i < count && w.status == UMI_STATUS_OK; ++ i){
        uint32_t sample = storyboard ? times[i] : timeMs;
        UmiCreativeExport svg = {0};
        UmiStatus status = UmiCreativeExportSvg(p, sample, & svg);
        if (status!= UMI_STATUS_OK){
            w.status = status;
            break;
        }
        UmiCreativeWriterText( & w, "<figure>");
        UmiCreativeWriterAppend( & w, svg.bytes, svg.size);
        UmiCreativeExportFree( & svg);
        UmiCreativeWriterText( & w, "<figcaption>Time: ");
        UmiCreativeWriterNumber( & w, sample);
        UmiCreativeWriterText( & w, " ms. Drawing units: ");
        UmiCreativeWriterText( & w, UmiCreativeUnitText(p -> settings.unit));
        UmiCreativeWriterText( & w, ".</figcaption></figure>");
    }
    List( & w, p);
    UmiCreativeWriterText( & w, "<footer>Created with Umicom. Project <strong>");
    UmiCreativeWriterXml( & w, p -> id);
    UmiCreativeWriterText( & w, "</strong>, source revision ");
    UmiCreativeWriterNumber( & w, p -> revision);
    UmiCreativeWriterText( & w, ". A visual design is not a manufacturing specification or an approved building plan.</footer></main></body></html>\n");
    return UmiCreativeWriterFinish( & w, out);
}

