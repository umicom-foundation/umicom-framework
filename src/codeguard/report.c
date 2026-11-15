/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/report.c
 *
 * PURPOSE:
 *   Render CodeGuard findings as text, JSON or SARIF for local use and CI
 *   systems.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */

#include "umicom/codeguard/report.h"
#include "umicom/codeguard/category.h"
#include "umicom/codeguard/severity.h"
#include "umicom/codeguard/summary.h"
#include <stdio.h>
#include <string.h>
/* The shared JSON writer, bounded finding validation and checked sinks replace
 * the original formatting routines. Keep the former implementation for owner
 * review; the public stream and file interfaces remain available. */
#if 0
/* Provide the jsons operation used by this module and its client applications. */
static void jsons(FILE *s,const char *v){fputc('"',s);/* Visit each bounded item once so every record receives the same rule. */ for(;v!=NULL&&*v!='\0';++v){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(*v=='"'||*v=='\\')fputc('\\',s);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(*v=='\n'){fputs("\\n",s);continue;}fputc(*v,s);}fputc('"',s);}
/* Provide the text operation used by this module and its client applications. */
static UmiStatus text(FILE *s,const UmiCodeGuardResult *r){UmiCodeGuardSummary m=umi_codeguard_summary_build(r);fprintf(s,"Umicom CodeGuard: %zu findings (critical=%zu high=%zu medium=%zu low=%zu info=%zu)\n",m.total,m.critical,m.high,m.medium,m.low,m.info);/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<umi_codeguard_result_count(r);++i){const UmiCodeGuardFinding *f=umi_codeguard_result_at(r,i);fprintf(s,"[%s] %s %s:%zu:%zu %s",umi_codeguard_severity_text(f->severity),f->rule_id,f->path,f->line,f->column,f->message);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(f->cwe[0]!='\0')fprintf(s," (%s)",f->cwe);fprintf(s,"\n  Fix: %s\n",f->remediation);}return UMI_STATUS_OK;}
/* Provide the json operation used by this module and its client applications. */
static UmiStatus json(FILE *s,const UmiCodeGuardResult *r){fputs("{\"tool\":\"Umicom CodeGuard\",\"findings\":[",s);/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<umi_codeguard_result_count(r);++i){const UmiCodeGuardFinding *f=umi_codeguard_result_at(r,i);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(i>0U)fputc(',',s);fputs("{\"rule\":",s);jsons(s,f->rule_id);fputs(",\"severity\":",s);jsons(s,umi_codeguard_severity_text(f->severity));fputs(",\"category\":",s);jsons(s,umi_codeguard_category_text(f->category));fputs(",\"path\":",s);jsons(s,f->path);fprintf(s,",\"line\":%zu,\"confidence\":%u,\"message\":",f->line,f->confidence);jsons(s,f->message);fputs("}",s);}fputs("]}\n",s);return UMI_STATUS_OK;}
/* Provide the sarif operation used by this module and its client applications. */
static UmiStatus sarif(FILE *s,const UmiCodeGuardResult *r){fputs("{\"version\":\"2.1.0\",\"runs\":[{\"tool\":{\"driver\":{\"name\":\"Umicom CodeGuard\"}},\"results\":[",s);/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<umi_codeguard_result_count(r);++i){const UmiCodeGuardFinding *f=umi_codeguard_result_at(r,i);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(i>0U)fputc(',',s);fputs("{\"ruleId\":",s);jsons(s,f->rule_id);fputs(",\"level\":",s);jsons(s,f->severity>=UMI_CODEGUARD_HIGH?"error":(f->severity==UMI_CODEGUARD_MEDIUM?"warning":"note"));fputs(",\"message\":{\"text\":",s);jsons(s,f->message);fputs("},\"locations\":[{\"physicalLocation\":{\"artifactLocation\":{\"uri\":",s);jsons(s,f->path);fprintf(s,"},\"region\":{\"startLine\":%zu}}}]} ",f->line);fputs("}",s);}fputs("]}]}\n",s);return UMI_STATUS_OK;}
/*
 * Write codeguard report in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_codeguard_report_write(FILE *s,UmiCodeGuardReportFormat f,const UmiCodeGuardResult *r){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||r==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Select the behaviour associated with the requested command or state value. */ switch(f){case UMI_CODEGUARD_REPORT_TEXT:return text(s,r);case UMI_CODEGUARD_REPORT_JSON:return json(s,r);case UMI_CODEGUARD_REPORT_SARIF:return sarif(s,r);default:return UMI_STATUS_INVALID_ARGUMENT;}}
