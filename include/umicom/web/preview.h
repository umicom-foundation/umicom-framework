/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/preview.h
 *
 * PURPOSE:
 *   Own a local static-preview service with one copied root and the canonical browser request gate.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WEB_PREVIEW_H
#define UMICOM_WEB_PREVIEW_H
#include "umicom/web/server.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWebPreview UmiWebPreview;
/* Admission reads only bounded syntax. It neither accesses the root nor opens
 * a listener. Ports are explicitly selected in the range 1024..65535. */
UmiStatus UmiWebPreviewValidate(const char *absolute_root, uint32_t port);
/* Decimal digits only; failure leaves out_port unchanged. */
UmiStatus UmiWebPreviewPortParse(const char *text, uint32_t *out_port);
/* Create copies the root and owns every borrowed routing context. No bind or
 * file read occurs here. On failure, a valid out_preview is set to NULL. */
UmiStatus UmiWebPreviewCreate(const char *absolute_root, uint32_t port,
    UmiWebPreview **out_preview);
void UmiWebPreviewDestroy(UmiWebPreview *preview);
UmiStatus UmiWebPreviewStart(UmiWebPreview *preview);
UmiStatus UmiWebPreviewStop(UmiWebPreview *preview);
/* Headless request dispatch uses the same Host/Origin gate and static fallback.
 * The caller supplies a parsed request. File I/O can block; do not call on a
 * GUI event thread. UMI_STATUS_OK may represent a deliberate HTTP refusal. */
UmiStatus UmiWebPreviewHandle(UmiWebPreview *preview,
    const UmiWebRequest *request, UmiWebResponse *response);
/* A started session serves one bounded exchange; see ServerServeNext for
 * deadlines, cooperative cancellation and socket ownership. Errors never
 * retry a request. The caller decides whether to keep serving after an error.
 * All operations require one owner thread. Only cancellation may cross threads.
 * This is read-only local development hosting: no TLS, authentication,
 * uploads, directory listings, caching, remote deployment or sandbox. */
UmiStatus UmiWebPreviewServeNext(UmiWebPreview *preview, uint32_t wait_ms,
    uint32_t exchange_ms, const UmiCancellationToken *cancel,
    UmiWebExchangeResult *out);
#ifdef __cplusplus
}
#endif
#endif
