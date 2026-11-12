/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/source_contracts/guide_branding_fixture.h
 *
 * PURPOSE:
 *   Share small independent binary identities for native guide-audit regressions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_TEST_GUIDE_BRANDING_FIXTURE_H
#define UMICOM_TEST_GUIDE_BRANDING_FIXTURE_H
#include "umicom/source_contracts/source_contracts.h"
#include <stdio.h>
#include <string.h>
/* These three-byte fixtures exercise identity, not image rendering. Their
 * digests are fixed expected values, independent of the production hasher. */
static const unsigned char GbTestIco[] = {0U, 0U, 1U};
static const unsigned char GbTestLight[] = {1U, 2U, 3U};
static const unsigned char GbTestDark[] = {4U, 5U, 6U};
static const unsigned char GbTestSymbol[] = {7U, 8U, 9U};
static inline UmiSourceBrandAssets GbTestAssets(void)
{
    return (UmiSourceBrandAssets){{GbTestIco, 3U}, {GbTestLight, 3U},
        {GbTestDark, 3U}, {GbTestSymbol, 3U}};
}
static inline int GbTestDocument(char *out, size_t capacity, bool light)
{
    return snprintf(out, capacity,
        "<!doctype html><html><head>"
        "<meta name=\"umicom-guide-branding\" content=\"official-assets\">"
        "<link rel=\"icon\" type=\"image/x-icon\" data-umicom-guide-ico=\"cf7605ed1bc735f6c825554154627467e1cac9df54cee8699218ed434603c568\" href=\"data:image/x-icon;base64,AAAB\">"
        "<style id=\"umicom-guide-fonts\">"
        "body{font-family:\"Segoe UI Variable Text\",\"Segoe UI\",sans-serif}"
        "pre,code,kbd,samp{font-family:\"Cascadia Code\",Consolas,monospace}</style></head><body>"
        "<img alt=\"Umicom\" data-umicom-guide-logo=\"%s\" src=\"data:image/svg+xml;base64,%s\" data-umicom-guide-surface=\"%s\">"
        "<main><h1>Learn C</h1><p>Keep source ownership explicit.</p></main></body></html>",
        light ? "039058c6f2c0cb492c533b0a4d14ef77cc0f78abccced5287d84a1a2011cfb81" : "787c798e39a5bc1910355bae6d0cd87a36b2e10fd0202a83e3bb6b005da83472", light ? "AQID" : "BAUG", light ? "light" : "dark");
}
#endif
