/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/preview.c
 *
 * PURPOSE:
 *   Compose rooted files, loopback request admission and native listener ownership without duplicating their policies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/preview.h"
#include "umicom/web/static_files.h"
#include "umicom/web/loopback_access.h"
#include <stdlib.h>
#include <string.h>
struct UmiWebPreview {
    UmiWebStaticFiles files;
    UmiWebLoopbackAccess access;
    UmiWebService *service;
    UmiWebServer *server;
};
UmiStatus UmiWebPreviewValidate(const char *absolute_root, uint32_t port)
{
    if (port < 1024U || port > 65535U) return UMI_STATUS_INVALID_ARGUMENT;
    UmiWebStaticFiles files;
    return umi_web_static_files_init(&files, absolute_root);
}
UmiStatus UmiWebPreviewPortParse(const char *text, uint32_t *out_port)
{
    if (text == NULL || out_port == NULL || text[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    uint32_t value = 0U;
    for (size_t i = 0U; text[i] != '\0'; ++i) {
        if (text[i] < '0' || text[i] > '9') return UMI_STATUS_INVALID_ARGUMENT;
        uint32_t digit = (uint32_t)(text[i] - '0');
        if (value > (65535U - digit) / 10U) return UMI_STATUS_INVALID_ARGUMENT;
        value = value * 10U + digit;
    }
    if (value < 1024U) return UMI_STATUS_INVALID_ARGUMENT;
    *out_port = value;
    return UMI_STATUS_OK;
}
UmiStatus UmiWebPreviewCreate(const char *absolute_root, uint32_t port,
    UmiWebPreview **out_preview)
{
    if (out_preview == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_preview = NULL;
    UmiStatus status = UmiWebPreviewValidate(absolute_root, port);
    if (status != UMI_STATUS_OK) return status;
    UmiWebPreview *preview = calloc(1U, sizeof(*preview));
    if (preview == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_web_static_files_init(&preview->files, absolute_root);
    preview->access.port = (uint16_t)port;
    if (status == UMI_STATUS_OK) status = umi_web_service_create(&preview->service);
    if (status == UMI_STATUS_OK)
        status = UmiWebServiceSetRequestGate(preview->service,
            UmiWebLoopbackRequestGate, &preview->access);
    if (status == UMI_STATUS_OK)
        status = UmiWebRouterSetFallback(umi_web_service_router(preview->service),
            UmiWebStaticFilesHandle, &preview->files);
    if (status == UMI_STATUS_OK)
        status = UmiWebServiceSetBodyFallback(preview->service,
            UmiWebStaticFilesHandleBody, &preview->files);
    UmiWebServerConfig config = umi_web_server_config_default();
    /* Keep transport binding independent of browser header admission. */
    memcpy(config.bind_address, "127.0.0.1", sizeof("127.0.0.1"));
    config.loopback_only = 1;
    config.port = (uint16_t)port;
    if (status == UMI_STATUS_OK)
        status = umi_web_server_create(&config, preview->service, &preview->server);
    if (status != UMI_STATUS_OK) { UmiWebPreviewDestroy(preview); return status; }
    *out_preview = preview;
    return UMI_STATUS_OK;
}
void UmiWebPreviewDestroy(UmiWebPreview *preview)
{
    if (preview == NULL) return;
    /* Destroy borrowers before their service and embedded context storage. */
    umi_web_server_destroy(preview->server);
    umi_web_service_destroy(preview->service);
    free(preview);
}
UmiStatus UmiWebPreviewStart(UmiWebPreview *preview)
{
    return preview != NULL ? umi_web_server_start(preview->server) : UMI_STATUS_INVALID_ARGUMENT;
}
UmiStatus UmiWebPreviewStop(UmiWebPreview *preview)
{
    return preview != NULL ? umi_web_server_stop(preview->server) : UMI_STATUS_INVALID_ARGUMENT;
}
UmiStatus UmiWebPreviewHandle(UmiWebPreview *preview,
    const UmiWebRequest *request, UmiWebResponse *response)
{
    if (preview == NULL || request == NULL || response == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return umi_web_service_handle(preview->service, request, response);
}
UmiStatus UmiWebPreviewServeNext(UmiWebPreview *preview, uint32_t wait_ms,
    uint32_t exchange_ms, const UmiCancellationToken *cancel, UmiWebExchangeResult *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    if (preview == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return UmiWebServerServeNext(preview->server, wait_ms, exchange_ms, cancel, out);
}

UmiStatus UmiWebPreviewHandleBody(UmiWebPreview *preview, const UmiWebRequest *request,
    UmiWebResponse *response, UmiWebResponseBody **out_body)
{
    if (out_body == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_body = NULL;
    if (preview == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return UmiWebServiceHandleBody(preview->service, request, response, out_body);
}
