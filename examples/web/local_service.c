/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/web/local_service.c
 *
 * PURPOSE:
 *   Serve a small C-authored website and API through Framework routing on an explicitly chosen loopback port.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/server.h"
#include "umicom/web/preview.h"
#include "umicom/platform/native_arguments.h"
#include "umicom/web/static_files.h"
#include "umicom/web/loopback_access.h"
#ifdef UMICOM_LOCAL_WEB_BRANDING
#include "umicom_web_brand_resources.h"
#endif
#ifdef UMICOM_LOCAL_WEB_CVM_BROWSER
#include "umicom_web_cvm_browser_resources.h"
#endif
#ifdef UMICOM_HAS_FASTCGI_LOOPBACK
#include "umicom/platform/filesystem.h"
#include "umicom/web/fastcgi_loopback.h"
#endif
#ifdef UMICOM_LOCAL_WEB_CVM_PREVIEW
#include "umicom/web/cvm_preview.h"
#endif
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static volatile sig_atomic_t stop_requested = 0;
static void Stop(int signal_number)
{
    (void)signal_number;
    stop_requested = 1;
}
/* HTML belongs to this example, while sockets, HTTP framing and route ownership
 * remain Framework services. Add a route below to expose a new C operation. */
static UmiStatus Home(const UmiWebRequest *request, UmiWebResponse *response, void *context)
{
    (void)request;
    (void)context;
    UmiStatus status = umi_web_response_set_text(
        response, 200, "text/html; charset=utf-8",
        "<!doctype html><html lang=\"en\"><meta charset=\"utf-8\">"
        "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
        "<title>Umicom local web service</title>"
#ifdef UMICOM_LOCAL_WEB_BRANDING
        /* The demo uses the shared SVG for the wordmark and both favicon formats. */
        "<link rel=\"icon\" type=\"image/vnd.microsoft.icon\" href=\"/favicon.ico\">"
        "<link rel=\"icon\" type=\"image/svg+xml\" href=\"/branding/umicom-icon.svg\">"
        "<link rel=\"stylesheet\" href=\"/branding/local-service.css\">"
#endif
        "<body><main>"
#ifdef UMICOM_LOCAL_WEB_BRANDING
        "<img src=\"/branding/umicom-logo.svg\" width=\"180\" height=\"44\" alt=\"Umicom\">"
#endif
        "<h1>Umicom local web service</h1><p>This page is served by C code using Umicom Framework.</p>"
        "<p><a href=\"/api/health\">Read the JSON health endpoint</a></p>"
        "<form method=\"post\" action=\"/api/echo\"><label>Text to send <input name=\"message\" "
        "maxlength=\"200\" required></label>"
        "<button type=\"submit\">Send to the C handler</button></form>"
        "<p>The echo endpoint displays form-encoded text. It does not save it or run commands.</p>"
        "<p>Use your terminal to stop the local server with Ctrl+C.</p></main></body></html>");
    if (status == UMI_STATUS_OK)
        status = umi_web_response_set_header(
            response, "Content-Security-Policy",
            "default-src 'none'; "
#ifdef UMICOM_LOCAL_WEB_BRANDING
            "img-src 'self'; style-src 'self'; "
#endif
#ifdef UMICOM_LOCAL_WEB_CVM_BROWSER
            /* The opted-in browser host can load its same-origin worker. */
            "worker-src 'self'; "
#endif
            "form-action 'self'; base-uri 'none'; frame-ancestors 'none'");
    if (status == UMI_STATUS_OK)
        status = umi_web_response_set_header(response, "X-Content-Type-Options", "nosniff");
    return status;
}

#ifdef UMICOM_LOCAL_WEB_BRANDING
/* The assembled demo owns its immutable resources. Larger SVG/ICO
 * payloads use Framework's bounded response-body contract. Other paths still
 * follow the normal router, and no URL is interpreted as a file path here. */
