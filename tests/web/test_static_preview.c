/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web/test_static_preview.c
 * PURPOSE: Check complete static previews, rooted path admission, read-only methods and route priority using isolated native files.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/platform/rooted_files.h"
#include "umicom/platform/filesystem.h"
#include "umicom/web/connection.h"
#include "umicom/web/static_files.h"
#include <wchar.h>

/* Each case owns a fresh directory. No fixture opens a user project or removes
 * existing files; retained files help diagnose a failed owner-run test. */
static const char *Header(const UmiWebResponse *response, const char *name)
{
    for (size_t i = 0U; i < response->header_count; ++i)
        if (umi_web_header_name_equal(&response->headers[i], name))
            return response->headers[i].value;
    return "";
}
static UmiStatus Exact(const UmiWebRequest *request, UmiWebResponse *response, void *context)
{
    (void)request;
    if (context != NULL)
        return UMI_STATUS_IO_ERROR;
    return umi_web_response_set_text(response, 404, "text/plain", "intentional route result");
}
static int Link(const char *alias, const char *target, int directory)
{
#ifdef _WIN32
    wchar_t link_name[UMI_PATH_CAPACITY], target_name[UMI_PATH_CAPACITY];
    CHECK(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, alias, -1, link_name, (int)UMI_PATH_CAPACITY) >
          0);
    CHECK(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, target, -1, target_name,
                              (int)UMI_PATH_CAPACITY) > 0);
    DWORD flags = directory ? SYMBOLIC_LINK_FLAG_DIRECTORY : 0U;
    if (CreateSymbolicLinkW(link_name, target_name, flags | 2U))
        return 1;
    DWORD error = GetLastError();
    if (error == ERROR_INVALID_PARAMETER)
    {
        if (CreateSymbolicLinkW(link_name, target_name, flags))
            return 1;
        error = GetLastError();
    }
    if (error == ERROR_PRIVILEGE_NOT_HELD || error == ERROR_NOT_SUPPORTED || error == ERROR_INVALID_FUNCTION)
        return 0;
    CHECK(0);
    return 0;
#else
    (void)directory;
    CHECK(symlink(target, alias) == 0);
    return 1;
