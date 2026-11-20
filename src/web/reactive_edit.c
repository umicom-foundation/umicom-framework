/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/reactive_edit.c
 *
 * PURPOSE:
 *   Admit one bounded browser edit and apply it through the existing state owner.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/web/reactive_edit.h"

#include "umicom/base/ascii_number.h"
#include "umicom/protocol/json_writer.h"

#include <string.h>

/* Public configuration layouts stay typed. Both adapters use the same
 * authorization, revision and atomic publication path in this module. */
typedef struct ReactiveEdit {
    const char *path;
    const char *key;
    UmiUiReactiveObservableStore *store;
    const UmiWebOriginPolicy *origins;
    UmiWebRequestGate authorize;
    void *authorizeContext;
    UmiUiValueKind kind;
    int64_t minimum;
    int64_t maximum;
} ReactiveEdit;

static UmiStatus EditCacheHeaders(UmiWebResponse *reply)
{
    UmiStatus status = umi_web_response_set_header(reply, "Cache-Control", "no-store");
    if (status == UMI_STATUS_OK)
        status = umi_web_response_set_header(reply, "X-Content-Type-Options", "nosniff");
    return status;
}

static UmiStatus EditReply(UmiWebResponse *reply, int code, const char *body)
{
    UmiStatus status = umi_web_response_set_text(reply, code, "application/json", body);
    return status == UMI_STATUS_OK ? EditCacheHeaders(reply) : status;
}

/* Require an unambiguous value whether the handler is reached through the
 * HTTP parser or called by another native host with a populated request. */
static const char *SingleHeader(const UmiWebRequest *request, const char *name)
{
    const char *value = NULL;
    for (size_t index = 0U; index < request->header_count; ++index) {
        const UmiWebHeader *header = &request->headers[index];
        if (!umi_web_header_name_equal(header, name)) continue;
        if (value != NULL || memchr(header->value, '\0', sizeof header->value) == NULL)
            return NULL;
        value = header->value;
    }
    return value;
}

/* The original integer-only parser remains below for review. The active
 * parser keeps the same bounded revision proof and decodes the configured type. */
#if 0
static UmiStatus ParseEdit(const UmiWebRequest *request, uint64_t *revision, int64_t *value)
{
    const char revisionPrefix[] = "revision=";
    const char valuePrefix[] = "&value=";
    const size_t start = sizeof revisionPrefix - 1U;
    const size_t separator = sizeof valuePrefix - 1U;
    const size_t maximum = sizeof "revision=18446744073709551615&value=-9223372036854775808" - 1U;
    if (request->query[0] != '\0' || request->body_length < start + separator + 2U ||
        request->body_length > maximum || memcmp(request->body, revisionPrefix, start) != 0)
        return UMI_STATUS_PARSE_ERROR;
    const char *baseText = request->body + start;
    const char *end = request->body + request->body_length;
    const char *separatorAt = memchr(baseText, '&', (size_t)(end - baseText));
    if (separatorAt == NULL || separatorAt == baseText || (size_t)(end - separatorAt) <= separator ||
        memcmp(separatorAt, valuePrefix, separator) != 0 ||
        (baseText[0] == '0' && separatorAt != baseText + 1U))
        return UMI_STATUS_PARSE_ERROR;
    const char *valueText = separatorAt + separator;
    if (valueText[0] == '+' || (valueText[0] == '0' && end != valueText + 1U) ||
        (end - valueText >= 2 && valueText[0] == '-' && valueText[1] == '0'))
        return UMI_STATUS_PARSE_ERROR;
    UmiStatus status = umi_ascii_uint64_parse(baseText, (size_t)(separatorAt - baseText), revision);
    if (status == UMI_STATUS_OK)
        status = umi_ascii_int64_parse(valueText, (size_t)(end - valueText), value);
    return status;
}
#endif

