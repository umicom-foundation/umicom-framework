/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/local_models/download.c
 *
 * PURPOSE:
 *   Download pinned public model bytes with explicit consent, bounded storage and post-download verification.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "umicom/platform/output_file.h"
#include <stdio.h>
#include <string.h>
#ifdef UMICOM_MODELS_HAS_CURL
#include <curl/curl.h>
#endif

UmiStatus UmiModelDownloadPlanCreate(const UmiModelSource *source,
    const char *destination, UmiModelDownloadPlan *outPlan)
{
    if (outPlan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (UmiModelSourceValidate(source) != UMI_STATUS_OK ||
        !UmiModelTextFits(destination, UMI_MODEL_PATH_CAPACITY) ||
        UmiOutputFileValidatePath(destination) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = strlen(destination);
    if (length <= 5U || strcmp(destination + length - 5U, ".gguf") != 0) return UMI_STATUS_INVALID_ARGUMENT;
    UmiModelDownloadPlan plan = {0};
    plan.source = *source;
    memcpy(plan.destination, destination, length + 1U);
    int count = snprintf(plan.url, sizeof(plan.url), "https://huggingface.co/%s/resolve/%s/%s",
        source->repository, source->revision, source->filename);
    if (count < 0 || (size_t)count >= sizeof(plan.url)) return UMI_STATUS_CAPACITY_EXCEEDED;
    *outPlan = plan;
    return UMI_STATUS_OK;
}
#ifdef UMICOM_MODELS_HAS_CURL
typedef struct ModelDownloadSink {
    UmiOutputFile *file;
    const UmiCancellationToken *cancellation;
    uint64_t expected, received;
    UmiStatus status;
} ModelDownloadSink;
static size_t ReceiveModel(char *bytes, size_t size, size_t count, void *context)
{
    ModelDownloadSink *sink = context;
    if (size != 0U && count > SIZE_MAX / size) {
        sink->status = UMI_STATUS_CAPACITY_EXCEEDED;
        return 0U;
    }
    size_t amount = size * count;
    if (sink->cancellation != NULL && umi_cancellation_token_is_requested(sink->cancellation)) {
        sink->status = UMI_STATUS_CANCELLED;
        return 0U;
    }
    if ((uint64_t)amount > sink->expected - sink->received) {
        sink->status = UMI_STATUS_CAPACITY_EXCEEDED;
        return 0U;
    }
    sink->status = UmiOutputFileWrite(sink->file, bytes, amount);
    if (sink->status != UMI_STATUS_OK) return 0U;
    sink->received += (uint64_t)amount;
    return amount;
}
static int DownloadProgress(void *context, curl_off_t total, curl_off_t now,
    curl_off_t uploadTotal, curl_off_t uploaded)
{
    ModelDownloadSink *sink = context;
    (void)total; (void)now; (void)uploadTotal; (void)uploaded;
    return sink->cancellation != NULL && umi_cancellation_token_is_requested(sink->cancellation) ? 1 : 0;
}
bool UmiModelDownloadAvailable(void) { return true; }
#else
bool UmiModelDownloadAvailable(void) { return false; }
#endif
UmiStatus UmiModelDownload(const UmiModelDownloadPlan *plan, bool approved,
    const UmiCancellationToken *cancellation, UmiModelInspection *outInspection)
{
    if (outInspection != NULL) memset(outInspection, 0, sizeof(*outInspection));
    if (plan == NULL || outInspection == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Rebuild the URL from reviewed fields: modifying a public plan's url must
     * never redirect execution to an arbitrary origin or attach credentials. */
    UmiModelDownloadPlan checked;
    UmiStatus status = UmiModelDownloadPlanCreate(&plan->source, plan->destination, &checked);
    if (status != UMI_STATUS_OK || !UmiModelTextFits(plan->url, sizeof(plan->url)) ||
        strcmp(plan->url, checked.url) != 0) return UMI_STATUS_INVALID_ARGUMENT;
    if (!approved) return UMI_STATUS_PERMISSION_DENIED;
    if (cancellation != NULL && umi_cancellation_token_is_requested(cancellation)) return UMI_STATUS_CANCELLED;
#ifndef UMICOM_MODELS_HAS_CURL
    return UMI_STATUS_UNAVAILABLE;
#else
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) return UMI_STATUS_UNAVAILABLE;
    CURL *curl = curl_easy_init();
    ModelDownloadSink sink = {0};
    sink.expected = checked.source.bytes;
    sink.cancellation = cancellation;
    if (curl == NULL) { curl_global_cleanup(); return UMI_STATUS_OUT_OF_MEMORY; }
    status = UmiOutputFileCreate(checked.destination, &sink.file);
    if (status != UMI_STATUS_OK) goto done;
    /* Hugging Face serves pinned large files through HTTPS CDN redirects.
     * No credentials are sent, redirects cannot change protocol, proxy/netrc
     * discovery is disabled, and every option failure aborts the operation. */
#define MODEL_CURL(option, value) do { if (curl_easy_setopt(curl, option, value) != CURLE_OK) { status = UMI_STATUS_UNAVAILABLE; goto done; } } while (0)
    MODEL_CURL(CURLOPT_URL, checked.url);
    MODEL_CURL(CURLOPT_PROTOCOLS_STR, "https");
    MODEL_CURL(CURLOPT_REDIR_PROTOCOLS_STR, "https");
    MODEL_CURL(CURLOPT_FOLLOWLOCATION, 1L);
    MODEL_CURL(CURLOPT_MAXREDIRS, 5L);
    MODEL_CURL(CURLOPT_SSL_VERIFYPEER, 1L);
    MODEL_CURL(CURLOPT_SSL_VERIFYHOST, 2L);
    MODEL_CURL(CURLOPT_PROXY, "");
    MODEL_CURL(CURLOPT_NOPROXY, "*");
    MODEL_CURL(CURLOPT_NETRC, (long)CURL_NETRC_IGNORED);
    MODEL_CURL(CURLOPT_UNRESTRICTED_AUTH, 0L);
    MODEL_CURL(CURLOPT_FAILONERROR, 1L);
    MODEL_CURL(CURLOPT_NOSIGNAL, 1L);
    MODEL_CURL(CURLOPT_CONNECTTIMEOUT, 30L);
    MODEL_CURL(CURLOPT_TIMEOUT, 86400L);
    MODEL_CURL(CURLOPT_LOW_SPEED_LIMIT, 1024L);
    MODEL_CURL(CURLOPT_LOW_SPEED_TIME, 120L);
    MODEL_CURL(CURLOPT_MAXFILESIZE_LARGE, (curl_off_t)sink.expected);
    MODEL_CURL(CURLOPT_WRITEFUNCTION, ReceiveModel);
    MODEL_CURL(CURLOPT_WRITEDATA, &sink);
    MODEL_CURL(CURLOPT_NOPROGRESS, 0L);
    MODEL_CURL(CURLOPT_XFERINFOFUNCTION, DownloadProgress);
    MODEL_CURL(CURLOPT_XFERINFODATA, &sink);
    MODEL_CURL(CURLOPT_USERAGENT, "Umicom-Local-Models/1");
    CURLcode code = curl_easy_perform(curl);
    long response = 0L;
    if (sink.status != UMI_STATUS_OK) status = sink.status;
    else if (code == CURLE_ABORTED_BY_CALLBACK) status = UMI_STATUS_CANCELLED;
    else if (code == CURLE_OPERATION_TIMEDOUT) status = UMI_STATUS_TIMEOUT;
    else if (code != CURLE_OK || curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response) != CURLE_OK ||
        response != 200L || sink.received != sink.expected) status = UMI_STATUS_IO_ERROR;
    else status = UMI_STATUS_OK;
done:
    if (sink.file != NULL) {
        UmiStatus closed = UmiOutputFileClose(sink.file);
        if (closed != UMI_STATUS_OK) status = closed;
        UmiOutputFileDestroy(sink.file);
    }
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    if (status != UMI_STATUS_OK) return status;
    /* Reopen and hash the stored bytes, not only the incoming network chunks.
     * A short, changed or wrong-digest file is retained, never reported ready. */
    UmiModelInspection inspected;
    status = UmiModelInspect(checked.destination, checked.source.sha256, cancellation, &inspected);
    if (status == UMI_STATUS_OK && inspected.bytes != checked.source.bytes) status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) *outInspection = inspected;
    return status;
#undef MODEL_CURL
#endif
}