/*
 * Provide the codeguard report file operation used by this module and its client
 * applications.
 */
UmiStatus umi_codeguard_report_file(const char *p,UmiCodeGuardReportFormat f,const UmiCodeGuardResult *r){FILE *s;UmiStatus st;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=fopen(p,"wb");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_IO_ERROR;st=umi_codeguard_report_write(s,f,r);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(fclose(s)!=0&&st==UMI_STATUS_OK)st=UMI_STATUS_IO_ERROR;return st;}
#endif

#include "umicom/base/utf8.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/output_file.h"
#include "umicom/platform/path_uri.h"
#include "umicom/platform/path.h"
#include "umicom/protocol/json_writer.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/* A sink counts the same byte sequence that it later writes. A memory sink
 * therefore allocates exactly once, while a FILE sink can stream a large
 * result without holding a second copy. Failures are sticky. */
typedef struct ReportSink {
    FILE *stream;
    /* The native export uses bounded staging and shared exclusive file ownership.
     * Other sinks leave these fields zero and keep their established behavior. */
    UmiOutputFile *native_file;
    char *native_staging;
    size_t native_count, native_capacity;
    char *buffer;
    size_t length, capacity;
    UmiStatus status;
} ReportSink;

/* Keep syscall count bounded by report bytes rather than serializer tokens.
 * Once a write fails, neither later tokens nor final flushing retry that data. */
static void report_native_flush(ReportSink *sink)
{
    if (sink->status != UMI_STATUS_OK || sink->native_count == 0U) return;
    sink->status = UmiOutputFileWrite(sink->native_file, sink->native_staging,
        sink->native_count);
    if (sink->status == UMI_STATUS_OK) sink->native_count = 0U;
}

/* Add buffered native export through the shared exclusive file owner; preserve the prior FILE/memory-only sink for review. The previous implementation is retained for engineering review. */
#if 0
static void report_bytes(ReportSink *sink, const char *bytes, size_t count)
{
    if (sink->status != UMI_STATUS_OK) return;
    if (count > SIZE_MAX - 1U - sink->length) {
        sink->status = UMI_STATUS_CAPACITY_EXCEEDED;
        return;
    }
    if (sink->stream != NULL) {
        if (fwrite(bytes, 1U, count, sink->stream) != count) {
            sink->status = UMI_STATUS_IO_ERROR;
            return;
        }
    } else if (sink->buffer != NULL) {
        if (count > sink->capacity - sink->length) {
            sink->status = UMI_STATUS_CAPACITY_EXCEEDED;
            return;
        }
        memcpy(sink->buffer + sink->length, bytes, count);
    }
    sink->length += count;
}
#endif
static void report_bytes(ReportSink *sink, const char *bytes, size_t count)
{
    if (sink->status != UMI_STATUS_OK) return;
    if (count > SIZE_MAX - 1U - sink->length) {
        sink->status = UMI_STATUS_CAPACITY_EXCEEDED;
        return;
    }
    if (sink->stream != NULL) {
        if (fwrite(bytes, 1U, count, sink->stream) != count) {
            sink->status = UMI_STATUS_IO_ERROR;
            return;
        }
    } else if (sink->native_file != NULL) {
        size_t remaining = count;
        const char *cursor = bytes;
        while (remaining != 0U && sink->status == UMI_STATUS_OK) {
            size_t available = sink->native_capacity - sink->native_count;
            size_t chunk = remaining < available ? remaining : available;
            memcpy(sink->native_staging + sink->native_count, cursor, chunk);
            sink->native_count += chunk;
            cursor += chunk;
            remaining -= chunk;
            if (sink->native_count == sink->native_capacity) report_native_flush(sink);
        }
        if (sink->status != UMI_STATUS_OK) return;
    } else if (sink->buffer != NULL) {
        if (count > sink->capacity - sink->length) {
            sink->status = UMI_STATUS_CAPACITY_EXCEEDED;
            return;
        }
        memcpy(sink->buffer + sink->length, bytes, count);
    }
    sink->length += count;
}