static UmiStatus BrandAsset(const UmiWebRequest *request, UmiWebResponse *response,
                            UmiWebResponseBody **body, void *context)
{
    /* Apply the supplied typography roles to the existing page geometry. */
    static const unsigned char style[] =
        "body{font-family:'Segoe UI Variable Text','Segoe UI',system-ui,sans-serif}"
        "code,pre,kbd,samp{font-family:'Cascadia Code',Consolas,monospace}\n";
    static const struct {
        const char *path;
        const char *media_type;
        const unsigned char *bytes;
        size_t length;
        bool worker_script;
    } assets[] = {
        {"/branding/umicom-logo.svg", "image/svg+xml", UmiWebBrandWordmark, sizeof UmiWebBrandWordmark, false},
        {"/branding/umicom-icon.svg", "image/svg+xml", UmiWebBrandIconSvg, sizeof UmiWebBrandIconSvg, false},
        {"/favicon.ico", "image/vnd.microsoft.icon", UmiWebBrandIconIco, sizeof UmiWebBrandIconIco, false},
        {"/branding/local-service.css", "text/css; charset=utf-8", style, sizeof style - 1U, false},
#ifdef UMICOM_LOCAL_WEB_CVM_BROWSER
        /* Preserve the SDK modules' relative import in one bounded prefix. */
        {"/cvm/browser/worker.mjs", "text/javascript; charset=utf-8",
         UmiWebCvmBrowserWorker, sizeof UmiWebCvmBrowserWorker, true},
        {"/cvm/browser/arithmetic.mjs", "text/javascript; charset=utf-8",
         UmiWebCvmBrowserArithmetic, sizeof UmiWebCvmBrowserArithmetic, true},
#endif
    };

    if (body == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *body = NULL;
    if (request == NULL || response == NULL || context == NULL ||
        memchr(request->path, '\0', sizeof request->path) == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < sizeof assets / sizeof assets[0]; ++index) {
        if (strcmp(request->path, assets[index].path) != 0) continue;
        /* HEAD shares the resource length and metadata. The server owns the
         * decision to omit its payload from the outgoing response. */
        if (request->method != UMI_HTTP_METHOD_GET && request->method != UMI_HTTP_METHOD_HEAD) {
            UmiStatus status = umi_web_response_set_text(
                response, 405, "text/plain; charset=utf-8", "Method Not Allowed\n");
            return status == UMI_STATUS_OK
                ? umi_web_response_set_header(response, "Allow", "GET, HEAD") : status;
        }
        UmiStatus status = umi_web_response_set_text(response, 200, assets[index].media_type, "");
        if (status == UMI_STATUS_OK)
            status = umi_web_response_set_header(response, "X-Content-Type-Options", "nosniff");
        if (status == UMI_STATUS_OK && assets[index].worker_script) {
            /* The interpreter imports only its local sibling and owns no
             * network connections. Rebuilding the SDK must replace old bytes. */
            status = umi_web_response_set_header(response, "Content-Security-Policy",
                "default-src 'none'; script-src 'self'; connect-src 'none'; worker-src 'none'");
            if (status == UMI_STATUS_OK)
                status = umi_web_response_set_header(response, "Cache-Control", "no-store");
        }
        if (status == UMI_STATUS_OK)
            status = UmiWebResponseBodyCreate(assets[index].bytes, assets[index].length, body);
        return status;
    }
    return umi_web_router_dispatch(context, request, response);
}
#endif

static UmiStatus Health(const UmiWebRequest *request, UmiWebResponse *response, void *context)
{
    (void)request;
    (void)context;
    return umi_web_response_set_text(response, 200, "application/json",
                                     "{\"service\":\"umicom-local-example\",\"status\":\"ready\"}\n");
}
static UmiStatus Echo(const UmiWebRequest *request, UmiWebResponse *response, void *context)
{
    (void)context;
    UmiStatus status = umi_web_response_set_text(response, 200, "text/plain; charset=utf-8", "");
    if (status != UMI_STATUS_OK)
        return status;
    memcpy(response->body, request->body, request->body_length);
    response->body_length = request->body_length;
    return umi_web_response_set_header(response, "X-Content-Type-Options", "nosniff");
}
/* The shared preview admission now owns decimal port policy. Retain this
 * former local parser for review; the accepted range remains unchanged. */
#if 0
static int ParsePort(const char *text, uint16_t *out)
{
    if (text == NULL || *text == '\0')
        return 0;
    unsigned port = 0U;
    for (size_t i = 0U; text[i] != '\0'; ++i)
    {
        if (text[i] < '0' || text[i] > '9')
            return 0;
        unsigned digit = (unsigned)(text[i] - '0');
        if (port > (65535U - digit) / 10U)
            return 0;
        port = port * 10U + digit;
    }
    if (port < 1024U)
        return 0;
    *out = (uint16_t)port;
    return 1;
}
#endif
static int ParsePort(const char *text, uint16_t *out)
{
    uint32_t port = 0U;
    if (out == NULL || UmiWebPreviewPortParse(text, &port) != UMI_STATUS_OK) return 0;
    *out = (uint16_t)port;
    return 1;
}
/* Capture Windows Unicode arguments before interpreting a selected root.
 * Keep the previous entry signature for review; its body follows unchanged. */
#if 0
int main(int argc, char **argv)
#endif
static int LocalServiceMain(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "--help") == 0)
    {
        puts("Umicom local C web service\nUse --port <1024..65535> to start on 127.0.0.1.\nRoutes: /, "
             "/api/health, POST /api/echo. Stop with Ctrl+C.\n"
             "Add --root <absolute-export-directory> to preview files instead of the demo routes.\n"
             /* The former inline limit is retained for review: "smaller than 16 KiB". */
             "Static files are read-only and must each fit within 1 MiB.");
#ifdef UMICOM_LOCAL_WEB_CVM_PREVIEW
        puts("Demo preview: POST " UMI_WEB_CVM_PREVIEW_PATH " with Content-Type "
             UMI_WEB_CVM_ARTIFACT_MEDIA_TYPE ".");
        puts("For a required-input artifact use POST " UMI_WEB_CVM_INT_INPUT_PATH "?input=20.");
#endif
#ifdef UMICOM_LOCAL_WEB_CVM_BROWSER
        /* This reports an available worker module, independent of the C API. */
        puts("Browser worker module: /cvm/browser/worker.mjs");
#endif
#ifdef UMICOM_HAS_FASTCGI_LOOPBACK
        puts("Add --fastcgi <worker-port> <absolute-entry-file> to serve only /app from a running local worker.");
#endif
        return 0;
    }
    UmiWebServerConfig config = umi_web_server_config_default();
    bool fastcgi = false;
