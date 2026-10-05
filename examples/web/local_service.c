/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/web/local_service.c
 * PURPOSE: Serve a small C-authored website and API through Framework routing on an explicitly chosen loopback port.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/server.h"
#include "umicom/web/static_files.h"
#include "umicom/web/loopback_access.h"
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
        "<title>Umicom local web service</title><body><main>"
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
            "default-src 'none'; form-action 'self'; base-uri 'none'; frame-ancestors 'none'");
    if (status == UMI_STATUS_OK)
        status = umi_web_response_set_header(response, "X-Content-Type-Options", "nosniff");
    return status;
}
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
int main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "--help") == 0)
    {
        puts("Umicom local C web service\nUse --port <1024..65535> to start on 127.0.0.1.\nRoutes: /, "
             "/api/health, POST /api/echo. Stop with Ctrl+C.\n"
             "Add --root <absolute-export-directory> to preview files instead of the demo routes.\n"
             "Static files are read-only and must each be smaller than 16 KiB.");
        return 0;
    }
    UmiWebServerConfig config = umi_web_server_config_default();
    if ((argc != 3 && argc != 5) || strcmp(argv[1], "--port") != 0 || !ParsePort(argv[2], &config.port) ||
        (argc == 5 && strcmp(argv[3], "--root") != 0))
    {
        fprintf(stderr, "Choose a port explicitly: --port <1024..65535>. Use --help for details.\n");
        return 2;
    }
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
    if (status == UMI_STATUS_OK && argc == 5)
        status = UmiWebRouterSetFallback(umi_web_service_router(service), UmiWebStaticFilesHandle, &files);
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
