/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/interaction_recording.c
 *
 * PURPOSE:
 *   Keep interaction evidence bounded, private by default and safely escaped.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/interaction_recording.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiUiRecording {
    char applicationId[UMI_UI_ID_CAPACITY];
    char buildIdentity[UMI_UI_DESCRIPTION_CAPACITY];
    UmiUiRecordingEntry *entries;
    size_t count;
    size_t capacity;
    int active;
    int limitReached;
};

/* Validate complete UTF-8 scalar values while locating the terminator. This
 * stops at NUL; it never treats a short caller string as a full-size array. */
static int ValidText(const char *text, size_t capacity)
{
    size_t i = 0U;
    if (text == NULL) return 0;
    while (i < capacity) {
        const unsigned char c = (unsigned char)text[i++];
        unsigned need;
        uint32_t value, minimum;
        if (c == 0U) return 1;
        if (c < 0x80U) continue;
        if (c >= 0xc2U && c <= 0xdfU) { need = 1U; value = c & 0x1fU; minimum = 0x80U; }
        else if (c >= 0xe0U && c <= 0xefU) { need = 2U; value = c & 0x0fU; minimum = 0x800U; }
        else if (c >= 0xf0U && c <= 0xf4U) { need = 3U; value = c & 0x07U; minimum = 0x10000U; }
        else return 0;
        while (need-- != 0U) {
            unsigned char next;
            if (i >= capacity) return 0;
            next = (unsigned char)text[i++];
            if ((next & 0xc0U) != 0x80U) return 0;
            value = (value << 6U) | (uint32_t)(next & 0x3fU);
        }
        if (value < minimum || value > 0x10ffffU ||
            (value >= 0xd800U && value <= 0xdfffU)) return 0;
    }
    return 0;
}

/* Reports may link only to a PNG directly beside the report. No path, URL or
 * HTML syntax is accepted. The exporter cannot link to outside local files. */
static int ValidAttachment(const char *name)
{
    size_t i, n = strlen(name);
    if (n == 0U) return 1;
    if (n < 5U || strcmp(name + n - 4U, ".png") != 0) return 0;
    for (i = 0U; i < n - 4U; ++i) {
        char c = name[i];
        if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
              (c >= '0' && c <= '9') || c == '-' || c == '_')) return 0;
    }
    return 1;
}

