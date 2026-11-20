/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/fastcgi_application.c
 *
 * PURPOSE:
 *   Map a parsed request to one configured FastCGI application without deriving an executable path.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/fastcgi_application.h"
#include "umicom/platform/path.h"
#include "connection_internal.h"
#include <stdio.h>
#include <string.h>

static size_t BoundedLength(const char *text, size_t capacity)
{
    if (text == NULL) return capacity;
    size_t size = 0U;
    while (size < capacity && text[size] != '\0') ++size;
    return size;
}

static bool TargetText(const char *text, size_t length, bool path)
{
    for (size_t i = 0U; i < length; ++i) {
        const unsigned char c = (unsigned char)text[i];
        if (c <= 0x20U || c >= 0x7fU || c == '#' || c == '\\' || (path && c == '?'))
            return false;
    }
    return true;
}

static bool PrintableText(const char *text, size_t length)
{
    for (size_t i = 0U; i < length; ++i) {
        const unsigned char c = (unsigned char)text[i];
        if (c < 0x20U || c == 0x7fU) return false;
    }
    return true;
}

static bool ValidServerName(const char *text, size_t length)
{
    for (size_t i = 0U; i < length; ++i) {
        const char c = text[i];
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) continue;
        if ((c == '.' || c == '-') && i != 0U && i + 1U < length) continue;
        return false;
    }
    return length != 0U;
}