static UmiStatus ParseEditValue(const UmiWebRequest *request, UmiUiValueKind kind,
    uint64_t *revision, UmiUiValue *value)
{
    const char revisionPrefix[] = "revision=";
    const char valuePrefix[] = "&value=";
    const size_t start = sizeof revisionPrefix - 1U;
    const size_t separator = sizeof valuePrefix - 1U;
    const size_t maximum = sizeof "revision=18446744073709551615&value=-9223372036854775808" - 1U;
    if (request->query[0] != '\0' || request->body_length < start + separator + 2U ||
        request->body_length > maximum || memcmp(request->body, revisionPrefix, start) != 0)
        return UMI_STATUS_PARSE_ERROR;
    const char *baseText = request->body + start;
    const char *end = request->body + request->body_length;
    const char *separatorAt = memchr(baseText, '&', (size_t)(end - baseText));
    if (separatorAt == NULL || separatorAt == baseText || (size_t)(end - separatorAt) <= separator ||
        memcmp(separatorAt, valuePrefix, separator) != 0 ||
        (baseText[0] == '0' && separatorAt != baseText + 1U))
        return UMI_STATUS_PARSE_ERROR;
    const char *valueText = separatorAt + separator;
    const size_t length = (size_t)(end - valueText);
    UmiStatus status = umi_ascii_uint64_parse(baseText, (size_t)(separatorAt - baseText), revision);
    if (status != UMI_STATUS_OK) return status;
    /* Boolean values remain distinct from integer and text values. The wire
     * accepts exactly one of the two lower-case literals without coercion. */
    if (kind == UMI_UI_VALUE_BOOLEAN) {
        if (length == 4U && memcmp(valueText, "true", 4U) == 0) return umi_ui_value_set_boolean(value, 1);
        if (length == 5U && memcmp(valueText, "false", 5U) == 0) return umi_ui_value_set_boolean(value, 0);
        return UMI_STATUS_PARSE_ERROR;
    }
    if (valueText[0] == '+' || (valueText[0] == '0' && length != 1U) ||
        (length >= 2U && valueText[0] == '-' && valueText[1] == '0'))
        return UMI_STATUS_PARSE_ERROR;
    int64_t integer;
    status = umi_ascii_int64_parse(valueText, length, &integer);
    return status == UMI_STATUS_OK ? umi_ui_value_set_integer(value, integer) : status;
}

static UmiStatus Acknowledge(UmiWebResponse *reply, uint64_t revision)
{
    char body[128];
    UmiJsonWriter writer;
    UmiJsonWriterInit(&writer, body, sizeof body);
    UmiJsonWriterRaw(&writer, "{\"schema\":\"umicom.reactive.edit/1\",\"revision\":\"");
    UmiJsonWriterUInt64(&writer, revision);
    UmiStatus status = UmiJsonWriterRaw(&writer, "\"}\n");
    return status == UMI_STATUS_OK ? EditReply(reply, UMI_HTTP_STATUS_OK, body) : status;
}

/* The earlier integer commit remains for review. The shared commit below
 * takes a typed value and keeps its state and acknowledgement ordering. */
#if 0
static UmiStatus ApplyIntegerEdit(const UmiWebRequest *request, UmiWebResponse *reply,
                                  const UmiWebReactiveIntegerEdit *edit)
{
    uint64_t revision;
    int64_t value;
    if (ParseEdit(request, &revision, &value) != UMI_STATUS_OK)
        return EditReply(reply, UMI_HTTP_STATUS_BAD_REQUEST, "{\"error\":\"body\"}\n");
    if (value < edit->minimum || value > edit->maximum)
        return EditReply(reply, UMI_HTTP_STATUS_BAD_REQUEST, "{\"error\":\"range\"}\n");
    if (revision != edit->store->revision)
        return EditReply(reply, UMI_HTTP_STATUS_CONFLICT, "{\"error\":\"revision\"}\n");
    if (revision == UINT64_MAX)
        return EditReply(reply, UMI_HTTP_STATUS_CONFLICT, "{\"error\":\"revision-exhausted\"}\n");

    UmiUiReactiveChangeEvent event = {0};
    UmiStatus status = umi_ui_reactive_observable_store_get(edit->store, edit->key, &event.before_value);
    if (status != UMI_STATUS_OK) return status;
    if (event.before_value.kind != UMI_UI_VALUE_INTEGER) return UMI_STATUS_INVALID_STATE;
    memcpy(event.path, edit->key, strlen(edit->key) + 1U);
    event.after_value = (UmiUiValue){.kind = UMI_UI_VALUE_INTEGER, .integer_value = value};
    UmiUiReactiveChangeTransaction transaction = {0};
    UmiUiReactiveBatchUpdate batch;
    status = umi_ui_reactive_change_transaction_begin(&transaction, "browser.integer.edit", revision);
    if (status == UMI_STATUS_OK) status = Acknowledge(reply, revision + 1U);
    /* Prepare the complete reply before mutating. The store owns the compare
     * and commit together, so an operational refusal cannot publish success. */
    if (status == UMI_STATUS_OK)
        status = umi_ui_reactive_observable_store_apply(edit->store, &transaction, &event, 1U, &batch);
    return status;
}
#endif