#ifdef UMICOM_HAS_FASTCGI_LOOPBACK
    fastcgi = argc == 6 && strcmp(argv[3], "--fastcgi") == 0;
#endif
    if ((!fastcgi && argc != 3 && argc != 5) || strcmp(argv[1], "--port") != 0 || !ParsePort(argv[2], &config.port) ||
        (argc == 5 && strcmp(argv[3], "--root") != 0))
    {
        fprintf(stderr, "Choose a port explicitly: --port <1024..65535>. Use --help for details.\n");
        return 2;
    }
#ifdef UMICOM_HAS_FASTCGI_LOOPBACK
    /* The operator selects one existing entry and a separate worker port.
     * The route and endpoint live for the whole local listener lifetime. */
    UmiWebFastCgiLoopbackEndpoint worker = {.timeoutMs = 2000U};
    UmiWebFastCgiRoute application = {0};
    if (fastcgi) {
        if (!ParsePort(argv[4], &worker.port) || worker.port == config.port ||
            !umi_fs_is_absolute(argv[5]) || !umi_fs_is_file(argv[5])) {
            fputs("Choose a distinct local worker port and an existing absolute FastCGI entry file.\n", stderr);
            return 2;
        }
        application.application = (UmiWebFastCgiApplication){argv[5], "/app", "localhost", config.port,
                                                            1U, UMI_WEB_CONNECTION_RESPONSE_LIMIT};
        UmiStatus prepared = UmiWebFastCgiLoopbackConnectorInit(&worker, &application.connector);
        if (prepared != UMI_STATUS_OK) {
            fprintf(stderr, "Cannot configure the local FastCGI connector: %s\n", umi_status_text(prepared));
            return 1;
        }
    }
#endif
    /* Keep the selected root alive for the complete server lifetime. Preview
     * mode has no demo API routes, so exported paths keep their own meaning. */
    UmiWebStaticFiles files;
    if (argc == 5 && umi_web_static_files_init(&files, argv[4]) != UMI_STATUS_OK)
    {
        fputs("Choose an absolute export folder without dot segments.\n", stderr);
        return 2;
    }
    UmiWebService *service = NULL;
    UmiWebServer *server = NULL;
    UmiStatus status = umi_web_service_create(&service);
    /* Loopback binding and browser admission solve different problems. Reject
     * an unrelated site's Host/Origin before exposing any route or file. */
    UmiWebLoopbackAccess access = {config.port};
    if (status == UMI_STATUS_OK)
        status = UmiWebServiceSetRequestGate(service, UmiWebLoopbackRequestGate, &access);
    const struct
    {
        UmiHttpMethod method;
        const char *path;
        UmiWebHandler handler;
    } routes[] = {{UMI_HTTP_METHOD_GET, "/", Home},
                  {UMI_HTTP_METHOD_HEAD, "/", Home},
                  {UMI_HTTP_METHOD_GET, "/api/health", Health},
                  {UMI_HTTP_METHOD_HEAD, "/api/health", Health},
                  {UMI_HTTP_METHOD_POST, "/api/echo", Echo}};
    for (size_t i = 0U; argc == 3 && status == UMI_STATUS_OK && i < sizeof(routes) / sizeof(routes[0]); ++i)
    {
        UmiWebRoute route;
        status = umi_web_route_init(&route, routes[i].method, routes[i].path, routes[i].handler, NULL);
        if (status == UMI_STATUS_OK)
            status = umi_web_router_add(umi_web_service_router(service), &route);
    }
