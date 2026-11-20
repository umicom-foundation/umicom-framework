/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/reactive_view.c
 *
 * PURPOSE:
 *   Encode native reactive deltas through a bounded, read-only HTTP route.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/reactive_view.h"
#include "umicom/base/ascii_number.h"
#include "umicom/protocol/json_writer.h"
#include <stdlib.h>
#include <string.h>

static UmiStatus PrepareReply(UmiWebResponse *response, int code, const char *body)
{
    UmiStatus status = umi_web_response_set_text(response, code, "application/json", body);
    if (status == UMI_STATUS_OK)
        status = umi_web_response_set_header(response, "Cache-Control", "no-store");
    if (status == UMI_STATUS_OK)
        status = umi_web_response_set_header(response, "X-Content-Type-Options", "nosniff");
    return status;
}

static UmiStatus ReadCursor(const char *query, size_t length, uint64_t *out)
{
    if (length == 0U) {
        *out = 0U;
        return UMI_STATUS_OK;
    }
    const char prefix[] = "since=";
    const size_t start = sizeof prefix - 1U;
    if (length <= start || memcmp(query, prefix, start) != 0 ||
        (query[start] == '0' && length != start + 1U))
        return UMI_STATUS_PARSE_ERROR;
    return umi_ascii_uint64_parse(query + start, length - start, out);
}

static void WriteRevision(UmiJsonWriter *writer, uint64_t revision)
{
    (void)UmiJsonWriterRaw(writer, "\"");
    (void)UmiJsonWriterUInt64(writer, revision);
    (void)UmiJsonWriterRaw(writer, "\"");
}

static UmiStatus WriteValue(UmiJsonWriter *writer, const UmiUiValue *value)
{
    /* Transmit active payloads only. The native store also accepts nonfinite
     * real values; an HTTP frame can publish only finite JSON numbers. */
    switch (value->kind) {
    case UMI_UI_VALUE_NONE:
        (void)UmiJsonWriterRaw(writer, "\"kind\":\"none\",\"value\":null");
        break;
    case UMI_UI_VALUE_BOOLEAN:
        (void)UmiJsonWriterRaw(writer, "\"kind\":\"boolean\",\"value\":");
        (void)UmiJsonWriterBool(writer, value->boolean_value);
        break;
    case UMI_UI_VALUE_INTEGER:
        (void)UmiJsonWriterRaw(writer, "\"kind\":\"integer\",\"value\":\"");
        (void)UmiJsonWriterInt64(writer, value->integer_value);
        (void)UmiJsonWriterRaw(writer, "\"");
        break;
    case UMI_UI_VALUE_REAL: {
        char number[64];
        size_t length = 0U;
        UmiStatus status = umi_ascii_double_format(value->real_value, number, sizeof number, &length);
        if (status != UMI_STATUS_OK) return status;
        (void)UmiJsonWriterRaw(writer, "\"kind\":\"real\",\"value\":");
        (void)UmiJsonWriterRaw(writer, number);
        break;
    }
    case UMI_UI_VALUE_STRING:
        (void)UmiJsonWriterRaw(writer, "\"kind\":\"string\",\"value\":");
        (void)UmiJsonWriterString(writer, value->string_value);
        break;
    default:
        return UMI_STATUS_INVALID_STATE;
    }
    return writer->status;
}

static void FreeBody(void *context, void *bytes)
{
    (void)context;
    free(bytes);
}

