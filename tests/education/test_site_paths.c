/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education/test_site_paths.c
 * PURPOSE: Check exact URL-path denial before the preview touches a file.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/site.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void Accept(const char *request, const char *expected)
{
    char path[UMICOM_EDUCATION_PATH_CAPACITY];
    assert(UmicomEducationResolveUrlPath(request, path, sizeof(path)) == UMICOM_EDUCATION_PATH_OK);
    assert(strcmp(path, expected) == 0);
}
static void Refuse(const char *request)
{
    char path[UMICOM_EDUCATION_PATH_CAPACITY];
    assert(UmicomEducationResolveUrlPath(request, path, sizeof(path)) != UMICOM_EDUCATION_PATH_OK);
}
int main(void)
{
    Accept("/", "index.html");
    Accept("/intro/", "intro/index.html");
    Accept("/intro/index.html?query=1", "intro/index.html");
    Accept("/assets/umicom%2dlogo.svg", "assets/umicom-logo.svg");
    Accept("/file.css", "file.css");
    Refuse("/../hidden");
    Refuse("/.git/config");
    Refuse("/folder/.env");
    Refuse("//index.html");
    Refuse("/folder//index.html");
    Refuse("/folder/%2e%2e/file");
    Refuse("/folder/%2Fetc");
    Refuse("/folder/%5cetc");
    Refuse("/folder/%252e%252e");
    Refuse("/bad%GG");
    Refuse("/%");
    Refuse("/%4");
    Refuse("/file:stream");
    Refuse("/trailing.");
    Refuse("/trailing ");
    Refuse("/con");
    Refuse("/CON.txt");
    Refuse("/LPT1.log");
    Refuse("/file\\private");
    Refuse("/x%00.html");
    Refuse("http://remote/path");
    char tiny[12];
    assert(UmicomEducationResolveUrlPath("/much-longer-name", tiny, sizeof(tiny)) ==
           UMICOM_EDUCATION_PATH_TOO_LONG);
    assert(strcmp(UmicomEducationMimeType("index.html"), "text/html; charset=utf-8") == 0);
    assert(strcmp(UmicomEducationMimeType("image.svg"), "image/svg+xml") == 0);
    assert(strcmp(UmicomEducationMimeType("asset.unknown"), "application/octet-stream") == 0);
    puts("URL decoding, rooted path boundaries and MIME checks passed.");
    return 0;
}