static void report_literal(ReportSink *sink, const char *text_value)
{
    report_bytes(sink, text_value, strlen(text_value));
}

static void report_number(ReportSink *sink, size_t value)
{
    char number[sizeof(size_t) * 3U + 1U];
    int count = snprintf(number, sizeof number, "%zu", value);
    if (count < 0 || (size_t)count >= sizeof number) {
        if (sink->status == UMI_STATUS_OK) sink->status = UMI_STATUS_IO_ERROR;
        return;
    }
    report_bytes(sink, number, (size_t)count);
}

static void report_json_string(ReportSink *sink, const char *value)
{
    if (sink->status != UMI_STATUS_OK) return;
    /* Finding text is bounded by PATH_CAPACITY. Percent-encoded URI output
     * contains no JSON quote/control bytes and also fits this buffer. */
    char encoded[UMI_CODEGUARD_PATH_CAPACITY * 6U + 64U];
    UmiJsonWriter writer;
    UmiJsonWriterInit(&writer, encoded, sizeof encoded);
    UmiStatus status = UmiJsonWriterString(&writer, value);
    if (status != UMI_STATUS_OK) { sink->status = status; return; }
    report_bytes(sink, encoded, writer.length);
}

static bool report_field_valid(const char *text_value, size_t capacity)
{
    const char *end = memchr(text_value, '\0', capacity);
    return end != NULL && UmiUtf8Validate((const unsigned char *)text_value,
        (size_t)(end - text_value), NULL) != 0;
}

static UmiStatus report_validate(UmiCodeGuardReportFormat format,
    const UmiCodeGuardResult *result)
{
    if (result == NULL || (format != UMI_CODEGUARD_REPORT_TEXT &&
        format != UMI_CODEGUARD_REPORT_JSON && format != UMI_CODEGUARD_REPORT_SARIF))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < umi_codeguard_result_count(result); ++index) {
        const UmiCodeGuardFinding *finding = umi_codeguard_result_at(result, index);
        if (finding == NULL ||
            !report_field_valid(finding->rule_id, sizeof finding->rule_id) ||
            !report_field_valid(finding->cwe, sizeof finding->cwe) ||
            !report_field_valid(finding->path, sizeof finding->path) ||
            !report_field_valid(finding->message, sizeof finding->message) ||
            !report_field_valid(finding->remediation, sizeof finding->remediation) ||
            (unsigned)finding->severity > (unsigned)UMI_CODEGUARD_CRITICAL ||
            (unsigned)finding->category > (unsigned)UMI_CODEGUARD_CATEGORY_QUALITY ||
            finding->confidence > 100U)
            return UMI_STATUS_INVALID_ARGUMENT;
        if (format == UMI_CODEGUARD_REPORT_SARIF && finding->path[0] != '\0') {
            char uri[UMI_PATH_CAPACITY * 3U + 9U];
            UmiStatus status = UmiPathUriReference(finding->path, uri, sizeof uri);
            if (status != UMI_STATUS_OK) return status;
        }
    }
    return UMI_STATUS_OK;
}

static void report_text(ReportSink *sink, const UmiCodeGuardResult *result)
{
    UmiCodeGuardSummary summary = umi_codeguard_summary_build(result);
    report_literal(sink, "Umicom CodeGuard: "); report_number(sink, summary.total);
    report_literal(sink, " findings (critical="); report_number(sink, summary.critical);
    report_literal(sink, " high="); report_number(sink, summary.high);
    report_literal(sink, " medium="); report_number(sink, summary.medium);
    report_literal(sink, " low="); report_number(sink, summary.low);
    report_literal(sink, " info="); report_number(sink, summary.info);
    report_literal(sink, ")\n");
    for (size_t index = 0U; index < umi_codeguard_result_count(result); ++index) {
        const UmiCodeGuardFinding *finding = umi_codeguard_result_at(result, index);
        report_literal(sink, "["); report_literal(sink, umi_codeguard_severity_text(finding->severity));
        report_literal(sink, "] "); report_literal(sink, finding->rule_id);
        report_literal(sink, " "); report_literal(sink, finding->path);
        report_literal(sink, ":"); report_number(sink, finding->line);
        report_literal(sink, ":"); report_number(sink, finding->column);
        report_literal(sink, " "); report_literal(sink, finding->message);
        if (finding->cwe[0] != '\0') {
            report_literal(sink, " ("); report_literal(sink, finding->cwe);
            report_literal(sink, ")");
        }
        report_literal(sink, "\n  Fix: "); report_literal(sink, finding->remediation);
        report_literal(sink, "\n");
    }
}