static UmiStatus ApplyEdit(const UmiWebRequest *request, UmiWebResponse *reply, const ReactiveEdit *edit)
{
    uint64_t revision;
    UmiUiValue value;
    if (ParseEditValue(request, edit->kind, &revision, &value) != UMI_STATUS_OK)
        return EditReply(reply, UMI_HTTP_STATUS_BAD_REQUEST, "{\"error\":\"body\"}\n");
    if (edit->kind == UMI_UI_VALUE_INTEGER && (value.integer_value < edit->minimum || value.integer_value > edit->maximum))
        return EditReply(reply, UMI_HTTP_STATUS_BAD_REQUEST, "{\"error\":\"range\"}\n");
    if (revision != edit->store->revision)
        return EditReply(reply, UMI_HTTP_STATUS_CONFLICT, "{\"error\":\"revision\"}\n");
    if (revision == UINT64_MAX)
        return EditReply(reply, UMI_HTTP_STATUS_CONFLICT, "{\"error\":\"revision-exhausted\"}\n");

    UmiUiReactiveChangeEvent event = {0};
    UmiStatus status = umi_ui_reactive_observable_store_get(edit->store, edit->key, &event.before_value);
    if (status != UMI_STATUS_OK) return status;
    if (event.before_value.kind != edit->kind) return UMI_STATUS_INVALID_STATE;
    memcpy(event.path, edit->key, strlen(edit->key) + 1U);
    event.after_value = value;
    UmiUiReactiveChangeTransaction transaction = {0};
    UmiUiReactiveBatchUpdate batch;
    const char *operation = edit->kind == UMI_UI_VALUE_BOOLEAN ? "browser.boolean.edit" : "browser.integer.edit";
    status = umi_ui_reactive_change_transaction_begin(&transaction, operation, revision);
    if (status == UMI_STATUS_OK) status = Acknowledge(reply, revision + 1U);
    /* Build the complete response before the store compares and commits the
     * typed change. A failed transaction cannot publish an acknowledgement. */
    if (status == UMI_STATUS_OK)
        status = umi_ui_reactive_observable_store_apply(edit->store, &transaction, &event, 1U, &batch);
    return status;
}

static UmiStatus HandleEdit(const UmiWebRequest *request, UmiWebResponse *response, const ReactiveEdit *edit)
{
    if (request == NULL || response == NULL || edit == NULL || edit->path == NULL ||
        edit->path[0] != '/' || memchr(request->path, '\0', sizeof request->path) == NULL ||
        request->header_count > UMI_WEB_MAX_HEADERS || edit->store == NULL ||
        edit->origins == NULL || edit->authorize == NULL || edit->key == NULL ||
        edit->key[0] == '\0' || strlen(edit->key) >= UMI_UI_REACTIVE_PATH_CAPACITY ||
        edit->minimum > edit->maximum)
        return UMI_STATUS_INVALID_ARGUMENT;

    UmiWebResponse reply;
    UmiStatus status;
    if (strcmp(request->path, edit->path) != 0) {
        status = EditReply(&reply, UMI_HTTP_STATUS_NOT_FOUND, "{\"error\":\"edit\"}\n");
    } else if (request->method != UMI_HTTP_METHOD_POST) {
        status = EditReply(&reply, UMI_HTTP_STATUS_METHOD_NOT_ALLOWED, "{\"error\":\"method\"}\n");
        if (status == UMI_STATUS_OK) status = umi_web_response_set_header(&reply, "Allow", "POST");
    } else if (!umi_web_origin_is_allowed(edit->origins, SingleHeader(request, "Origin"))) {
        status = EditReply(&reply, UMI_HTTP_STATUS_FORBIDDEN, "{\"error\":\"origin\"}\n");
    } else {
        bool accepted = false;
        status = umi_web_response_set_text(&reply, UMI_HTTP_STATUS_FORBIDDEN,
                                           "application/json", "{\"error\":\"permission\"}\n");
        if (status == UMI_STATUS_OK)
            status = edit->authorize(request, &reply, &accepted, edit->authorizeContext);
        if (status == UMI_STATUS_OK && !accepted) status = EditCacheHeaders(&reply);
        if (status == UMI_STATUS_OK && accepted) {
            const char *type = SingleHeader(request, "Content-Type");
            if (type == NULL || strcmp(type, UMI_WEB_REACTIVE_EDIT_CONTENT_TYPE) != 0)
                status = EditReply(&reply, UMI_HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE, "{\"error\":\"type\"}\n");
            else
                status = ApplyEdit(request, &reply, edit);
        }
    }
    if (status == UMI_STATUS_OK) *response = reply;
    return status;
}

/* Convert the public integer and Boolean application configurations without
 * weakening any of the origin, request gate or existing-key requirements. */
UmiStatus UmiWebReactiveIntegerEditHandle(const UmiWebRequest *request,
    UmiWebResponse *response, void *context)
{
    const UmiWebReactiveIntegerEdit *edit = context;
    if (edit == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    const ReactiveEdit binding = {edit->path, edit->key, edit->store, edit->origins, edit->authorize,
        edit->authorizeContext, UMI_UI_VALUE_INTEGER, edit->minimum, edit->maximum};
    return HandleEdit(request, response, &binding);
}

UmiStatus UmiWebReactiveBooleanEditHandle(const UmiWebRequest *request,
    UmiWebResponse *response, void *context)
{
    const UmiWebReactiveBooleanEdit *edit = context;
    if (edit == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    const ReactiveEdit binding = {.path = edit->path, .key = edit->key, .store = edit->store,
        .origins = edit->origins, .authorize = edit->authorize, .authorizeContext = edit->authorizeContext,
        .kind = UMI_UI_VALUE_BOOLEAN};
    return HandleEdit(request, response, &binding);
}