static UmiStatus EncodeBody(const UmiUiReactiveKeyValue *rows, UmiUiReactiveStoreRead read,
    uint64_t since, UmiWebResponseBody **out)
{
    /* JSON escapes can expand each text byte up to six times. Cap the
     * estimate before allocation and keep the HTTP payload bound inclusive. */
    const size_t bytesPerRow = 6U * (UMI_UI_REACTIVE_PATH_CAPACITY +
        UMI_UI_VALUE_STRING_CAPACITY) + 128U;
    size_t capacity = 128U + read.row_count * bytesPerRow;
    if (capacity > UMI_WEB_RESPONSE_BODY_LIMIT + 1U) capacity = UMI_WEB_RESPONSE_BODY_LIMIT + 1U;
    unsigned char *buffer = malloc(capacity);
    if (buffer == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiJsonWriter writer;
    UmiJsonWriterInit(&writer, (char *)buffer, capacity);
    (void)UmiJsonWriterRaw(&writer, "{\"schema\":\"umicom.reactive.view/1\",\"since\":");
    WriteRevision(&writer, since);
    (void)UmiJsonWriterRaw(&writer, ",\"revision\":");
    WriteRevision(&writer, read.observed_revision);
    (void)UmiJsonWriterRaw(&writer, ",\"rows\":[");
    UmiStatus status = writer.status;
    for (size_t index = 0U; index < read.row_count && status == UMI_STATUS_OK; ++index) {
        const UmiUiReactiveKeyValue *row = &rows[index];
        (void)UmiJsonWriterRaw(&writer, index == 0U ? "{\"key\":" : ",{\"key\":");
        (void)UmiJsonWriterString(&writer, row->key);
        (void)UmiJsonWriterRaw(&writer, ",\"revision\":");
        WriteRevision(&writer, row->revision);
        (void)UmiJsonWriterRaw(&writer, ",");
        status = WriteValue(&writer, &row->value);
        if (status == UMI_STATUS_OK) status = UmiJsonWriterRaw(&writer, "}");
    }
    if (status == UMI_STATUS_OK) status = UmiJsonWriterRaw(&writer, "]}\n");
    if (status == UMI_STATUS_OK)
        status = UmiWebResponseBodyTake(&buffer, writer.length, FreeBody, NULL, out);
    free(buffer);
    return status;
}

UmiStatus UmiWebReactiveViewHandle(const UmiWebRequest *request, UmiWebResponse *response,
    UmiWebResponseBody **outBody, void *context)
{
    const UmiWebReactiveView *view = context;
    if (request == NULL || response == NULL || outBody == NULL || *outBody != NULL ||
        view == NULL || view->path == NULL || view->path[0] != '/' || view->store == NULL ||
        view->keyCount > UMI_UI_REACTIVE_MAX_ITEMS || (view->keyCount != 0U && view->keys == NULL) ||
        memchr(request->path, '\0', sizeof request->path) == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;

    UmiWebResponse reply;
    UmiWebResponseBody *body = NULL;
    UmiStatus status;
    const char *queryEnd = memchr(request->query, '\0', sizeof request->query);
    uint64_t since = 0U;
    if (strcmp(request->path, view->path) != 0) {
        status = PrepareReply(&reply, UMI_HTTP_STATUS_NOT_FOUND, "{\"error\":\"view\"}\n");
    } else if (request->method != UMI_HTTP_METHOD_GET) {
        status = PrepareReply(&reply, UMI_HTTP_STATUS_METHOD_NOT_ALLOWED, "{\"error\":\"method\"}\n");
        if (status == UMI_STATUS_OK) status = umi_web_response_set_header(&reply, "Allow", "GET");
    } else if (request->body_length != 0U || queryEnd == NULL ||
               ReadCursor(request->query, (size_t)(queryEnd - request->query), &since) != UMI_STATUS_OK) {
        status = PrepareReply(&reply, UMI_HTTP_STATUS_BAD_REQUEST, "{\"error\":\"query\"}\n");
    } else if (since > view->store->revision) {
        status = PrepareReply(&reply, UMI_HTTP_STATUS_CONFLICT, "{\"error\":\"revision\"}\n");
    } else {
        UmiUiReactiveKeyValue *rows = view->keyCount == 0U ? NULL : calloc(view->keyCount, sizeof *rows);
        if (view->keyCount != 0U && rows == NULL) return UMI_STATUS_OUT_OF_MEMORY;
        UmiUiReactiveStoreRead read;
        status = umi_ui_reactive_observable_store_read_changes(view->store, since,
            view->keys, view->keyCount, rows, view->keyCount, &read);
        if (status == UMI_STATUS_OK) status = PrepareReply(&reply, UMI_HTTP_STATUS_OK, "");
        if (status == UMI_STATUS_OK) status = EncodeBody(rows, read, since, &body);
        free(rows);
    }
    if (status != UMI_STATUS_OK) return status;
    *response = reply;
    *outBody = body;
    return UMI_STATUS_OK;
}