static void report_json(ReportSink *sink, const UmiCodeGuardResult *result)
{
    report_literal(sink, "{\"tool\":\"Umicom CodeGuard\",\"findings\":[");
    for (size_t index = 0U; index < umi_codeguard_result_count(result); ++index) {
        const UmiCodeGuardFinding *finding = umi_codeguard_result_at(result, index);
        if (index != 0U) report_literal(sink, ",");
        report_literal(sink, "{\"rule\":"); report_json_string(sink, finding->rule_id);
        report_literal(sink, ",\"severity\":"); report_json_string(sink, umi_codeguard_severity_text(finding->severity));
        report_literal(sink, ",\"category\":"); report_json_string(sink, umi_codeguard_category_text(finding->category));
        report_literal(sink, ",\"path\":"); report_json_string(sink, finding->path);
        report_literal(sink, ",\"line\":"); report_number(sink, finding->line);
        report_literal(sink, ",\"confidence\":"); report_number(sink, finding->confidence);
        report_literal(sink, ",\"message\":"); report_json_string(sink, finding->message);
        report_literal(sink, "}");
    }
    report_literal(sink, "]}\n");
}

static void report_sarif(ReportSink *sink, const UmiCodeGuardResult *result)
{
    report_literal(sink, "{\"version\":\"2.1.0\",\"runs\":[{\"tool\":{\"driver\":{\"name\":\"Umicom CodeGuard\"}},\"results\":[");
    for (size_t index = 0U; index < umi_codeguard_result_count(result); ++index) {
        const UmiCodeGuardFinding *finding = umi_codeguard_result_at(result, index);
        if (index != 0U) report_literal(sink, ",");
        report_literal(sink, "{\"ruleId\":"); report_json_string(sink, finding->rule_id);
        report_literal(sink, ",\"level\":"); report_json_string(sink,
            finding->severity >= UMI_CODEGUARD_HIGH ? "error" :
            finding->severity == UMI_CODEGUARD_MEDIUM ? "warning" : "note");
        report_literal(sink, ",\"message\":{\"text\":");
        report_json_string(sink, finding->message); report_literal(sink, "}");
        /* Missing locations and unknown line/column values are omitted instead
         * of manufacturing a file or emitting a zero-based SARIF region. */
        if (finding->path[0] != '\0') {
            char uri[UMI_PATH_CAPACITY * 3U + 9U];
            UmiStatus status = UmiPathUriReference(finding->path, uri, sizeof uri);
            if (status != UMI_STATUS_OK) { sink->status = status; return; }
            report_literal(sink, ",\"locations\":[{\"physicalLocation\":{\"artifactLocation\":{\"uri\":");
            report_json_string(sink, uri); report_literal(sink, "}");
            if (finding->line != 0U) {
                report_literal(sink, ",\"region\":{\"startLine\":"); report_number(sink, finding->line);
                if (finding->column != 0U) {
                    report_literal(sink, ",\"startColumn\":"); report_number(sink, finding->column);
                }
                report_literal(sink, "}");
            }
            report_literal(sink, "}}]");
        }
        report_literal(sink, "}");
    }
    report_literal(sink, "]}]}\n");
}

static void report_emit(ReportSink *sink, UmiCodeGuardReportFormat format,
    const UmiCodeGuardResult *result)
{
    switch (format) {
        case UMI_CODEGUARD_REPORT_TEXT: report_text(sink, result); break;
        case UMI_CODEGUARD_REPORT_JSON: report_json(sink, result); break;
        case UMI_CODEGUARD_REPORT_SARIF: report_sarif(sink, result); break;
        default: sink->status = UMI_STATUS_INVALID_ARGUMENT; break;
    }
}

