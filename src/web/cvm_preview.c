/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/cvm_preview.c
 *
 * PURPOSE:
 *   Translate an admitted binary HTTP request into a bounded Umicom C preview.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/cvm_preview.h"

#include "umicom/toolchain/cvm_preview.h"
#include "umicom/base/ascii_number.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static UmiStatus Reply(UmiWebResponse *response, int code, const char *json)
{
    UmiStatus status = umi_web_response_set_text(response, code, "application/json", json);
    if (status == UMI_STATUS_OK) {
        status = umi_web_response_set_header(response, "Cache-Control", "no-store");
    }
    if (status == UMI_STATUS_OK) {
        status = umi_web_response_set_header(response, "X-Content-Type-Options", "nosniff");
    }
    return status;
}

static bool MatchesMediaType(const char *text)
{
    const size_t length = sizeof UMI_WEB_CVM_ARTIFACT_MEDIA_TYPE - 1U;
    if (strlen(text) != length) {
        return false;
    }
    for (size_t i = 0U; i < length; ++i) {
        unsigned char character = (unsigned char)text[i];
        if (character >= 'A' && character <= 'Z') {
            character = (unsigned char)(character + ('a' - 'A'));
        }
        if (character != (unsigned char)UMI_WEB_CVM_ARTIFACT_MEDIA_TYPE[i]) {
            return false;
        }
    }
    return true;
}

static bool HasArtifactContentType(const UmiWebRequest *request)
{
    bool found = false;
    if (request->header_count > UMI_WEB_MAX_HEADERS) {
        return false;
    }
    for (size_t i = 0U; i < request->header_count; ++i) {
        const UmiWebHeader *header = &request->headers[i];
        if (memchr(header->name, '\0', sizeof header->name) == NULL) {
            return false;
        }
        if (umi_web_header_name_equal(header, "Content-Type")) {
            if (found || memchr(header->value, '\0', sizeof header->value) == NULL ||
                !MatchesMediaType(header->value)) {
                return false;
            }
            found = true;
        }
        if (umi_web_header_name_equal(header, "Content-Encoding")) {
            return false;
        }
    }
    return found;
}

static bool ReadIntInput(const UmiWebRequest *request, int32_t *out)
{
    static const char prefix[] = "input=";
    const char *end = memchr(request->query, '\0', sizeof request->query);
    if (end == NULL) return false;
    const size_t length = (size_t)(end - request->query);
    const size_t first = sizeof prefix - 1U;
    /* This endpoint admits one literal decimal field. A general query map
     * would accept additional separators and decoded spellings here. */
    if (length <= first || memcmp(request->query, prefix, first) != 0) return false;
    int64_t value = 0;
    if (umi_ascii_int64_parse(request->query + first, length - first, &value) != UMI_STATUS_OK ||
        value < INT32_MIN || value > INT32_MAX) return false;
    *out = (int32_t)value;
    return true;
}

static UmiStatus HandlePreview(const UmiWebRequest *request, UmiWebResponse *response,
    const UmiWebCvmPreviewConfig *config, bool input_required)
{
    if (request == NULL || response == NULL || config == NULL ||
        config->fuel == 0U || config->fuel > UMI_WEB_CVM_PREVIEW_MAX_FUEL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (request->method != UMI_HTTP_METHOD_POST) {
        UmiStatus status = Reply(response, UMI_HTTP_STATUS_METHOD_NOT_ALLOWED, "{\"error\":\"method\"}\n");
        return status == UMI_STATUS_OK ? umi_web_response_set_header(response, "Allow", "POST") : status;
    }
    int32_t input = 0;
    if (input_required ? !ReadIntInput(request, &input) : request->query[0] != '\0') {
        return Reply(response, UMI_HTTP_STATUS_BAD_REQUEST, "{\"error\":\"query\"}\n");
    }
    if (!HasArtifactContentType(request)) {
        return Reply(response, UMI_HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE, "{\"error\":\"content-type\"}\n");
    }
    if (request->body_length >= sizeof request->body || request->body_length > UMC_BYTECODE_LIMIT) {
        return Reply(response, UMI_HTTP_STATUS_PAYLOAD_TOO_LARGE, "{\"error\":\"artifact-size\"}\n");
    }
    if (request->body_length == 0U) {
        return Reply(response, UMI_HTTP_STATUS_BAD_REQUEST, "{\"error\":\"invalid-artifact\"}\n");
    }

    /* Give each request independent scratch storage. The existing CVM adapter
     * owns bytecode verification and the runtime owns the instruction budget. */
    UmiCvmPreviewWorkspace *workspace = malloc(sizeof *workspace);
    if (workspace == NULL) {
        return Reply(response, UMI_HTTP_STATUS_INTERNAL_ERROR, "{\"error\":\"preview-unavailable\"}\n");
    }
    UmiCvmPreviewResult result = {0};
    const UmiStatus status = input_required
        ? umi_cvm_preview_int_input_bytecode((const uint8_t *)request->body, request->body_length,
            input, config->fuel, workspace, sizeof *workspace, &result, sizeof result)
        : umi_cvm_preview_bytecode((const uint8_t *)request->body, request->body_length,
            config->fuel, workspace, sizeof *workspace, &result, sizeof result);
    free(workspace);
    if (status != UMI_STATUS_OK) {
        if (status == UMI_STATUS_UNAVAILABLE) {
            return Reply(response, UMI_HTTP_STATUS_SERVICE_UNAVAILABLE, "{\"error\":\"input-unavailable\"}\n");
        }
        if (result.phase == UMI_CVM_DECODING) {
            return Reply(response, UMI_HTTP_STATUS_BAD_REQUEST, "{\"error\":\"invalid-artifact\"}\n");
        }
        if (result.diagnostic.status == UMC_FUEL_EXHAUSTED) {
            return Reply(response, UMI_HTTP_STATUS_BAD_REQUEST, "{\"error\":\"instruction-budget\"}\n");
        }
        return Reply(response, UMI_HTTP_STATUS_BAD_REQUEST, "{\"error\":\"execution-refused\"}\n");
    }
    if (!result.completed) {
        return Reply(response, UMI_HTTP_STATUS_INTERNAL_ERROR, "{\"error\":\"preview-unavailable\"}\n");
    }
    char json[120];
    const int length = snprintf(json, sizeof json, "{\"value\":%" PRId32 ",\"instructions\":%zu}\n",
        result.execution.value, result.execution.instructions_executed);
    if (length < 0 || (size_t)length >= sizeof json) {
        return UMI_STATUS_INTERNAL_ERROR;
    }
    return Reply(response, UMI_HTTP_STATUS_OK, json);
}

UmiStatus UmiWebCvmPreviewHandle(const UmiWebRequest *request, UmiWebResponse *response,
    void *context)
{
    return HandlePreview(request, response, context, false);
}

UmiStatus UmiWebCvmPreviewIntInputHandle(const UmiWebRequest *request, UmiWebResponse *response,
    void *context)
{
    return HandlePreview(request, response, context, true);
}
