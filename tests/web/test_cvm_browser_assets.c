/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web/test_cvm_browser_assets.c
 *
 * PURPOSE:
 *   Verify the normal loopback demo's worker resource routes without network I/O.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Use the actual sample's immutable fallback with its generated SDK resources. */
#define main LocalServiceEntry
#include "../../examples/web/local_service.c"
#undef main

static bool HasHeader(const UmiWebResponse *response, const char *name, const char *value)
{
    for (size_t index = 0U; index < response->header_count; ++index) {
        const UmiWebHeader *header = &response->headers[index];
        if (strcmp(header->name, name) == 0 && strcmp(header->value, value) == 0)
            return true;
    }
    return false;
}

/* HEAD must retain the exact metadata and lease length for the transport.
 * POST is refused before any immutable resource body is acquired. */
static bool CheckRoute(UmiWebRouter *router, UmiWebRequest *request, UmiWebResponse *response,
    UmiHttpMethod method, const char *path, const unsigned char *expected, size_t length)
{
    UmiWebResponseBody *body = NULL;
    if (umi_web_copy_text(request->path, sizeof request->path, path) != UMI_STATUS_OK)
        return false;
    request->method = method;
    bool passed = BrandAsset(request, response, &body, router) == UMI_STATUS_OK;
    if (passed && method == UMI_HTTP_METHOD_POST) {
        passed = response->status == 405 && body == NULL && HasHeader(response, "Allow", "GET, HEAD");
    } else if (passed) {
        passed = response->status == 200 && UmiWebResponseBodyLength(body) == length &&
            memcmp(UmiWebResponseBodyBytes(body), expected, length) == 0 &&
            HasHeader(response, "Content-Type", "text/javascript; charset=utf-8") &&
            HasHeader(response, "X-Content-Type-Options", "nosniff") &&
            HasHeader(response, "Cache-Control", "no-store") &&
            HasHeader(response, "Content-Security-Policy",
                "default-src 'none'; script-src 'self'; connect-src 'none'; worker-src 'none'");
    }
    if (!passed) fprintf(stderr, "%s %s worker resource check failed\n", umi_http_method_text(method), path);
    UmiWebResponseBodyDestroy(body);
    return passed;
}

int main(void)
{
    UmiWebRouter *router = NULL;
    UmiWebRequest *request = calloc(1U, sizeof(*request));
    UmiWebResponse *response = calloc(1U, sizeof(*response));
    bool passed = request != NULL && response != NULL && umi_web_router_create(&router) == UMI_STATUS_OK;
    if (passed) {
        /* Check both SDK modules and preserve the default router for other URLs. */
        passed = CheckRoute(router, request, response, UMI_HTTP_METHOD_GET, "/cvm/browser/worker.mjs",
                            UmiWebCvmBrowserWorker, sizeof UmiWebCvmBrowserWorker) &&
            CheckRoute(router, request, response, UMI_HTTP_METHOD_HEAD, "/cvm/browser/arithmetic.mjs",
                       UmiWebCvmBrowserArithmetic, sizeof UmiWebCvmBrowserArithmetic) &&
            CheckRoute(router, request, response, UMI_HTTP_METHOD_POST, "/cvm/browser/worker.mjs", NULL, 0U);
        UmiWebResponseBody *body = NULL;
        request->method = UMI_HTTP_METHOD_GET;
        passed = passed && umi_web_copy_text(request->path, sizeof request->path, "/cvm/browser/absent.mjs") == UMI_STATUS_OK &&
            BrandAsset(request, response, &body, router) == UMI_STATUS_OK && response->status == 404 && body == NULL;
        UmiWebResponseBodyDestroy(body);
    }
    if (router != NULL) umi_web_router_destroy(router);
    free(response);
    free(request);
    return passed ? 0 : 1;
}