#ifdef UMICOM_LOCAL_WEB_BRANDING
    /* Customer previews and external workers retain their own page assets.
     * Install this fallback only for the local demonstration website. */
    if (argc == 3 && status == UMI_STATUS_OK)
        status = UmiWebServiceSetBodyFallback(service, BrandAsset, umi_web_service_router(service));
#endif
#ifdef UMICOM_LOCAL_WEB_CVM_PREVIEW
    /* Enable the bounded VM only in the opted-in demo. The same loopback
     * request gate runs before every route, including artifact preview. */
    UmiWebCvmPreviewConfig cvm_preview = {UMI_WEB_CVM_PREVIEW_MAX_FUEL};
    if (argc == 3 && status == UMI_STATUS_OK)
    {
        UmiWebRoute route;
        status = umi_web_route_init(&route, UMI_HTTP_METHOD_POST, UMI_WEB_CVM_PREVIEW_PATH,
            UmiWebCvmPreviewHandle, &cvm_preview);
        if (status == UMI_STATUS_OK)
            status = umi_web_router_add(umi_web_service_router(service), &route);
        if (status == UMI_STATUS_OK)
            status = umi_web_route_init(&route, UMI_HTTP_METHOD_POST, UMI_WEB_CVM_INT_INPUT_PATH,
                UmiWebCvmPreviewIntInputHandle, &cvm_preview);
        if (status == UMI_STATUS_OK)
            status = umi_web_router_add(umi_web_service_router(service), &route);
    }
#endif
    if (status == UMI_STATUS_OK && argc == 5)
        status = UmiWebRouterSetFallback(umi_web_service_router(service), UmiWebStaticFilesHandle, &files);
    if (status == UMI_STATUS_OK && argc == 5)
        status = UmiWebServiceSetBodyFallback(service, UmiWebStaticFilesHandleBody, &files);
#ifdef UMICOM_HAS_FASTCGI_LOOPBACK
    if (status == UMI_STATUS_OK && fastcgi)
        status = UmiWebServiceSetBodyFallback(service, UmiWebFastCgiRouteHandle, &application);
#endif
    if (status == UMI_STATUS_OK)
        status = umi_web_server_create(&config, service, &server);
    if (status == UMI_STATUS_OK)
        status = umi_web_server_start(server);
    if (status != UMI_STATUS_OK)
    {
        fprintf(stderr, "Cannot start local service: %s\n", umi_status_text(status));
        umi_web_server_destroy(server);
        umi_web_service_destroy(service);
        return 1;
    }
    /* The signal handler only sets a flag. All library work stays on the owner
     * thread, which wakes at bounded intervals even when no client connects. */
    (void)signal(SIGINT, Stop);
    (void)signal(SIGTERM, Stop);
    if (fastcgi)
        printf("Open http://127.0.0.1:%u/app in a browser. Stop with Ctrl+C.\n", (unsigned)config.port);
    else
        printf("Open http://127.0.0.1:%u/ in a browser. Stop with Ctrl+C.\n", (unsigned)config.port);
    fflush(stdout);
    while (!stop_requested)
    {
        UmiWebExchangeResult result;
        status = UmiWebServerServeNext(server, 100U, 3000U, NULL, &result);
        if (status == UMI_STATUS_NOT_IMPLEMENTED && !result.response_complete)
            break;
        /* A listener failure is different from a client that disconnected.
         * Do not spin forever if the native listener can no longer be used. */
        if (status == UMI_STATUS_IO_ERROR && result.request_status == UMI_STATUS_OK &&
            result.bytes_received == 0U && !result.dispatched)
            break;
        if (status != UMI_STATUS_OK && status != UMI_STATUS_TIMEOUT && status != UMI_STATUS_UNAVAILABLE)
            fprintf(stderr, "Request ended: %s; response %s.\n", umi_status_text(status),
                    result.response_complete ? "sent" : "incomplete");
    }
    umi_web_server_destroy(server);
    umi_web_service_destroy(service);
    return !stop_requested && status != UMI_STATUS_OK ? 1 : 0;
}

/* The platform owner provides UTF-8 arguments on every supported host. */
int main(int argc, char **argv)
{
    int result = 1;
    UmiStatus status = UmiNativeArgumentsDispatch(argc, argv, LocalServiceMain, &result);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Cannot capture native arguments: %s\n", umi_status_text(status));
        return 1;
    }
    return result;
}