static UmiStatus ConfiguredRoute(const UmiWebFastCgiApplication *application)
{
    const size_t fileBytes = BoundedLength(application->scriptFilename, UMI_PATH_CAPACITY);
    const size_t nameBytes = BoundedLength(application->scriptName, UMI_WEB_PATH_CAPACITY);
    const size_t serverBytes = BoundedLength(application->serverName, 254U);
    if (fileBytes == 0U || fileBytes == UMI_PATH_CAPACITY ||
        !PrintableText(application->scriptFilename, fileBytes) ||
        !umi_path_is_absolute(application->scriptFilename) ||
        nameBytes == 0U || nameBytes == UMI_WEB_PATH_CAPACITY || application->scriptName[0] != '/' ||
        !TargetText(application->scriptName, nameBytes, true) ||
        serverBytes == 254U || !ValidServerName(application->serverName, serverBytes) ||
        application->serverPort == 0U || application->requestId == 0U || application->replyLimit == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    return application->replyLimit > UMI_WEB_FASTCGI_EXCHANGE_LIMIT ?
        UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}

static UmiStatus ReadContentType(const UmiWebRequest *request, const char **contentType)
{
    for (size_t i = 0U; i < request->header_count; ++i) {
        const UmiWebHeader *header = &request->headers[i];
        const size_t name = BoundedLength(header->name, sizeof header->name);
        if (name == 0U || name == sizeof header->name) return UMI_STATUS_INVALID_ARGUMENT;
        if (!UmiWebFieldEqual((const unsigned char *)header->name, name, "content-type")) continue;
        const size_t value = BoundedLength(header->value, sizeof header->value);
        if (*contentType != NULL || value == 0U || value == sizeof header->value ||
            !PrintableText(header->value, value))
            return UMI_STATUS_PARSE_ERROR;
        *contentType = header->value;
    }
    return UMI_STATUS_OK;
}

UmiStatus UmiWebFastCgiApplicationHandle(const UmiWebFastCgiApplication *application,
    const UmiWebRequest *request, const UmiWebConnectionIo *worker, const UmiCancellationToken *cancel,
    UmiWebResponse *response, UmiWebResponseBody **outBody)
{
    if (application == NULL || request == NULL || worker == NULL || response == NULL ||
        outBody == NULL || *outBody != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = ConfiguredRoute(application);
    if (status != UMI_STATUS_OK) return status;
    const size_t path = BoundedLength(request->path, sizeof request->path);
    const size_t query = BoundedLength(request->query, sizeof request->query);
    if (path == 0U || path == sizeof request->path || query == sizeof request->query ||
        !TargetText(request->path, path, true) || !TargetText(request->query, query, false) ||
        memcmp(request->version, "HTTP/1.1", sizeof "HTTP/1.1") != 0 ||
        request->header_count > UMI_WEB_MAX_HEADERS || request->body_length > sizeof request->body)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(request->path, application->scriptName) != 0) return UMI_STATUS_NOT_FOUND;
    if (request->method <= UMI_HTTP_METHOD_UNKNOWN || request->method >= UMI_HTTP_METHOD_HEAD)
        return UMI_STATUS_NOT_IMPLEMENTED;
    const char *contentType = NULL;
    status = ReadContentType(request, &contentType);
    if (status != UMI_STATUS_OK) return status;

    char uri[UMI_WEB_PATH_CAPACITY + UMI_WEB_QUERY_CAPACITY];
    memcpy(uri, request->path, path);
    if (query != 0U) {
        uri[path] = '?';
        memcpy(uri + path + 1U, request->query, query + 1U);
    } else {
        uri[path] = '\0';
    }
    char port[6], length[6];
    const int portChars = snprintf(port, sizeof port, "%u", (unsigned)application->serverPort);
    const int lengthChars = snprintf(length, sizeof length, "%zu", request->body_length);
    if (portChars < 1 || (size_t)portChars >= sizeof port ||
        lengthChars < 1 || (size_t)lengthChars >= sizeof length)
        return UMI_STATUS_INTERNAL_ERROR;

    /* Only these explicit CGI parameters cross the application boundary.
     * Body framing and server name come from the admitted request and
     * configured host, so unrelated HTTP headers never become process inputs. */
    const char *fields[][2] = {
        {"GATEWAY_INTERFACE", "CGI/1.1"}, {"SERVER_PROTOCOL", "HTTP/1.1"},
        {"SERVER_SOFTWARE", "Umicom"}, {"SERVER_NAME", application->serverName},
        {"SERVER_PORT", port}, {"REQUEST_METHOD", umi_http_method_text(request->method)},
        {"SCRIPT_FILENAME", application->scriptFilename}, {"SCRIPT_NAME", application->scriptName},
        {"QUERY_STRING", request->query}, {"REQUEST_URI", uri}, {"CONTENT_LENGTH", length},
        {"CONTENT_TYPE", contentType}
    };
    const size_t count = sizeof fields / sizeof fields[0] - (contentType == NULL ? 1U : 0U);
    UmiWebFastCgiVariable parameters[sizeof fields / sizeof fields[0]];
    for (size_t i = 0U; i < count; ++i)
        parameters[i] = (UmiWebFastCgiVariable){fields[i][0], strlen(fields[i][0]), fields[i][1],
            strlen(fields[i][1])};

    const UmiWebFastCgiRequest exchange = {.requestId = application->requestId,
        .variables = parameters, .variableCount = count, .body = request->body,
        .bodyBytes = request->body_length, .replyLimit = application->replyLimit};
    UmiWebFastCgiResult result = {0};
    status = UmiWebFastCgiExchange(worker, &exchange, cancel, &result);
    if (status == UMI_STATUS_OK) status = UmiWebFastCgiResponseDecode(&result, response, outBody);
    UmiWebFastCgiResultDestroy(&result);
    return status;
}

typedef struct RouteConnection {
    const UmiWebFastCgiRoute *route;
    UmiWebConnectionIo worker;
    bool attempted;
    bool owned;
} RouteConnection;

static UmiStatus RouteWrite(void *context, const void *bytes, size_t length, size_t *written)
{
    RouteConnection *connection = context;
    const UmiWebFastCgiRoute *route = connection->route;
    if (!connection->owned) {
        /* The application and exchange have admitted the entire request by
         * their first write. Only then may a host acquire worker resources. */
        connection->attempted = true;
        UmiStatus status = route->connector.open(route->connector.context, route->cancel, &connection->worker);
        if (status != UMI_STATUS_OK) return status;
        connection->owned = true;
        if (connection->worker.read == NULL || connection->worker.write == NULL)
            return UMI_STATUS_INVALID_STATE;
        if (umi_cancellation_token_is_requested(route->cancel)) return UMI_STATUS_CANCELLED;
    }
    return connection->worker.write(connection->worker.context, bytes, length, written);
}

static UmiStatus RouteRead(void *context, void *bytes, size_t capacity, size_t *received)
{
    RouteConnection *connection = context;
    if (!connection->owned) return UMI_STATUS_INVALID_STATE;
    return connection->worker.read(connection->worker.context, bytes, capacity, received);
}

UmiStatus UmiWebFastCgiRouteHandle(const UmiWebRequest *request, UmiWebResponse *response,
    UmiWebResponseBody **outBody, void *context)
{
    const UmiWebFastCgiRoute *route = context;
    if (route == NULL || route->connector.open == NULL || route->connector.close == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    RouteConnection connection = {.route = route};
    const UmiWebConnectionIo io = {.context = &connection, .read = RouteRead, .write = RouteWrite};
    UmiStatus status = UmiWebFastCgiApplicationHandle(&route->application, request, &io,
        route->cancel, response, outBody);
    if (connection.owned) route->connector.close(route->connector.context, &connection.worker);
    /* Map admission results to HTTP only before a worker was contacted.
     * The same statuses returned by a connector still mean backend failure. */
    if (!connection.attempted && status == UMI_STATUS_NOT_FOUND)
        return umi_web_response_set_text(response, UMI_HTTP_STATUS_NOT_FOUND, "text/plain; charset=utf-8",
            "Not Found");
    if (!connection.attempted && status == UMI_STATUS_NOT_IMPLEMENTED)
        return umi_web_response_set_text(response, 501, "text/plain; charset=utf-8",
            "Method not supported by this application");
    return status;
}