#endif
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    const char *cases[] = {"index",          "directory-index",
                           "html",           "css",
                           "binary",         "unicode",
                           "space",          "decode-once",
                           "empty",          "exact-limit",
                           "oversized",      "missing",
                           "missing-parent", "directory",
                           "traversal",      "encoded-traversal",
                           "encoded-slash",  "encoded-backslash",
                           "raw-backslash",  "nul",
                           "bad-percent",    "query",
                           "fragment",       "metadata",
                           "device",         "double-slash",
                           "dot-segment",    "trailing-dot",
                           "relative-root",  "root-overflow",
                           "root-copy",      "invalid-descriptor",
                           "path-overflow",  "head",
                           "post",           "fallback",
                           "exact-priority", "exact-failure",
                           "fallback-clear", "leaf-link",
                           "parent-link",    "root-link"};
    size_t matched = 0U;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        matched += (size_t)(strcmp(mode, cases[i]) == 0);
    if (matched != 1U)
        return 2;
    char root[UMI_PATH_CAPACITY], folder[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    FixturePath(folder, root, "assets");
    CHECK(umi_fs_make_directories(folder) == UMI_STATUS_OK);
    CHECK(UmiRootedFileWrite(root, "index.html", "<h1>root</h1>", 13U) == UMI_STATUS_OK);
    CHECK(UmiRootedFileWrite(root, "assets/index.html", "nested", 6U) == UMI_STATUS_OK);
    UmiWebStaticFiles files;
    memset(&files, 0, sizeof(files));
    CHECK(umi_web_static_files_init(&files, root) == UMI_STATUS_OK);
    UmiWebResponse *response = calloc(1U, sizeof(*response));
    CHECK(response != NULL);
    const char *request_path = "/", *expected = "<h1>root</h1>", *mime = "text/html; charset=utf-8";
    size_t expected_size = 13U;
    int expected_status = 200;
    unsigned char *allocated = NULL;
    const unsigned char binary[] = {0U, 255U, 10U, 0U, 13U, 128U};
    if (strcmp(mode, "directory-index") == 0)
    {
        request_path = "/assets/";
        expected = "nested";
        expected_size = 6U;
    }
    else if (strcmp(mode, "html") == 0)
        request_path = "/index.html";
    else if (strcmp(mode, "css") == 0)
    {
        CHECK(UmiRootedFileWrite(root, "assets/style.css", "body{}", 6U) == UMI_STATUS_OK);
        request_path = "/assets/style.css";
        expected = "body{}";
        expected_size = 6U;
        mime = "text/css; charset=utf-8";
    }
    else if (strcmp(mode, "binary") == 0)
    {
        CHECK(UmiRootedFileWrite(root, "image.png", binary, sizeof(binary)) == UMI_STATUS_OK);
        request_path = "/image.png";
        expected = (const char *)binary;
        expected_size = sizeof(binary);
        mime = "image/png";
    }
    else if (strcmp(mode, "unicode") == 0 || strcmp(mode, "space") == 0 || strcmp(mode, "decode-once") == 0)
    {
        const char *leaf = strcmp(mode, "unicode") == 0 ? "caf\xc3\xa9.txt"
                           : strcmp(mode, "space") == 0 ? "two words.txt"
                                                        : "%2e%2e.txt";
        CHECK(UmiRootedFileWrite(root, leaf, "text", 4U) == UMI_STATUS_OK);
        request_path = strcmp(mode, "unicode") == 0 ? "/caf%C3%A9.txt"
                       : strcmp(mode, "space") == 0 ? "/two%20words.txt"
                                                    : "/%252e%252e.txt";
        expected = "text";
        expected_size = 4U;
        mime = "text/plain; charset=utf-8";
    }
    else if (strcmp(mode, "empty") == 0)
    {
        CHECK(UmiRootedFileWrite(root, "empty.txt", NULL, 0U) == UMI_STATUS_OK);
        request_path = "/empty.txt";
        expected = "";
        expected_size = 0U;
        mime = "text/plain; charset=utf-8";
    }
    else if (strcmp(mode, "exact-limit") == 0 || strcmp(mode, "oversized") == 0)
    {
        size_t size = UMI_WEB_BODY_CAPACITY - (strcmp(mode, "exact-limit") == 0 ? 1U : 0U);
        allocated = malloc(size);
        CHECK(allocated != NULL);
        memset(allocated, 'x', size);
        CHECK(UmiRootedFileWrite(root, "large.bin", allocated, size) == UMI_STATUS_OK);
        request_path = "/large.bin";
        expected = (const char *)allocated;
        expected_size = size;
        mime = "application/octet-stream";
        if (strcmp(mode, "oversized") == 0)
            expected_status = 413;
    }
    else if (strcmp(mode, "missing") == 0)
    {
        request_path = "/absent.txt";
        expected_status = 404;
    }
    else if (strcmp(mode, "missing-parent") == 0)
    {
        request_path = "/absent/index.html";
        expected_status = 404;
    }
    else if (strcmp(mode, "directory") == 0)
    {
        request_path = "/assets";
        expected_status = 403;
    }
    else if (strcmp(mode, "relative-root") == 0 || strcmp(mode, "root-overflow") == 0)
    {
        UmiWebStaticFiles before = files;
        char huge[UMI_WEB_PATH_CAPACITY + 1U];
        memset(huge, 'x', sizeof(huge));
        huge[sizeof(huge) - 1U] = '\0';
        CHECK(umi_web_static_files_init(&files, strcmp(mode, "relative-root") == 0 ? "." : huge) !=
              UMI_STATUS_OK);
        CHECK(memcmp(&files, &before, sizeof(files)) == 0);
        CHECK(umi_web_static_files_init(NULL, root) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_web_static_files_init(&files, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    }
    else if (strcmp(mode, "root-copy") == 0)
    {
        char selected[UMI_PATH_CAPACITY];
        memcpy(selected, root, strlen(root) + 1U);
        CHECK(umi_web_static_files_init(&files, selected) == UMI_STATUS_OK);
        selected[0] = '!';
        CHECK(umi_web_static_files_init(&files, files.root) == UMI_STATUS_OK);
    }
    else if (strcmp(mode, "invalid-descriptor") == 0)
    {
        memset(files.root, 'x', sizeof(files.root));
        CHECK(umi_web_static_files_serve(&files, "/", response) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_web_static_files_init(&files, root) == UMI_STATUS_OK);
    }
    else if (strcmp(mode, "path-overflow") == 0)
    {
        char large[UMI_WEB_PATH_CAPACITY + 1U];
        memset(large, 'a', sizeof(large));
        large[0] = '/';
        large[sizeof(large) - 1U] = '\0';
        CHECK(umi_web_static_files_serve(&files, large, response) == UMI_STATUS_OK &&
              response->status == 403);
    }
    else if (strcmp(mode, "head") == 0 || strcmp(mode, "post") == 0)
    {
        UmiWebRequest *request = calloc(1U, sizeof(*request));
        CHECK(request != NULL);
        request->method = strcmp(mode, "head") == 0 ? UMI_HTTP_METHOD_HEAD : UMI_HTTP_METHOD_POST;
        strcpy(request->path, "/");
        CHECK(UmiWebStaticFilesHandle(request, response, &files) == UMI_STATUS_OK);
        if (strcmp(mode, "post") == 0)
            CHECK(response->status == 405 && strcmp(Header(response, "Allow"), "GET, HEAD") == 0);
        else
        {
            unsigned char wire[1024];
            size_t length = 0U;
            CHECK(response->status == 200 && response->body_length == 13U);
            CHECK(UmiWebResponseFormatClosed(response, true, wire, sizeof(wire) - 1U, &length) ==
                  UMI_STATUS_OK);
            wire[length] = 0U;
            CHECK(strstr((const char *)wire, "Content-Length: 13\r\n") != NULL);
            CHECK(length >= 4U && memcmp(wire + length - 4U, "\r\n\r\n", 4U) == 0);
        }
        free(request);
    }
    else if (strcmp(mode, "fallback") == 0 || strcmp(mode, "exact-priority") == 0 ||
             strcmp(mode, "exact-failure") == 0 || strcmp(mode, "fallback-clear") == 0)
    {
        UmiWebRouter *router = NULL;
        CHECK(umi_web_router_create(&router) == UMI_STATUS_OK);
        CHECK(UmiWebRouterSetFallback(router, UmiWebStaticFilesHandle, &files) == UMI_STATUS_OK);
        UmiWebRequest *request = calloc(1U, sizeof(*request));
        CHECK(request != NULL);
        request->method = UMI_HTTP_METHOD_GET;
        strcpy(request->path, "/");
        if (strcmp(mode, "fallback-clear") == 0)
            CHECK(UmiWebRouterSetFallback(router, NULL, &files) == UMI_STATUS_OK);
        if (strcmp(mode, "exact-priority") == 0 || strcmp(mode, "exact-failure") == 0)
        {
            UmiWebRoute route;
            CHECK(umi_web_route_init(&route, UMI_HTTP_METHOD_GET, "/", Exact,
                                     strcmp(mode, "exact-failure") == 0 ? &files : NULL) == UMI_STATUS_OK);
            CHECK(umi_web_router_add(router, &route) == UMI_STATUS_OK);
        }
        UmiStatus status = umi_web_router_dispatch(router, request, response);
        if (strcmp(mode, "exact-failure") == 0)
            CHECK(status == UMI_STATUS_IO_ERROR);
        else
        {
            CHECK(status == UMI_STATUS_OK);
            CHECK(response->status == (strcmp(mode, "fallback") == 0 ? 200 : 404));
            if (strcmp(mode, "exact-priority") == 0)
                CHECK(strcmp(response->body, "intentional route result") == 0);
        }
        CHECK(UmiWebRouterSetFallback(NULL, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        umi_web_router_destroy(router);
        free(request);
    }
    else if (strcmp(mode, "leaf-link") == 0 || strcmp(mode, "parent-link") == 0 ||
             strcmp(mode, "root-link") == 0)
    {
        FixturePath(path, root, "alias");
        char target[UMI_PATH_CAPACITY];
        FixturePath(target, root, "index.html");
        int directory = strcmp(mode, "leaf-link") != 0;
        if (!Link(path, directory ? folder : target, directory))
        {
            free(response);
            return 77;
        }
        request_path = directory ? "/alias/" : "/alias";
        if (strcmp(mode, "root-link") == 0)
        {
            CHECK(umi_web_static_files_init(&files, path) == UMI_STATUS_OK);
            request_path = "/";
        }
        expected_status = 403;
    }
    else
    {
        const struct
        {
            const char *mode, *path;
        } denied[] = {{"traversal", "/../index.html"},
                      {"encoded-traversal", "/%2e%2e/index.html"},
                      {"encoded-slash", "/assets%2Findex.html"},
                      {"encoded-backslash", "/assets%5cindex.html"},
                      {"raw-backslash", "/assets\\index.html"},
                      {"nul", "/index.html%00.txt"},
                      {"bad-percent", "/index%G1.html"},
                      {"query", "/index.html?key=value"},
                      {"fragment", "/index.html#top"},
                      {"metadata", "/.GiT/config"},
                      {"device", "/NUL.txt"},
                      {"double-slash", "//index.html"},
                      {"dot-segment", "/assets/./index.html"},
                      {"trailing-dot", "/index.html."}};
        for (size_t i = 0U; i < sizeof(denied) / sizeof(denied[0]); ++i)
            if (strcmp(mode, denied[i].mode) == 0)
            {
                request_path = denied[i].path;
                expected_status = 403;
            }
    }
    CHECK(umi_web_static_files_serve(&files, request_path, response) == UMI_STATUS_OK);
    CHECK(response->status == expected_status);
    if (expected_status == 200)
    {
        CHECK(response->body_length == expected_size && memcmp(response->body, expected, expected_size) == 0);
        CHECK(strcmp(Header(response, "Content-Type"), mime) == 0);
        CHECK(strcmp(Header(response, "X-Content-Type-Options"), "nosniff") == 0);
        CHECK(strcmp(Header(response, "Cache-Control"), "no-store") == 0);
    }
    CHECK(response->body_length < UMI_WEB_BODY_CAPACITY);
    free(allocated);
    free(response);
    return 0;
}
