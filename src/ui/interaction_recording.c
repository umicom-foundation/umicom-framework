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
#include "umicom/ui/control_inventory.h"
#include "ui_internal.h"
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

/* Reuse the UI text boundary used by scenarios and control inventories. */
static int ValidText(const char *text, size_t capacity)
{
    return umi_ui_text_is_valid_utf8(text, capacity);
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
        result->step.operation > UMI_UI_AUTOMATION_ASSERT_SELECTED)
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

/* Control inventories share the recorder's escaping and failure-aware sink.
 * Their schema is separate: reading a control is not recording a user action. */
UmiStatus UmiUiControlInventoryWriteJson(const UmiUiControlInventory *inventory,
    UmiUiRecordingWriteFn write, void *context)
{
    Writer w = {write, context, UMI_STATUS_OK};
    UmiUiControlInventorySummary summary;
    size_t i;
    if (inventory == NULL || write == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (UmiUiControlInventorySummarise(inventory, &summary) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    Emit(&w, "{\"schema\":\"umicom.ui.controls\",\"schema_revision\":1,");
    Emit(&w, "\"values_recorded\":false,\"summary\":{\"controls\":"); Number(&w, summary.controls);
    Emit(&w, ",\"windows\":"); Number(&w, summary.windows);
    Emit(&w, ",\"interactive\":"); Number(&w, summary.interactive);
    Emit(&w, ",\"tagged\":"); Number(&w, summary.tagged);
    Emit(&w, ",\"inherited_targets\":"); Number(&w, summary.inheritedTargets);
    Emit(&w, ",\"missing_targets\":"); Number(&w, summary.unaddressableInteractive);
    Emit(&w, ",\"ambiguous_targets\":"); Number(&w, summary.ambiguousTargets);
    Emit(&w, ",\"private_controls\":"); Number(&w, summary.privateControls);
    Emit(&w, "},\"controls\":[\n");
    for (i = 0U; i < summary.controls && w.status == UMI_STATUS_OK; ++i) {
        UmiUiControlRecord record;
        size_t target;
        w.status = UmiUiControlInventoryAt(inventory, i, &record);
        if (w.status != UMI_STATUS_OK) break;
        if (i != 0U) Emit(&w, ",\n");
        Emit(&w, "{\"index\":"); Number(&w, i);
        Emit(&w, ",\"parent_index\":");
        if (record.parentIndex == UMI_UI_CONTROL_NO_PARENT) Emit(&w, "null");
        else Number(&w, record.parentIndex);
        Emit(&w, ",\"scope_id\":"); Number(&w, record.scopeId);
        Emit(&w, ",\"automation_id\":"); String(&w, record.automationId);
        Emit(&w, ",\"role\":"); String(&w, record.roleName);
        Emit(&w, ",\"target_index\":");
        if (UmiUiControlInventoryTarget(inventory, i, &target) == UMI_STATUS_OK) Number(&w, target);
        else Emit(&w, "null");
        Emit(&w, ",\"visible\":"); Emit(&w, record.visible ? "true" : "false");
        Emit(&w, ",\"mapped\":"); Emit(&w, record.mapped ? "true" : "false");
        Emit(&w, ",\"enabled\":"); Emit(&w, record.enabled ? "true" : "false");
        Emit(&w, ",\"focused\":"); Emit(&w, record.focused ? "true" : "false");
        Emit(&w, ",\"interactive\":"); Emit(&w, record.interactive ? "true" : "false");
        Emit(&w, ",\"private\":"); Emit(&w, record.privateControl ? "true" : "false");
        Emit(&w, "}");
    }
    Emit(&w, "\n]}\n");
    return w.status;
}

UmiStatus UmiUiControlInventoryWriteHtml(const UmiUiControlInventory *inventory,
    UmiUiRecordingWriteFn write, void *context)
{
    Writer w = {write, context, UMI_STATUS_OK};
    UmiUiControlInventorySummary summary;
    size_t i;
    if (inventory == NULL || write == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (UmiUiControlInventorySummarise(inventory, &summary) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    Emit(&w, "<!doctype html><html lang=\"en-GB\"><meta charset=\"utf-8\">"
        "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
        "<title>Window control map</title><style>body{font:16px/1.65 system-ui;margin:24px;"
        "color:#213440}main{max-width:1200px;margin:auto}.table-wrap{overflow:auto}"
        "table{border-collapse:collapse;width:100%}th,td{border:1px solid #ccd6df;"
        "padding:9px;text-align:left;vertical-align:top}code{overflow-wrap:anywhere}"
        "th{background:#edf3f6}</style><main><h1>Window control map</h1>"
        "<p>This map lists control types, test identifiers and availability. "
        "It does not read document text, passwords, labels or clipboard contents. "
        "A control listed here still needs an action test to show that it works.</p><p>");
    Number(&w, summary.controls); Emit(&w, " controls in "); Number(&w, summary.windows);
    Emit(&w, " windows; "); Number(&w, summary.unaddressableInteractive);
    Emit(&w, " interactive controls without a test target; "); Number(&w, summary.ambiguousTargets);
    Emit(&w, " tagged records share an identifier in the same window; ");
    Number(&w, summary.privateControls); Emit(&w, " private subtrees omitted.</p>"
        "<p>A child of a tagged composite control can use its parent's identifier. "
        "Missing identifiers are testability gaps, not proof of missing callbacks. "
        "Disabled or hidden controls may be correct for the current application state. "
        "Mapped does not mean unobscured by another window.</p>"
        "<div class=\"table-wrap\"><table><thead><tr><th>Index / parent</th><th>Window</th>"
        "<th>Type</th><th>Direct test ID</th><th>Input target</th><th>State</th></tr></thead><tbody>");
    for (i = 0U; i < summary.controls && w.status == UMI_STATUS_OK; ++i) {
        UmiUiControlRecord record;
        size_t target;
        w.status = UmiUiControlInventoryAt(inventory, i, &record);
        if (w.status != UMI_STATUS_OK) break;
        Emit(&w, "<tr><td>"); Number(&w, i); Emit(&w, " / ");
        if (record.parentIndex == UMI_UI_CONTROL_NO_PARENT) Emit(&w, "root");
        else Number(&w, record.parentIndex);
        Emit(&w, "</td><td>"); Number(&w, record.scopeId);
        Emit(&w, "</td><td><code>"); Escape(&w, record.roleName, 1);
        Emit(&w, "</code></td><td><code>"); Escape(&w, record.automationId, 1);
        Emit(&w, "</code></td><td>");
        if (UmiUiControlInventoryTarget(inventory, i, &target) == UMI_STATUS_OK) Number(&w, target);
        else Emit(&w, record.privateControl ? "Private" : record.interactive ? "No test ID" : "Not assigned");
        Emit(&w, "</td><td>"); Emit(&w, record.visible ? "Visible; " : "Hidden; ");
        Emit(&w, record.mapped ? "mapped; " : "unmapped; ");
        Emit(&w, record.enabled ? "enabled" : "disabled");
        if (record.focused) Emit(&w, "; focused");
        Emit(&w, "</td></tr>");
    }
    Emit(&w, "</tbody></table></div><p>Keep this map beside the interaction report and "
        "the expected result. A developer can use its identifiers to reproduce the "
        "same operation without relying on a button's screen position.</p></main></html>\n");
    return w.status;
}