UmiStatus umi_codeguard_report_write(FILE *stream, UmiCodeGuardReportFormat format,
    const UmiCodeGuardResult *result)
{
    if (stream == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = report_validate(format, result);
    if (status != UMI_STATUS_OK) return status;
    if (ferror(stream)) return UMI_STATUS_IO_ERROR;
    ReportSink sink = {.stream = stream, .status = UMI_STATUS_OK};
    report_emit(&sink, format, result);
    if (sink.status != UMI_STATUS_OK) return sink.status;
    /* A successful fwrite may only have filled the C stream buffer. Flush
     * while the caller can still receive an error; never close their stream. */
    if (fflush(stream) != 0 || ferror(stream)) return UMI_STATUS_IO_ERROR;
    return UMI_STATUS_OK;
}

UmiStatus umi_codeguard_report_render(UmiCodeGuardReportFormat format,
    const UmiCodeGuardResult *result, size_t max_bytes, char **out_text, size_t *out_size)
{
    if (out_text == NULL || out_size == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = report_validate(format, result);
    if (status != UMI_STATUS_OK) return status;
    ReportSink measure = {.status = UMI_STATUS_OK};
    report_emit(&measure, format, result);
    if (measure.status != UMI_STATUS_OK) return measure.status;
    if (measure.length > max_bytes) return UMI_STATUS_CAPACITY_EXCEEDED;
    char *candidate = malloc(measure.length + 1U);
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    ReportSink sink = {.buffer = candidate, .capacity = measure.length,
        .status = UMI_STATUS_OK};
    report_emit(&sink, format, result);
    if (sink.status != UMI_STATUS_OK || sink.length != measure.length) {
        free(candidate);
        return sink.status != UMI_STATUS_OK ? sink.status : UMI_STATUS_INVALID_STATE;
    }
    candidate[sink.length] = '\0';
    *out_text = candidate;
    *out_size = sink.length;
    return UMI_STATUS_OK;
}

void umi_codeguard_report_free(char *text_value)
{
    free(text_value);
}

UmiStatus umi_codeguard_report_file(const char *path, UmiCodeGuardReportFormat format,
    const UmiCodeGuardResult *result)
{
    if (path == NULL || path[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    char *text_value = NULL;
    size_t size = 0U;
    UmiStatus status = umi_codeguard_report_render(format, result, SIZE_MAX - 1U,
        &text_value, &size);
    if (status != UMI_STATUS_OK) return status;
    /* Validate and render before a destination is opened. Platform owns UTF-8
     * path conversion and checked write/close. This retains overwrite behavior;
     * a storage error can still leave a partial destination. */
    status = umi_fs_write_bytes(path, text_value, size);
    umi_codeguard_report_free(text_value);
    return status;
}

UmiStatus umi_codeguard_report_file_new(const char *path,
    UmiCodeGuardReportFormat format, const UmiCodeGuardResult *result, size_t max_bytes)
{
    UmiStatus status = UmiOutputFileValidatePath(path);
    if (status != UMI_STATUS_OK) return status;
    status = report_validate(format, result);
    if (status != UMI_STATUS_OK) return status;
    ReportSink measure = {.status = UMI_STATUS_OK};
    report_emit(&measure, format, result);
    if (measure.status != UMI_STATUS_OK) return measure.status;
    if (measure.length > max_bytes) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiOutputFile *file = NULL;
    status = UmiOutputFileCreate(path, &file);
    if (status != UMI_STATUS_OK) return status;
    char staging[8192];
    ReportSink sink = {.native_file = file, .native_staging = staging,
        .native_capacity = sizeof staging, .status = UMI_STATUS_OK};
    report_emit(&sink, format, result);
    report_native_flush(&sink);
    status = sink.status;
    if (status == UMI_STATUS_OK && sink.length != measure.length)
        status = UMI_STATUS_INVALID_STATE;
    /* Cleanup always runs, but a later flush/close failure cannot replace the
     * original write failure. Destroy retains the new file, including a prefix. */
    UmiStatus closed = UmiOutputFileClose(file);
    if (status == UMI_STATUS_OK) status = closed;
    UmiOutputFileDestroy(file);
    return status;
}