UmiStatus UmiUiRecordingCreate(const char *applicationId, const char *buildIdentity,
    size_t capacity, UmiUiRecording **outRecording)
{
    UmiUiRecording *r;
    if (outRecording == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outRecording = NULL;
    if (!ValidText(applicationId, UMI_UI_ID_CAPACITY) || applicationId[0] == '\0' ||
        !ValidText(buildIdentity, UMI_UI_DESCRIPTION_CAPACITY) ||
        capacity == 0U || capacity > UMI_UI_RECORDING_MAX_ENTRIES)
        return UMI_STATUS_INVALID_ARGUMENT;
    r = calloc(1U, sizeof(*r));
    if (r == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    r->entries = calloc(capacity, sizeof(*r->entries));
    if (r->entries == NULL) { free(r); return UMI_STATUS_OUT_OF_MEMORY; }
    memcpy(r->applicationId, applicationId, strlen(applicationId) + 1U);
    memcpy(r->buildIdentity, buildIdentity, strlen(buildIdentity) + 1U);
    r->capacity = capacity;
    *outRecording = r;
    return UMI_STATUS_OK;
}
void UmiUiRecordingDestroy(UmiUiRecording *r)
{
    if (r == NULL) return;
    free(r->entries);
    free(r);
}
UmiStatus UmiUiRecordingSetActive(UmiUiRecording *r, int active)
{
    if (r == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (active && r->count == r->capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    r->active = active != 0;
    return UMI_STATUS_OK;
}
int UmiUiRecordingIsActive(const UmiUiRecording *r) { return r != NULL && r->active; }
size_t UmiUiRecordingCount(const UmiUiRecording *r) { return r != NULL ? r->count : 0U; }
int UmiUiRecordingLimitReached(const UmiUiRecording *r) { return r != NULL && r->limitReached; }

UmiStatus UmiUiRecordingAppend(UmiUiRecording *r, const UmiUiRecordingEntry *e)
{
    if (r == NULL || e == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!r->active) return UMI_STATUS_INVALID_STATE;
    if (e->kind < UMI_UI_RECORDING_INPUT || e->kind > UMI_UI_RECORDING_AUTOMATION ||
        e->status < UMI_STATUS_OK || e->status > UMI_STATUS_BUSY ||
        (e->outcomeKnown != 0 && e->outcomeKnown != 1) ||
        !ValidText(e->targetId, sizeof(e->targetId)) ||
        !ValidText(e->action, sizeof(e->action)) || e->action[0] == '\0' ||
        !ValidText(e->detail, sizeof(e->detail)) ||
        !ValidText(e->attachment, sizeof(e->attachment)) ||
        !ValidAttachment(e->attachment)) return UMI_STATUS_INVALID_ARGUMENT;
    if (r->count != 0U && e->elapsedMs < r->entries[r->count - 1U].elapsedMs)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (r->count == r->capacity) {
        r->active = 0; r->limitReached = 1;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    r->entries[r->count++] = *e;
    if (r->count == r->capacity) { r->active = 0; r->limitReached = 1; }
    return UMI_STATUS_OK;
}
UmiStatus UmiUiRecordingAt(const UmiUiRecording *r, size_t index, UmiUiRecordingEntry *outEntry)
{
    if (r == NULL || outEntry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= r->count) return UMI_STATUS_NOT_FOUND;
    *outEntry = r->entries[index];
    return UMI_STATUS_OK;
}
UmiStatus UmiUiRecordingAppendAutomation(UmiUiRecording *r, uint64_t elapsedMs,
    const UmiUiAutomationStepResult *result)
{
    UmiUiRecordingEntry e = {0};
    if (result == NULL ||
        !ValidText(result->step.target_id, sizeof(result->step.target_id)) ||
        result->step.operation < UMI_UI_AUTOMATION_FOCUS ||
        result->step.operation > UMI_UI_AUTOMATION_CAPTURE_EVIDENCE)
        return UMI_STATUS_INVALID_ARGUMENT;
    e.elapsedMs = elapsedMs; e.kind = UMI_UI_RECORDING_AUTOMATION;
    e.status = result->status; e.outcomeKnown = 1;
    memcpy(e.targetId, result->step.target_id, strlen(result->step.target_id) + 1U);
    (void)snprintf(e.action, sizeof(e.action), "automation-operation-%u", (unsigned)result->step.operation);
    (void)snprintf(e.detail, sizeof(e.detail), "Driver step result. Input values and observed text omitted.");
    return UmiUiRecordingAppend(r, &e);
}

/* Keep the first output error. No later write conceals a failed disk/sink. */
typedef struct Writer { UmiUiRecordingWriteFn fn; void *context; UmiStatus status; } Writer;
static void Emit(Writer *w, const char *s)
{
    if (w->status == UMI_STATUS_OK) w->status = w->fn(w->context, s, strlen(s));
}
static void Number(Writer *w, uint64_t n)
{
    char text[32]; (void)snprintf(text, sizeof(text), "%" PRIu64, n); Emit(w, text);
}
/* Valid UTF-8 bytes pass through; JSON/HTML syntax characters are escaped. */
static void Escape(Writer *w, const char *text, int html)
{
    const unsigned char *p = (const unsigned char *)text;
    for (; *p != 0U && w->status == UMI_STATUS_OK; ++p) {
        char one[8] = { (char)*p, '\0' };
        if (html) {
            if (*p == '&') Emit(w, "&amp;");
            else if (*p == '<') Emit(w, "&lt;");
            else if (*p == '>') Emit(w, "&gt;");
            else if (*p == '"') Emit(w, "&quot;");
            else if (*p == '\'') Emit(w, "&#39;");
            else if (*p < 0x20U && *p != '\n' && *p != '\t') Emit(w, " ");
            else Emit(w, one);
        } else {
            if (*p == '"' || *p == '\\') { one[0] = '\\'; one[1] = (char)*p; one[2] = '\0'; }
            else if (*p < 0x20U) (void)snprintf(one, sizeof(one), "\\u%04x", (unsigned)*p);
            Emit(w, one);
        }
    }
}
static void String(Writer *w, const char *s) { Emit(w, "\""); Escape(w, s, 0); Emit(w, "\""); }
/* Shared row serializer serves both streaming and complete exports. */
static void WriteEntry(Writer *w, uint64_t sequence, const UmiUiRecordingEntry *e)
{
        Emit(w, "{\"sequence\":"); Number(w, sequence);
        Emit(w, ",\"elapsed_ms\":"); Number(w, e->elapsedMs);
        Emit(w, ",\"kind\":"); Number(w, (uint64_t)e->kind);
        Emit(w, ",\"outcome_known\":"); Emit(w, e->outcomeKnown ? "true" : "false");
        Emit(w, ",\"status\":"); Number(w, (uint64_t)e->status);
        Emit(w, ",\"target_id\":"); String(w, e->targetId);
        Emit(w, ",\"action\":"); String(w, e->action);
        Emit(w, ",\"detail\":"); String(w, e->detail);
        Emit(w, ",\"attachment\":"); String(w, e->attachment); Emit(w, "}");
}
UmiStatus UmiUiRecordingWriteEntryJson(const UmiUiRecording *r, size_t index,
    UmiUiRecordingWriteFn write, void *context)
{
    Writer w = {write, context, UMI_STATUS_OK};
    if (r == NULL || write == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= r->count) return UMI_STATUS_NOT_FOUND;
    WriteEntry(&w, (uint64_t)index + 1U, &r->entries[index]);
    Emit(&w, "\n");
    return w.status;
}
UmiStatus UmiUiRecordingWriteJson(const UmiUiRecording *r, UmiUiRecordingWriteFn write, void *context)
{
    Writer w = {write, context, UMI_STATUS_OK}; size_t i;
    if (r == NULL || write == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    Emit(&w, "{\"schema\":\"umicom.ui.recording\",\"schema_revision\":1,\"application_id\":"); String(&w, r->applicationId);
    Emit(&w, ",\"build_identity\":"); String(&w, r->buildIdentity);
    Emit(&w, ",\"active\":"); Emit(&w, r->active ? "true" : "false");
    Emit(&w, ",\"limit_reached\":"); Emit(&w, r->limitReached ? "true" : "false");
    Emit(&w, ",\"count\":"); Number(&w, (uint64_t)r->count);
    Emit(&w, ",\"entries\":[\n");
    for (i = 0U; i < r->count && w.status == UMI_STATUS_OK; ++i) {
        const UmiUiRecordingEntry *e = &r->entries[i];
        if (i != 0U) Emit(&w, ",\n");
        WriteEntry(&w, (uint64_t)i + 1U, e);
    }
    Emit(&w, "\n]}\n"); return w.status;
}
UmiStatus UmiUiRecordingWriteHtml(const UmiUiRecording *r, UmiUiRecordingWriteFn write, void *context)
{
    Writer w = {write, context, UMI_STATUS_OK}; size_t i;
    if (r == NULL || write == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    Emit(&w, "<!doctype html><html lang=\"en-GB\"><meta charset=\"utf-8\"><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\"><title>GUI interaction report</title><style>body{font:16px/1.6 system-ui;margin:2em;max-width:1100px}table{border-collapse:collapse;width:100%}td,th{border:1px solid #ccc;padding:10px;vertical-align:top;text-align:left}td{overflow-wrap:anywhere}pre{white-space:pre-wrap} .table{overflow:auto}</style><h1>GUI interaction report</h1><p>Application: ");
    Escape(&w, r->applicationId, 1); Emit(&w, "</p><p>Build: "); Escape(&w, r->buildIdentity, 1);
    Emit(&w, "</p><p>Inputs show what was attempted. Check the file or application state to establish the outcome. Typed characters and clipboard contents are not collected by the recorder.</p>");
    if (r->limitReached) Emit(&w, "<p><strong>The recording reached its event limit and stopped.</strong></p>");
    if (r->active) Emit(&w, "<p>Recording was active when this snapshot was exported.</p>");
    Emit(&w, "<div class=\"table\"><table><tr><th>Step</th><th>Time (ms)</th><th>Control</th><th>Action</th><th>Details</th><th>Outcome</th></tr>");
    for (i = 0U; i < r->count && w.status == UMI_STATUS_OK; ++i) {
        const UmiUiRecordingEntry *e = &r->entries[i];
        Emit(&w, "<tr><td>"); Number(&w, (uint64_t)i + 1U); Emit(&w, "</td><td>"); Number(&w, e->elapsedMs);
        Emit(&w, "</td><td>"); Escape(&w, e->targetId, 1); Emit(&w, "</td><td>"); Escape(&w, e->action, 1);
        Emit(&w, "</td><td><pre>"); Escape(&w, e->detail, 1); Emit(&w, "</pre>");
        if (e->attachment[0] != '\0') {
            Emit(&w, "<a href=\""); Escape(&w, e->attachment, 1); Emit(&w, "\">Open screenshot</a>");
        }
        Emit(&w, "</td><td>");
        if (e->outcomeKnown) Escape(&w, umi_status_text(e->status), 1);
        else Emit(&w, "Not asserted");
        Emit(&w, "</td></tr>");
    }
    Emit(&w, "</table></div></html>\n"); return w.status;
}
