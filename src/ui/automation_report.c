/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/automation_report.c
 *
 * PURPOSE:
 *   Export a copied check-session report without hiding unattempted actions or
 *   treating cancellation as successful acceptance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/automation_session.h"
#include "ui_internal.h"
#include <stdlib.h>

/* Copy before calling an external sink. The sink can close the original GUI
 * session without invalidating the immutable report currently being written. */
typedef struct ReportCopy {
    UmiUiAutomationSessionSnapshot info;
    UmiUiAutomationStepResult rows[UMI_UI_AUTOMATION_STEP_MAX];
    UmiUiAutomationStepState states[UMI_UI_AUTOMATION_STEP_MAX];
} ReportCopy;

typedef struct ReportWriter {
    UmiUiRecordingWriteFn write;
    void *context;
    UmiStatus status;
} ReportWriter;

/* Capture every planned row before the first user-supplied writer callback. */
static UmiStatus CopyReport(const UmiUiAutomationReport *report, ReportCopy **outCopy)
{
    ReportCopy *copy;
    UmiStatus status;
    size_t i;
    *outCopy = NULL;
    if (report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    copy = calloc(1U, sizeof(*copy));
    if (copy == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiUiAutomationReportSnapshot(report, &copy->info);
    for (i = 0U; status == UMI_STATUS_OK && i < copy->info.planned; ++i)
        status = UmiUiAutomationReportStep(report, i, &copy->states[i], &copy->rows[i]);
    if (status != UMI_STATUS_OK) { free(copy); return status; }
    *outCopy = copy;
    return UMI_STATUS_OK;
}

/* Never continue after a short or failed write reported by the output sink. */
static void Emit(ReportWriter *writer, const char *text)
{
    if (writer->status == UMI_STATUS_OK)
        writer->status = writer->write(writer->context, text, strlen(text));
}
/* Format a bounded count without narrowing size_t on a 64-bit host. */
static void Number(ReportWriter *writer, size_t number)
{
    char text[32];
    (void)snprintf(text, sizeof(text), "%zu", number);
    Emit(writer, text);
}
/* Keep a driver error visible as its numeric status, even if it is unknown. */
static void StatusNumber(ReportWriter *writer, UmiStatus status)
{
    char text[32];
    (void)snprintf(text, sizeof(text), "%d", (int)status);
    Emit(writer, text);
}
/* These stable words identify run state in all three report formats. */
static const char *StateName(UmiUiAutomationSessionState state)
{
    switch (state) {
        case UMI_UI_AUTOMATION_SESSION_READY: return "ready";
        case UMI_UI_AUTOMATION_SESSION_RUNNING: return "running";
        case UMI_UI_AUTOMATION_SESSION_COMPLETED: return "completed";
        case UMI_UI_AUTOMATION_SESSION_STOPPED: return "stopped";
        case UMI_UI_AUTOMATION_SESSION_CANCELLED: return "cancelled";
        default: return "unknown";
    }
}
/* Distinguish a failed attempt from a step that never reached the driver. */
static const char *StepName(UmiUiAutomationStepState state)
{
    if (state == UMI_UI_AUTOMATION_STEP_PASSED) return "passed";
    if (state == UMI_UI_AUTOMATION_STEP_FAILED) return "failed";
    return "not-run";
}

/* Check the whole bounded field before rendering it. Escaping is not UTF-8
 * repair: an invalid driver field is replaced by a fixed explanation. XML 1.0
 * cannot represent most control bytes or U+FFFE/U+FFFF, so replace those too. */
static void Escaped(ReportWriter *writer, const char *text, size_t capacity, int xml)
{
    const unsigned char *p;
    if (!umi_ui_text_is_valid_utf8(text, capacity)) {
        Emit(writer, "[invalid UTF-8]");
        return;
    }
    p = (const unsigned char *)text;
    while (*p != 0U && writer->status == UMI_STATUS_OK) {
        char byte[8] = {(char)*p, '\0'};
        if (xml) {
            if (*p == '&') Emit(writer, "&amp;");
            else if (*p == '<') Emit(writer, "&lt;");
            else if (*p == '>') Emit(writer, "&gt;");
            else if (*p == '"') Emit(writer, "&quot;");
            else if (*p == '\'') Emit(writer, "&#39;");
            else if ((*p < 0x20U && *p != '\t' && *p != '\n' && *p != '\r') ||
                (*p == 0xefU && p[1] == 0xbfU && (p[2] == 0xbeU || p[2] == 0xbfU))) {
                Emit(writer, "&#xfffd;");
                if (*p == 0xefU) p += 2;
            } else Emit(writer, byte);
        } else {
            if (*p == '"' || *p == '\\') { byte[0] = '\\'; byte[1] = (char)*p; byte[2] = '\0'; }
            else if (*p < 0x20U) (void)snprintf(byte, sizeof(byte), "\\u%04x", (unsigned)*p);
            Emit(writer, byte);
        }
        ++p;
    }
}
/* Quote one validated/escaped field; do not splice raw text into JSON. */
static void JsonString(ReportWriter *w, const char *text, size_t capacity)
{
    Emit(w, "\""); Escaped(w, text, capacity, 0); Emit(w, "\"");
}
/* Completion needs a nonempty finished plan, independently of pass counts. */
static int RunComplete(const ReportCopy *copy)
{
    return copy->info.state == UMI_UI_AUTOMATION_SESSION_COMPLETED &&
        copy->info.planned != 0U && copy->info.notRun == 0U && !copy->info.stepInProgress;
}

/* Preserve planned rows and explicit null status for actions not attempted. */
static void JsonReport(ReportWriter *w, const ReportCopy *c, int includeText)
{
    size_t i;
    Emit(w, "{\"schema\":1,\"run_id\":"); JsonString(w, c->info.runId, sizeof(c->info.runId));
    Emit(w, ",\"scenario_id\":"); JsonString(w, c->info.scenarioId, sizeof(c->info.scenarioId));
    Emit(w, ",\"driver_id\":"); JsonString(w, c->info.driverId, sizeof(c->info.driverId));
    Emit(w, ",\"state\":\""); Emit(w, StateName(c->info.state)); Emit(w, "\",\"planned\":");
    Number(w, c->info.planned); Emit(w, ",\"attempted\":"); Number(w, c->info.attempted);
    Emit(w, ",\"passed\":"); Number(w, c->info.passed);
    Emit(w, ",\"failed\":"); Number(w, c->info.failed);
    Emit(w, ",\"not_run\":"); Number(w, c->info.notRun);
    Emit(w, ",\"step_in_progress\":"); Emit(w, c->info.stepInProgress ? "true" : "false");
    Emit(w, ",\"cancellation_requested\":"); Emit(w, c->info.cancellationRequested ? "true" : "false");
    Emit(w, ",\"accepted\":"); Emit(w, RunComplete(c) && c->info.failed == 0U ? "true" : "false");
    Emit(w, ",\"text_included\":"); Emit(w, includeText ? "true" : "false");
    if (includeText) { Emit(w, ",\"title\":"); JsonString(w, c->info.title, sizeof(c->info.title)); }
    Emit(w, ",\"steps\":[");
    for (i = 0U; i < c->info.planned && w->status == UMI_STATUS_OK; ++i) {
        const UmiUiAutomationStepResult *r = &c->rows[i];
        if (i != 0U) Emit(w, ",");
        Emit(w, "{\"sequence\":"); Number(w, i + 1U);
        Emit(w, ",\"step_id\":"); JsonString(w, r->step.step_id, sizeof(r->step.step_id));
        Emit(w, ",\"target_id\":"); JsonString(w, r->step.target_id, sizeof(r->step.target_id));
        Emit(w, ",\"operation\":"); Number(w, (size_t)r->step.operation);
        Emit(w, ",\"state\":\""); Emit(w, StepName(c->states[i])); Emit(w, "\",\"status\":");
        if (c->states[i] == UMI_UI_AUTOMATION_STEP_NOT_RUN) Emit(w, "null");
        else StatusNumber(w, r->status);
        if (includeText) {
            Emit(w, ",\"value\":"); JsonString(w, r->step.value, sizeof(r->step.value));
            if (c->states[i] != UMI_UI_AUTOMATION_STEP_NOT_RUN) {
                Emit(w, ",\"message\":"); JsonString(w, r->message, sizeof(r->message));
                Emit(w, ",\"observed_text\":"); JsonString(w, r->observation.text, sizeof(r->observation.text));
            }
        }
        Emit(w, "}");
    }
    Emit(w, "]}\n");
}

/* Present an ordinary reading view without scripts or external resources. */
static void HtmlReport(ReportWriter *w, const ReportCopy *c, int includeText)
{
    size_t i;
    Emit(w, "<!doctype html><html lang=\"en-GB\"><head><meta charset=\"utf-8\">"
        "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
        "<title>Check session report</title><style>body{font:17px/1.6 system-ui;max-width:1100px;"
        "margin:auto;padding:24px;color:#203440}table{border-collapse:collapse;width:100%}"
        "td,th{border:1px solid #cad6df;padding:10px;text-align:left;overflow-wrap:anywhere}"
        ".table{overflow:auto}pre{white-space:pre-wrap;overflow-wrap:anywhere}</style></head><body>"
        "<h1>Check session report</h1><p>Run: ");
    Escaped(w, c->info.runId, sizeof(c->info.runId), 1);
    Emit(w, "<br>Scenario: "); Escaped(w, c->info.scenarioId, sizeof(c->info.scenarioId), 1);
    Emit(w, "<br>Driver: "); Escaped(w, c->info.driverId, sizeof(c->info.driverId), 1);
    Emit(w, "<br>State: "); Emit(w, StateName(c->info.state));
    Emit(w, "<br>Acceptance: <strong>");
    Emit(w, RunComplete(c) && c->info.failed == 0U ? "Passed" : "Not passed"); Emit(w, "</strong></p>");
    if (includeText) { Emit(w, "<p>"); Escaped(w, c->info.title, sizeof(c->info.title), 1); Emit(w, "</p>"); }
    Emit(w, "<p>Planned: "); Number(w, c->info.planned); Emit(w, "; attempted: "); Number(w, c->info.attempted);
    Emit(w, "; passed: "); Number(w, c->info.passed); Emit(w, "; failed: "); Number(w, c->info.failed);
    Emit(w, "; not run: "); Number(w, c->info.notRun); Emit(w, ".</p>");
    Emit(w, "<p>A requested action needs a separate check of its result. Unrun steps have no outcome.</p>"
        "<div class=\"table\"><table><thead><tr><th>Step</th><th>Target</th><th>Operation</th><th>Result</th></tr>"
        "</thead><tbody>");
    for (i = 0U; i < c->info.planned && w->status == UMI_STATUS_OK; ++i) {
        const UmiUiAutomationStepResult *r = &c->rows[i];
        Emit(w, "<tr><td>"); Escaped(w, r->step.step_id, sizeof(r->step.step_id), 1);
        Emit(w, "</td><td>"); Escaped(w, r->step.target_id, sizeof(r->step.target_id), 1);
        Emit(w, "</td><td>"); Number(w, (size_t)r->step.operation);
        Emit(w, "</td><td>"); Emit(w, StepName(c->states[i]));
        if (c->states[i] != UMI_UI_AUTOMATION_STEP_NOT_RUN) {
            Emit(w, " (status "); StatusNumber(w, r->status); Emit(w, ")");
        }
        if (includeText) {
            Emit(w, "<pre>Value: "); Escaped(w, r->step.value, sizeof(r->step.value), 1);
            Emit(w, "\nMessage: "); Escaped(w, r->message, sizeof(r->message), 1);
            Emit(w, "\nObserved: "); Escaped(w, r->observation.text, sizeof(r->observation.text), 1);
            Emit(w, "</pre>");
        }
        Emit(w, "</td></tr>");
    }
    Emit(w, "</tbody></table></div><p>");
    Emit(w, includeText ? "Text was included by request. Review this report before sharing."
        : "Values, titles, driver messages and observed text were omitted.");
    Emit(w, "</p></body></html>\n");
}

/* The extra completion case keeps interrupted runs from appearing green. */
static void JUnitReport(ReportWriter *w, const ReportCopy *c, int includeText)
{
    size_t i;
    const size_t incomplete = RunComplete(c) ? 0U : 1U;
    Emit(w, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<testsuite name=\"");
    Escaped(w, c->info.scenarioId, sizeof(c->info.scenarioId), 1);
    Emit(w, "\" tests=\""); Number(w, c->info.planned + 1U);
    Emit(w, "\" failures=\""); Number(w, c->info.failed + incomplete);
    Emit(w, "\" errors=\"0\" skipped=\""); Number(w, c->info.notRun); Emit(w, "\">\n");
    Emit(w, "<properties><property name=\"run_id\" value=\"");
    Escaped(w, c->info.runId, sizeof(c->info.runId), 1);
    Emit(w, "\"/><property name=\"driver_id\" value=\""); Escaped(w, c->info.driverId, sizeof(c->info.driverId), 1);
    Emit(w, "\"/><property name=\"state\" value=\""); Emit(w, StateName(c->info.state));
    Emit(w, "\"/></properties>\n");
    for (i = 0U; i < c->info.planned && w->status == UMI_STATUS_OK; ++i) {
        const UmiUiAutomationStepResult *r = &c->rows[i];
        Emit(w, "<testcase classname=\"steps\" name=\"");
        Escaped(w, r->step.step_id, sizeof(r->step.step_id), 1); Emit(w, "\">");
        if (c->states[i] == UMI_UI_AUTOMATION_STEP_NOT_RUN) Emit(w, "<skipped message=\"Not run\"/>");
        else if (c->states[i] == UMI_UI_AUTOMATION_STEP_FAILED) {
            Emit(w, "<failure type=\"StepFailed\" message=\"Check failed\">Status ");
            StatusNumber(w, r->status); Emit(w, "</failure>");
        }
        if (includeText) {
            Emit(w, "<system-out>Value: "); Escaped(w, r->step.value, sizeof(r->step.value), 1);
            Emit(w, "\nMessage: "); Escaped(w, r->message, sizeof(r->message), 1);
            Emit(w, "\nObserved: "); Escaped(w, r->observation.text, sizeof(r->observation.text), 1);
            Emit(w, "</system-out>");
        }
        Emit(w, "</testcase>\n");
    }
    Emit(w, "<testcase classname=\"run\" name=\"run-completion\">");
    if (incomplete) Emit(w, "<failure type=\"IncompleteRun\" message=\"Run did not complete\">"
        "Cancelled, stopped, empty or still running; this is not accepted.</failure>");
    Emit(w, "</testcase>\n</testsuite>\n");
}

/* Validate the request, freeze its input and preserve the first sink error. */
static UmiStatus Export(const UmiUiAutomationReport *report, int includeText,
    UmiUiRecordingWriteFn write, void *context, int format)
{
    ReportCopy *copy = NULL;
    ReportWriter writer = {write, context, UMI_STATUS_OK};
    UmiStatus status;
    if (write == NULL || (includeText != 0 && includeText != 1)) return UMI_STATUS_INVALID_ARGUMENT;
    status = CopyReport(report, &copy);
    if (status != UMI_STATUS_OK) return status;
    if (format == 0) JsonReport(&writer, copy, includeText);
    else if (format == 1) HtmlReport(&writer, copy, includeText);
    else JUnitReport(&writer, copy, includeText);
    free(copy);
    return writer.status;
}
/* Machine-readable report with text disclosure controlled by the caller. */
UmiStatus UmiUiAutomationReportWriteJson(const UmiUiAutomationReport *r,
    int includeText, UmiUiRecordingWriteFn write, void *context)
{ return Export(r, includeText, write, context, 0); }
/* Human-readable report using the same copied state as the JSON export. */
UmiStatus UmiUiAutomationReportWriteHtml(const UmiUiAutomationReport *r,
    int includeText, UmiUiRecordingWriteFn write, void *context)
{ return Export(r, includeText, write, context, 1); }
/* CI report: attempted outcomes, unrun rows and the completion guard. */
UmiStatus UmiUiAutomationReportWriteJUnit(const UmiUiAutomationReport *r,
    int includeText, UmiUiRecordingWriteFn write, void *context)
{ return Export(r, includeText, write, context, 2); }
