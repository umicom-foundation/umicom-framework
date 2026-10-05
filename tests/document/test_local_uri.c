/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_local_uri.c
 * PURPOSE: Check local URI decoding, unchanged failure outputs and explicit refusal of ambiguous resources.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/local_uri.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(c))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c);                                          \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
#ifdef _WIN32
#define URI_ROOT "file:///C:/"
#define LOCAL_PATH "C:\\"
#else
#define URI_ROOT "file:///"
#define LOCAL_PATH "/"
#endif
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {
        "plain",           "space",       "unicode",      "localhost",        "case",
        "alias",           "capacity",    "nul",          "control",          "separator",
        "backslash",       "escape",      "utf8",         "authority",        "scheme",
        "query",           "fragment",    "raw-space",    "relative",         "network",
        "limits",          "arguments",   "device",       "device-extension", "device-port",
        "console",         "superscript", "trailing-dot", "trailing-space",   "alternate-stream",
        "encoded-question"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    const char *uri = URI_ROOT "source.c", *path = LOCAL_PATH "source.c";
    UmiStatus expected = UMI_STATUS_OK;
    char output[UMI_PATH_CAPACITY] = "unchanged", *owned = NULL;
    size_t capacity = sizeof(output);
    if (strcmp(mode, "space") == 0)
    {
        uri = URI_ROOT "source%20file.c";
        path = LOCAL_PATH "source file.c";
    }
    if (strcmp(mode, "unicode") == 0)
    {
        uri = URI_ROOT "caf%C3%A9.c";
        path = LOCAL_PATH "caf\xc3\xa9.c";
    }
    if (strcmp(mode, "localhost") == 0 || strcmp(mode, "case") == 0)
    {
#ifdef _WIN32
        uri = strcmp(mode, "case") == 0 ? "FILE://LOCALHOST/C:/source.c" : "file://localhost/C:/source.c";
#else
        uri = strcmp(mode, "case") == 0 ? "FILE://LOCALHOST/source.c" : "file://localhost/source.c";
#endif
    }
    if (strcmp(mode, "nul") == 0)
    {
        uri = URI_ROOT "good%00wrong";
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "control") == 0)
    {
        uri = URI_ROOT "bad%0Aname";
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "separator") == 0)
    {
        uri = URI_ROOT "folder%2Fsource.c";
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "backslash") == 0)
    {
        uri = URI_ROOT "folder%5Csource.c";
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "escape") == 0)
    {
        uri = URI_ROOT "bad%2";
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "utf8") == 0)
    {
        uri = URI_ROOT "bad%C0%80";
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "authority") == 0)
    {
        uri = "file://remote/share/source.c";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (strcmp(mode, "scheme") == 0)
    {
        uri = "https://example.invalid/source.c";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (strcmp(mode, "query") == 0)
    {
        uri = URI_ROOT "source.c?other";
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "fragment") == 0)
    {
        uri = URI_ROOT "source.c#other";
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "raw-space") == 0)
    {
        uri = URI_ROOT "source file.c";
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "relative") == 0)
    {
        uri = "source.c";
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "network") == 0)
    {
        uri = "file:////server/share/source.c";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (strcmp(mode, "capacity") == 0)
    {
        capacity = 2U;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strcmp(mode, "limits") == 0)
    {
        owned = malloc(8194U);
        CHECK(owned != NULL);
        memset(owned, 'x', 8193U);
        owned[8193] = '\0';
        uri = owned;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Syntax checks do not access native devices. Other hosts may use these
     * ordinary filename bytes; only the Windows policy refuses them. */
    if (strcmp(mode, "device") == 0)
    {
        uri = URI_ROOT "CON";
        path = LOCAL_PATH "CON";
#ifdef _WIN32
        expected = UMI_STATUS_INVALID_ARGUMENT;
#endif
    }
    if (strcmp(mode, "device-extension") == 0)
    {
        uri = URI_ROOT "nul.c";
        path = LOCAL_PATH "nul.c";
#ifdef _WIN32
        expected = UMI_STATUS_INVALID_ARGUMENT;
#endif
    }
    if (strcmp(mode, "device-port") == 0)
    {
        uri = URI_ROOT "COM1.txt";
        path = LOCAL_PATH "COM1.txt";
#ifdef _WIN32
        expected = UMI_STATUS_INVALID_ARGUMENT;
#endif
    }
    if (strcmp(mode, "console") == 0)
    {
        uri = URI_ROOT "CONIN$";
        path = LOCAL_PATH "CONIN$";
#ifdef _WIN32
        expected = UMI_STATUS_INVALID_ARGUMENT;
#endif
    }
    if (strcmp(mode, "superscript") == 0)
    {
        uri = URI_ROOT "LPT%C2%B2.c";
        path = LOCAL_PATH "LPT\xc2\xb2.c";
#ifdef _WIN32
        expected = UMI_STATUS_INVALID_ARGUMENT;
#endif
    }
    if (strcmp(mode, "trailing-dot") == 0)
    {
        uri = URI_ROOT "source.c.";
        path = LOCAL_PATH "source.c.";
#ifdef _WIN32
        expected = UMI_STATUS_INVALID_ARGUMENT;
#endif
    }
    if (strcmp(mode, "trailing-space") == 0)
    {
        uri = URI_ROOT "source.c%20";
        path = LOCAL_PATH "source.c ";
#ifdef _WIN32
        expected = UMI_STATUS_INVALID_ARGUMENT;
#endif
    }
    if (strcmp(mode, "alternate-stream") == 0)
    {
        uri = URI_ROOT "source.c:other";
        path = LOCAL_PATH "source.c:other";
#ifdef _WIN32
        expected = UMI_STATUS_INVALID_ARGUMENT;
#endif
    }
    if (strcmp(mode, "encoded-question") == 0)
    {
        uri = URI_ROOT "source%3F.c";
        path = LOCAL_PATH "source?.c";
#ifdef _WIN32
        expected = UMI_STATUS_INVALID_ARGUMENT;
#endif
    }
    if (strcmp(mode, "alias") == 0)
    {
        strcpy(output, uri);
        uri = output;
    }
    CHECK(UmiDocumentLocalFileUriToPath(uri, output, capacity) == expected);
    if (expected == UMI_STATUS_OK)
        CHECK(strcmp(output, path) == 0);
    else
        CHECK(strcmp(output, "unchanged") == 0);
    if (strcmp(mode, "arguments") == 0)
    {
        CHECK(UmiDocumentLocalFileUriToPath(NULL, output, sizeof(output)) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentLocalFileUriToPath(uri, NULL, sizeof(output)) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentLocalFileUriToPath(uri, output, 0U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(strcmp(output, path) == 0);
    }
    free(owned);
    return 0;
}
