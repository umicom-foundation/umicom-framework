/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/sarif.c
 *
 * PURPOSE:
 *   Implement a dependency-free SARIF 2.1.0 interchange boundary for the
 *   Framework Problems model. The writer emits portable result, location,
 *   baseline, suppression, fingerprint and fix evidence. The importer accepts
 *   that portable subset from Umicom and third-party tools.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/sarif.h"

#include <inttypes.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SARIF_INITIAL_CAPACITY 4096U
#define SARIF_MAX_JSON_TOKENS 65536U
#define SARIF_MESSAGE_CAPACITY 2048U

typedef struct SarifBuilder {
    char *bytes;
    size_t count;
    size_t capacity;
    int pretty;
} SarifBuilder;

typedef enum JsonTokenType {
    JSON_TOKEN_UNDEFINED = 0,
    JSON_TOKEN_OBJECT = 1,
    JSON_TOKEN_ARRAY = 2,
    JSON_TOKEN_STRING = 3,
    JSON_TOKEN_PRIMITIVE = 4
} JsonTokenType;

typedef struct JsonToken {
    JsonTokenType type;
    size_t start;
    size_t end;
    size_t child_count;
    int parent;
} JsonToken;

typedef struct JsonParser {
    size_t position;
    size_t next_token;
    int parent;
} JsonParser;

static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

static uint64_t parse_hex_u64(const char *text)
{
    uint64_t value = 0U;
    size_t index;
    if (text == NULL) return 0U;
    for (index = 0U; text[index] != '\0'; ++index) {
        unsigned int digit;
        unsigned char character = (unsigned char)text[index];
        if (character >= (unsigned char)'0' && character <= (unsigned char)'9') {
            digit = (unsigned int)(character - (unsigned char)'0');
        } else if (character >= (unsigned char)'a' &&
                   character <= (unsigned char)'f') {
            digit = 10U + (unsigned int)(character - (unsigned char)'a');
        } else if (character >= (unsigned char)'A' &&
                   character <= (unsigned char)'F') {
            digit = 10U + (unsigned int)(character - (unsigned char)'A');
        } else {
            return 0U;
        }
        if (value > (UINT64_MAX - (uint64_t)digit) / UINT64_C(16)) return 0U;
        value = value * UINT64_C(16) + (uint64_t)digit;
    }
    return value;
}

static UmiStatus builder_reserve(SarifBuilder *builder, size_t additional)
{
    size_t required;
    size_t capacity;
    char *replacement;
    if (builder == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (additional > SIZE_MAX - builder->count - 1U) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    required = builder->count + additional + 1U;
    if (required <= builder->capacity) return UMI_STATUS_OK;
    capacity = builder->capacity > 0U ? builder->capacity : SARIF_INITIAL_CAPACITY;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    replacement = (char *)realloc(builder->bytes, capacity);
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    builder->bytes = replacement;
    builder->capacity = capacity;
    return UMI_STATUS_OK;
}

static UmiStatus builder_append_n(SarifBuilder *builder,
                                  const char *text,
                                  size_t length)
{
    UmiStatus status;
    if (builder == NULL || (text == NULL && length > 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = builder_reserve(builder, length);
    if (status != UMI_STATUS_OK) return status;
    if (length > 0U) (void)memcpy(builder->bytes + builder->count, text, length);
    builder->count += length;
    builder->bytes[builder->count] = '\0';
    return UMI_STATUS_OK;
}

static UmiStatus builder_append(SarifBuilder *builder, const char *text)
{
    return text != NULL
        ? builder_append_n(builder, text, strlen(text))
        : UMI_STATUS_INVALID_ARGUMENT;
}

static UmiStatus builder_append_format(SarifBuilder *builder,
                                       const char *format,
                                       ...)
{
    va_list arguments;
    va_list copy;
    int required;
    UmiStatus status;
    if (builder == NULL || format == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    va_start(arguments, format);
    va_copy(copy, arguments);
    required = vsnprintf(NULL, 0, format, copy);
    va_end(copy);
    if (required < 0) {
        va_end(arguments);
        return UMI_STATUS_INTERNAL_ERROR;
    }
    status = builder_reserve(builder, (size_t)required);
    if (status == UMI_STATUS_OK) {
        int written = vsnprintf(builder->bytes + builder->count,
                                builder->capacity - builder->count,
                                format,
                                arguments);
        if (written != required) status = UMI_STATUS_INTERNAL_ERROR;
        else builder->count += (size_t)written;
    }
    va_end(arguments);
    return status;
}

static UmiStatus builder_indent(SarifBuilder *builder, size_t depth)
{
    size_t index;
    UmiStatus status;
    if (!builder->pretty) return UMI_STATUS_OK;
    status = builder_append(builder, "\n");
    for (index = 0U; index < depth && status == UMI_STATUS_OK; ++index) {
        status = builder_append(builder, "  ");
    }
    return status;
}

static UmiStatus builder_append_json_string(SarifBuilder *builder,
                                            const char *text)
{
    const unsigned char *cursor;
    UmiStatus status = builder_append(builder, "\"");
    if (status != UMI_STATUS_OK) return status;
    cursor = (const unsigned char *)(text != NULL ? text : "");
    while (*cursor != 0U && status == UMI_STATUS_OK) {
        unsigned char character = *cursor++;
        switch (character) {
            case '"': status = builder_append(builder, "\\\""); break;
            case '\\': status = builder_append(builder, "\\\\"); break;
            case '\b': status = builder_append(builder, "\\b"); break;
            case '\f': status = builder_append(builder, "\\f"); break;
            case '\n': status = builder_append(builder, "\\n"); break;
            case '\r': status = builder_append(builder, "\\r"); break;
            case '\t': status = builder_append(builder, "\\t"); break;
            default:
                if (character < 0x20U) {
                    status = builder_append_format(builder, "\\u%04x",
                                                   (unsigned int)character);
                } else {
                    status = builder_append_n(builder,
                                              (const char *)&character,
                                              1U);
                }
                break;
        }
    }
    if (status == UMI_STATUS_OK) status = builder_append(builder, "\"");
    return status;
}

static const char *sarif_level(UmiDiagnosticSeverity severity)
{
    switch (severity) {
        case UMI_DIAGNOSTIC_TRACE:
        case UMI_DIAGNOSTIC_INFO: return "note";
        case UMI_DIAGNOSTIC_WARNING: return "warning";
        case UMI_DIAGNOSTIC_ERROR:
        case UMI_DIAGNOSTIC_FATAL: return "error";
        default: return "none";
    }
}

static const char *baseline_state_text(UmiDiagnosticBaselineState state)
{
    switch (state) {
        case UMI_DIAGNOSTIC_BASELINE_NEW: return "new";
        case UMI_DIAGNOSTIC_BASELINE_UNCHANGED: return "unchanged";
        case UMI_DIAGNOSTIC_BASELINE_UPDATED: return "updated";
        case UMI_DIAGNOSTIC_BASELINE_ABSENT: return "absent";
        default: return NULL;
    }
}

static const char *suppression_kind_text(UmiDiagnosticSuppressionKind kind)
{
    return kind == UMI_DIAGNOSTIC_SUPPRESSION_IN_SOURCE
        ? "inSource" : "external";
}

static const char *suppression_status_text(UmiDiagnosticSuppressionStatus status)
{
    switch (status) {
        case UMI_DIAGNOSTIC_SUPPRESSION_ACCEPTED: return "accepted";
        case UMI_DIAGNOSTIC_SUPPRESSION_UNDER_REVIEW: return "underReview";
        case UMI_DIAGNOSTIC_SUPPRESSION_REJECTED: return "rejected";
        default: return "none";
    }
}

static int problem_is_exported(const UmiDiagnosticProblem *problem,
                               const UmiDiagnosticSarifExportOptions *options)
{
    if (!options->include_suppressed && problem->suppression.suppressed) return 0;
    if (!options->include_resolved && !problem->active) return 0;
    if (!options->include_disabled && problem->policy_disabled) return 0;
    return 1;
}

static UmiStatus append_result_location(SarifBuilder *builder,
                                        const UmiDiagnosticProblem *problem,
                                        size_t depth)
{
    const UmiDiagnosticSnapshot *diagnostic = &problem->finding.diagnostic;
    UmiStatus status = builder_indent(builder, depth);
    if (status == UMI_STATUS_OK) status = builder_append(builder, "\"locations\":[{");
    if (status == UMI_STATUS_OK) status = builder_indent(builder, depth + 1U);
    if (status == UMI_STATUS_OK) status = builder_append(builder, "\"physicalLocation\":{");
    if (status == UMI_STATUS_OK) status = builder_indent(builder, depth + 2U);
    if (status == UMI_STATUS_OK) status = builder_append(builder, "\"artifactLocation\":{\"uri\":");
    if (status == UMI_STATUS_OK) status = builder_append_json_string(builder, diagnostic->uri);
    if (status == UMI_STATUS_OK) status = builder_append(builder, "},");
    if (status == UMI_STATUS_OK) status = builder_indent(builder, depth + 2U);
    if (status == UMI_STATUS_OK) {
        status = builder_append_format(builder,
            "\"region\":{\"startLine\":%" PRIu32
            ",\"startColumn\":%" PRIu32
            ",\"endLine\":%" PRIu32
            ",\"endColumn\":%" PRIu32,
            diagnostic->line > 0U ? diagnostic->line : 1U,
            diagnostic->column > 0U ? diagnostic->column : 1U,
            diagnostic->end_line > 0U ? diagnostic->end_line
                                       : (diagnostic->line > 0U ? diagnostic->line : 1U),
            diagnostic->end_column > 0U ? diagnostic->end_column
                                         : (diagnostic->column > 0U ? diagnostic->column : 1U));
    }
    if (status == UMI_STATUS_OK && problem->finding.snippet[0] != '\0') {
        status = builder_append(builder, ",\"snippet\":{\"text\":");
        if (status == UMI_STATUS_OK) {
            status = builder_append_json_string(builder,
                                                 problem->finding.snippet);
        }
        if (status == UMI_STATUS_OK) status = builder_append(builder, "}");
    }
    if (status == UMI_STATUS_OK) status = builder_append(builder, "}");
    if (status == UMI_STATUS_OK) status = builder_indent(builder, depth + 1U);
    if (status == UMI_STATUS_OK) status = builder_append(builder, "}");
    if (status == UMI_STATUS_OK && problem->finding.logical_location[0] != '\0') {
        status = builder_append(builder, ",\"logicalLocations\":[{\"name\":");
        if (status == UMI_STATUS_OK) {
            status = builder_append_json_string(
                builder, problem->finding.logical_location);
        }
        if (status == UMI_STATUS_OK) status = builder_append(builder, "}]");
    }
    if (status == UMI_STATUS_OK) status = builder_append(builder, "}]");
    return status;
}

static UmiStatus append_result_fix(SarifBuilder *builder,
                                   const UmiDiagnosticProblem *problem,
                                   size_t depth)
{
    const UmiDiagnosticProviderFinding *finding = &problem->finding;
    UmiStatus status = builder_append(builder, ",");
    if (status == UMI_STATUS_OK) status = builder_indent(builder, depth);
    if (status == UMI_STATUS_OK) status = builder_append(builder, "\"fixes\":[{\"description\":{\"text\":");
    if (status == UMI_STATUS_OK) status = builder_append_json_string(builder, finding->fix_description);
    if (status == UMI_STATUS_OK) status = builder_append(builder, "},\"artifactChanges\":[{\"artifactLocation\":{\"uri\":");
    if (status == UMI_STATUS_OK) status = builder_append_json_string(builder, finding->diagnostic.uri);
    if (status == UMI_STATUS_OK) status = builder_append(builder, "},\"replacements\":[{\"deletedRegion\":{");
    if (status == UMI_STATUS_OK) {
        status = builder_append_format(builder,
            "\"startLine\":%" PRIu32 ",\"startColumn\":%" PRIu32
            ",\"endLine\":%" PRIu32 ",\"endColumn\":%" PRIu32 "},",
            finding->replacement_start_line > 0U
                ? finding->replacement_start_line : finding->diagnostic.line,
            finding->replacement_start_column > 0U
                ? finding->replacement_start_column : finding->diagnostic.column,
            finding->replacement_end_line > 0U
                ? finding->replacement_end_line : finding->diagnostic.end_line,
            finding->replacement_end_column > 0U
                ? finding->replacement_end_column : finding->diagnostic.end_column);
    }
    if (status == UMI_STATUS_OK) status = builder_append(builder, "\"insertedContent\":{\"text\":");
    if (status == UMI_STATUS_OK) status = builder_append_json_string(builder, finding->replacement_text);
    if (status == UMI_STATUS_OK) status = builder_append(builder, "}}]}]}]");
    return status;
}

static UmiStatus append_result_suppression(SarifBuilder *builder,
                                           const UmiDiagnosticProblem *problem,
                                           size_t depth)
{
    UmiStatus status = builder_append(builder, ",");
    if (status == UMI_STATUS_OK) status = builder_indent(builder, depth);
    if (status == UMI_STATUS_OK) status = builder_append(builder, "\"suppressions\":[{\"kind\":");
    if (status == UMI_STATUS_OK) {
        status = builder_append_json_string(
            builder, suppression_kind_text(problem->suppression.kind));
    }
    if (status == UMI_STATUS_OK) status = builder_append(builder, ",\"status\":");
    if (status == UMI_STATUS_OK) {
        status = builder_append_json_string(
            builder, suppression_status_text(problem->suppression.status));
    }
    if (status == UMI_STATUS_OK) status = builder_append(builder, ",\"justification\":");
    if (status == UMI_STATUS_OK) {
        status = builder_append_json_string(builder, problem->suppression.reason);
    }
    if (status == UMI_STATUS_OK) status = builder_append(builder, "}]");
    return status;
}

static UmiStatus append_result_properties(SarifBuilder *builder,
                                          const UmiDiagnosticProblem *problem,
                                          size_t depth)
{
    const UmiDiagnosticProviderFinding *finding = &problem->finding;
    const UmiDiagnosticSnapshot *diagnostic = &finding->diagnostic;
    UmiStatus status = builder_append(builder, ",");
    if (status == UMI_STATUS_OK) status = builder_indent(builder, depth);
    if (status == UMI_STATUS_OK) status = builder_append(builder, "\"properties\":{");
#define APPEND_STRING_PROPERTY(name, value)                                    \
    do {                                                                        \
        if (status == UMI_STATUS_OK) status = builder_append(builder, name);    \
        if (status == UMI_STATUS_OK)                                            \
            status = builder_append_json_string(builder, value);                \
    } while (0)
    APPEND_STRING_PROPERTY("\"umicomDiagnosticId\":", diagnostic->id);
    APPEND_STRING_PROPERTY(",\"umicomProvider\":", finding->provider_id);
    APPEND_STRING_PROPERTY(",\"umicomSource\":", diagnostic->source);
    APPEND_STRING_PROPERTY(",\"umicomOwner\":", diagnostic->owner);
    APPEND_STRING_PROPERTY(",\"umicomTags\":", diagnostic->tags);
    APPEND_STRING_PROPERTY(",\"umicomRuleName\":", finding->rule_name);
    APPEND_STRING_PROPERTY(",\"umicomHelpUri\":", finding->help_uri);
    APPEND_STRING_PROPERTY(",\"umicomSeverityRuleId\":",
                           problem->severity_rule_id);
#undef APPEND_STRING_PROPERTY
    if (status == UMI_STATUS_OK) {
        status = builder_append_format(builder,
            ",\"umicomKind\":%d,\"umicomOriginalSeverity\":%d"
            ",\"umicomEffectiveSeverity\":%d,\"umicomRunId\":%" PRIu64
            ",\"umicomSourceRevision\":%" PRIu64
            ",\"umicomTimestampNs\":%" PRIu64
            ",\"umicomActive\":%s,\"umicomPolicyDisabled\":%s}",
            (int)diagnostic->kind,
            (int)problem->original_severity,
            (int)problem->effective_severity,
            finding->run_id,
            finding->source_revision,
            diagnostic->timestamp_ns,
            problem->active ? "true" : "false",
            problem->policy_disabled ? "true" : "false");
    }
    return status;
}

static UmiStatus append_problem_result(SarifBuilder *builder,
                                       const UmiDiagnosticProblem *problem,
                                       const UmiDiagnosticSarifExportOptions *options,
                                       size_t depth)
{
    const UmiDiagnosticProviderFinding *finding = &problem->finding;
    const char *baseline = baseline_state_text(problem->baseline_state);
    UmiStatus status = builder_indent(builder, depth);
    if (status == UMI_STATUS_OK) status = builder_append(builder, "{");
    if (status == UMI_STATUS_OK) status = builder_indent(builder, depth + 1U);
    if (status == UMI_STATUS_OK) status = builder_append(builder, "\"ruleId\":");
    if (status == UMI_STATUS_OK) status = builder_append_json_string(builder,
        finding->diagnostic.code[0] != '\0' ? finding->diagnostic.code : "UMICOM");
    if (status == UMI_STATUS_OK) status = builder_append(builder, ",");
    if (status == UMI_STATUS_OK) status = builder_indent(builder, depth + 1U);
    if (status == UMI_STATUS_OK) status = builder_append(builder, "\"level\":");
    if (status == UMI_STATUS_OK) status = builder_append_json_string(builder,
        sarif_level(problem->effective_severity));
    if (status == UMI_STATUS_OK && baseline != NULL) {
        status = builder_append(builder, ",");
        if (status == UMI_STATUS_OK) status = builder_indent(builder, depth + 1U);
        if (status == UMI_STATUS_OK) status = builder_append(builder, "\"baselineState\":");
        if (status == UMI_STATUS_OK) status = builder_append_json_string(builder, baseline);
    }
    if (status == UMI_STATUS_OK) status = builder_append(builder, ",");
    if (status == UMI_STATUS_OK) status = builder_indent(builder, depth + 1U);
    if (status == UMI_STATUS_OK) status = builder_append(builder, "\"message\":{\"text\":");
    if (status == UMI_STATUS_OK) status = builder_append_json_string(builder,
        finding->diagnostic.message);
    if (status == UMI_STATUS_OK && finding->diagnostic.detail[0] != '\0') {
        status = builder_append(builder, ",\"markdown\":");
        if (status == UMI_STATUS_OK) status = builder_append_json_string(builder,
            finding->diagnostic.detail);
    }
    if (status == UMI_STATUS_OK) status = builder_append(builder, "},");
    if (status == UMI_STATUS_OK) status = append_result_location(builder, problem,
                                                                  depth + 1U);
    if (status == UMI_STATUS_OK) {
        status = builder_append(builder, ",");
        if (status == UMI_STATUS_OK) status = builder_indent(builder, depth + 1U);
        if (status == UMI_STATUS_OK) status = builder_append_format(builder,
            "\"partialFingerprints\":{\"umicomFingerprint/v1\":\"%016" PRIx64
            "\",\"umicomContentFingerprint/v1\":\"%016" PRIx64 "\"}",
            finding->fingerprint,
            finding->content_fingerprint);
    }
    if (status == UMI_STATUS_OK && problem->suppression.suppressed) {
        status = append_result_suppression(builder, problem, depth + 1U);
    }
    if (status == UMI_STATUS_OK && options->include_fixes && problem->fixable &&
        finding->has_fix) {
        status = append_result_fix(builder, problem, depth + 1U);
    }
    if (status == UMI_STATUS_OK) {
        status = append_result_properties(builder, problem, depth + 1U);
    }
    if (status == UMI_STATUS_OK) status = builder_indent(builder, depth);
    if (status == UMI_STATUS_OK) status = builder_append(builder, "}");
    return status;
}

UmiDiagnosticSarifRunDescriptor umi_diagnostic_sarif_run_default(
    const char *tool_name)
{
    UmiDiagnosticSarifRunDescriptor descriptor;
    size_t length;
    (void)memset(&descriptor, 0, sizeof(descriptor));
    descriptor.struct_size = (uint32_t)sizeof(descriptor);
    descriptor.api_version = UMI_DIAGNOSTIC_SARIF_API_VERSION;
    descriptor.successful = 1;
    if (tool_name != NULL) {
        length = strlen(tool_name);
        if (length >= sizeof(descriptor.tool_name)) {
            length = sizeof(descriptor.tool_name) - 1U;
        }
        (void)memcpy(descriptor.tool_name, tool_name, length);
        descriptor.tool_name[length] = '\0';
    }
    return descriptor;
}

UmiDiagnosticSarifExportOptions umi_diagnostic_sarif_export_options_default(void)
{
    UmiDiagnosticSarifExportOptions options;
    (void)memset(&options, 0, sizeof(options));
    options.pretty = 1;
    options.include_suppressed = 1;
    options.include_fixes = 1;
    return options;
}

UmiStatus umi_diagnostic_sarif_export(
    const UmiDiagnosticProblemModel *model,
    const UmiDiagnosticSarifRunDescriptor *run,
    const UmiDiagnosticSarifExportOptions *options,
    char **out_json,
    size_t *out_size)
{
    UmiDiagnosticSarifExportOptions effective;
    SarifBuilder builder;
    size_t position;
    size_t emitted = 0U;
    UmiStatus status;
    if (model == NULL || run == NULL || out_json == NULL ||
        run->struct_size != (uint32_t)sizeof(*run) ||
        run->api_version != UMI_DIAGNOSTIC_SARIF_API_VERSION ||
        !terminated(run->tool_name, sizeof(run->tool_name)) ||
        run->tool_name[0] == '\0' ||
        !terminated(run->tool_version, sizeof(run->tool_version)) ||
        !terminated(run->information_uri, sizeof(run->information_uri)) ||
        !terminated(run->automation_id, sizeof(run->automation_id))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_json = NULL;
    if (out_size != NULL) *out_size = 0U;
    effective = options != NULL ? *options
                                : umi_diagnostic_sarif_export_options_default();
    (void)memset(&builder, 0, sizeof(builder));
    builder.pretty = effective.pretty != 0;
    status = builder_append(&builder,
        "{\"$schema\":\"https://json.schemastore.org/sarif-2.1.0.json\","
        "\"version\":\"2.1.0\",\"runs\":[{");
    if (status == UMI_STATUS_OK) status = builder_indent(&builder, 1U);
    if (status == UMI_STATUS_OK) status = builder_append(&builder,
        "\"tool\":{\"driver\":{\"name\":");
    if (status == UMI_STATUS_OK) status = builder_append_json_string(&builder,
        run->tool_name);
    if (status == UMI_STATUS_OK && run->tool_version[0] != '\0') {
        status = builder_append(&builder, ",\"version\":");
        if (status == UMI_STATUS_OK) status = builder_append_json_string(&builder,
            run->tool_version);
    }
    if (status == UMI_STATUS_OK && run->information_uri[0] != '\0') {
        status = builder_append(&builder, ",\"informationUri\":");
        if (status == UMI_STATUS_OK) status = builder_append_json_string(&builder,
            run->information_uri);
    }
    if (status == UMI_STATUS_OK) status = builder_append(&builder, "}},");
    if (status == UMI_STATUS_OK) status = builder_indent(&builder, 1U);
    if (status == UMI_STATUS_OK) status = builder_append(&builder,
        "\"automationDetails\":{\"id\":");
    if (status == UMI_STATUS_OK) status = builder_append_json_string(&builder,
        run->automation_id[0] != '\0' ? run->automation_id : run->tool_name);
    if (status == UMI_STATUS_OK) status = builder_append(&builder, "},");
    if (status == UMI_STATUS_OK) status = builder_indent(&builder, 1U);
    if (status == UMI_STATUS_OK) status = builder_append(&builder,
        "\"invocations\":[{\"executionSuccessful\":");
    if (status == UMI_STATUS_OK) status = builder_append(&builder,
        run->successful ? "true" : "false");
    if (status == UMI_STATUS_OK) status = builder_append_format(&builder,
        ",\"exitCode\":%" PRId32
        ",\"properties\":{\"umicomRunId\":%" PRIu64
        ",\"umicomStartedTimestampNs\":%" PRIu64
        ",\"umicomEndedTimestampNs\":%" PRIu64 "}}],",
        run->exit_code,
        run->run_id,
        run->started_timestamp_ns,
        run->ended_timestamp_ns);
    if (status == UMI_STATUS_OK) status = builder_indent(&builder, 1U);
    if (status == UMI_STATUS_OK) status = builder_append(&builder, "\"results\":[");
    for (position = 0U;
         status == UMI_STATUS_OK && position < umi_diagnostic_problem_model_count(model);
         ++position) {
        UmiDiagnosticProblem problem;
        status = umi_diagnostic_problem_model_at(model, position, &problem);
        if (status != UMI_STATUS_OK) break;
        if (!problem_is_exported(&problem, &effective)) continue;
        if (effective.maximum_results > 0U && emitted >= effective.maximum_results) {
            break;
        }
        if (emitted > 0U) status = builder_append(&builder, ",");
        if (status == UMI_STATUS_OK) {
            status = append_problem_result(&builder, &problem, &effective, 2U);
        }
        ++emitted;
    }
    if (status == UMI_STATUS_OK) status = builder_indent(&builder, 1U);
    if (status == UMI_STATUS_OK) status = builder_append(&builder, "]");
    if (status == UMI_STATUS_OK) status = builder_indent(&builder, 0U);
    if (status == UMI_STATUS_OK) status = builder_append(&builder, "}]}");
    if (status != UMI_STATUS_OK) {
        free(builder.bytes);
        return status;
    }
    *out_json = builder.bytes;
    if (out_size != NULL) *out_size = builder.count;
    return UMI_STATUS_OK;
}

static JsonToken *json_allocate_token(JsonParser *parser,
                                      JsonToken *tokens,
                                      size_t capacity)
{
    JsonToken *token;
    if (parser->next_token >= capacity) return NULL;
    token = &tokens[parser->next_token++];
    (void)memset(token, 0, sizeof(*token));
    token->parent = -1;
    return token;
}

static UmiStatus json_parse_string(JsonParser *parser,
                                   const char *json,
                                   size_t size,
                                   JsonToken *tokens,
                                   size_t capacity)
{
    size_t start = ++parser->position;
    while (parser->position < size) {
        unsigned char character = (unsigned char)json[parser->position];
        if (character == (unsigned char)'"') {
            JsonToken *token = json_allocate_token(parser, tokens, capacity);
            if (token == NULL) return UMI_STATUS_CAPACITY_EXCEEDED;
            token->type = JSON_TOKEN_STRING;
            token->start = start;
            token->end = parser->position;
            token->parent = parser->parent;
            if (parser->parent >= 0) {
                ++tokens[(size_t)parser->parent].child_count;
            }
            return UMI_STATUS_OK;
        }
        if (character < 0x20U) return UMI_STATUS_PARSE_ERROR;
        if (character == (unsigned char)'\\') {
            size_t escape = ++parser->position;
            if (escape >= size) return UMI_STATUS_PARSE_ERROR;
            character = (unsigned char)json[escape];
            if (character == (unsigned char)'u') {
                size_t digit;
                if (escape + 4U >= size) return UMI_STATUS_PARSE_ERROR;
                for (digit = 1U; digit <= 4U; ++digit) {
                    unsigned char value = (unsigned char)json[escape + digit];
                    if (!((value >= (unsigned char)'0' && value <= (unsigned char)'9') ||
                          (value >= (unsigned char)'a' && value <= (unsigned char)'f') ||
                          (value >= (unsigned char)'A' && value <= (unsigned char)'F'))) {
                        return UMI_STATUS_PARSE_ERROR;
                    }
                }
                parser->position += 4U;
            } else if (strchr("\"\\/bfnrt", (int)character) == NULL) {
                return UMI_STATUS_PARSE_ERROR;
            }
        }
        ++parser->position;
    }
    return UMI_STATUS_PARSE_ERROR;
}

static int primitive_delimiter(unsigned char character)
{
    return character == (unsigned char)',' || character == (unsigned char)']' ||
           character == (unsigned char)'}' || character == (unsigned char)' ' ||
           character == (unsigned char)'\t' || character == (unsigned char)'\r' ||
           character == (unsigned char)'\n';
}

static UmiStatus json_parse_primitive(JsonParser *parser,
                                      const char *json,
                                      size_t size,
                                      JsonToken *tokens,
                                      size_t capacity)
{
    size_t start = parser->position;
    JsonToken *token;
    while (parser->position < size &&
           !primitive_delimiter((unsigned char)json[parser->position])) {
        unsigned char character = (unsigned char)json[parser->position];
        if (character < 0x20U || character >= 0x7fU || character == (unsigned char)':') {
            return UMI_STATUS_PARSE_ERROR;
        }
        ++parser->position;
    }
    if (parser->position == start) return UMI_STATUS_PARSE_ERROR;
    token = json_allocate_token(parser, tokens, capacity);
    if (token == NULL) return UMI_STATUS_CAPACITY_EXCEEDED;
    token->type = JSON_TOKEN_PRIMITIVE;
    token->start = start;
    token->end = parser->position;
    token->parent = parser->parent;
    if (parser->parent >= 0) ++tokens[(size_t)parser->parent].child_count;
    if (parser->position > 0U) --parser->position;
    return UMI_STATUS_OK;
}

static UmiStatus json_parse(const char *json,
                            size_t size,
                            JsonToken *tokens,
                            size_t capacity,
                            size_t *out_count)
{
    JsonParser parser;
    size_t position;
    if (json == NULL || size == 0U || tokens == NULL || capacity == 0U ||
        out_count == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(&parser, 0, sizeof(parser));
    parser.parent = -1;
    for (parser.position = 0U; parser.position < size; ++parser.position) {
        unsigned char character = (unsigned char)json[parser.position];
        UmiStatus status = UMI_STATUS_OK;
        switch (character) {
            case '{':
            case '[': {
                JsonToken *token = json_allocate_token(&parser, tokens, capacity);
                if (token == NULL) return UMI_STATUS_CAPACITY_EXCEEDED;
                token->type = character == (unsigned char)'{'
                    ? JSON_TOKEN_OBJECT : JSON_TOKEN_ARRAY;
                token->start = parser.position;
                token->parent = parser.parent;
                if (parser.parent >= 0) {
                    ++tokens[(size_t)parser.parent].child_count;
                }
                parser.parent = (int)(parser.next_token - 1U);
                break;
            }
            case '}':
            case ']': {
                JsonTokenType expected = character == (unsigned char)'}'
                    ? JSON_TOKEN_OBJECT : JSON_TOKEN_ARRAY;
                int open = parser.parent;
                if (open < 0 || tokens[(size_t)open].type != expected ||
                    tokens[(size_t)open].end != 0U) {
                    return UMI_STATUS_PARSE_ERROR;
                }
                tokens[(size_t)open].end = parser.position + 1U;
                parser.parent = tokens[(size_t)open].parent;
                break;
            }
            case '"':
                status = json_parse_string(&parser, json, size, tokens, capacity);
                if (status != UMI_STATUS_OK) return status;
                break;
            case ' ':
            case '\t':
            case '\r':
            case '\n':
            case ':':
            case ',':
                break;
            default:
                status = json_parse_primitive(&parser, json, size, tokens, capacity);
                if (status != UMI_STATUS_OK) return status;
                break;
        }
    }
    if (parser.parent != -1 || parser.next_token == 0U) {
        return UMI_STATUS_PARSE_ERROR;
    }
    for (position = 0U; position < parser.next_token; ++position) {
        if ((tokens[position].type == JSON_TOKEN_OBJECT ||
             tokens[position].type == JSON_TOKEN_ARRAY) &&
            tokens[position].end == 0U) {
            return UMI_STATUS_PARSE_ERROR;
        }
    }
    *out_count = parser.next_token;
    return UMI_STATUS_OK;
}

static size_t json_token_next(const JsonToken *tokens,
                              size_t count,
                              size_t position)
{
    size_t next = position + 1U;
    if (position >= count) return count;
    while (next < count && tokens[next].start < tokens[position].end) ++next;
    return next;
}

static int json_token_equals(const char *json,
                             const JsonToken *token,
                             const char *text)
{
    size_t length;
    if (json == NULL || token == NULL || text == NULL ||
        token->type != JSON_TOKEN_STRING) return 0;
    length = strlen(text);
    return token->end - token->start == length &&
           memcmp(json + token->start, text, length) == 0;
}

static size_t json_object_get(const char *json,
                              const JsonToken *tokens,
                              size_t count,
                              size_t object,
                              const char *name)
{
    size_t position;
    if (object >= count || tokens[object].type != JSON_TOKEN_OBJECT) return SIZE_MAX;
    position = object + 1U;
    while (position < count && tokens[position].start < tokens[object].end) {
        size_t value = position + 1U;
        if (tokens[position].parent == (int)object && value < count &&
            tokens[value].parent == (int)object &&
            json_token_equals(json, &tokens[position], name)) {
            return value;
        }
        if (value >= count) break;
        position = json_token_next(tokens, count, value);
    }
    return SIZE_MAX;
}

static size_t json_array_item(const JsonToken *tokens,
                              size_t count,
                              size_t array,
                              size_t wanted)
{
    size_t position;
    size_t index = 0U;
    if (array >= count || tokens[array].type != JSON_TOKEN_ARRAY) return SIZE_MAX;
    position = array + 1U;
    while (position < count && tokens[position].start < tokens[array].end) {
        if (tokens[position].parent == (int)array) {
            if (index == wanted) return position;
            ++index;
            position = json_token_next(tokens, count, position);
        } else {
            ++position;
        }
    }
    return SIZE_MAX;
}

static size_t json_array_count(const JsonToken *tokens,
                               size_t count,
                               size_t array)
{
    size_t position;
    size_t result = 0U;
    if (array >= count || tokens[array].type != JSON_TOKEN_ARRAY) return 0U;
    position = array + 1U;
    while (position < count && tokens[position].start < tokens[array].end) {
        if (tokens[position].parent == (int)array) {
            ++result;
            position = json_token_next(tokens, count, position);
        } else {
            ++position;
        }
    }
    return result;
}

static int hex_digit(unsigned char character)
{
    if (character >= (unsigned char)'0' && character <= (unsigned char)'9') {
        return (int)(character - (unsigned char)'0');
    }
    if (character >= (unsigned char)'a' && character <= (unsigned char)'f') {
        return 10 + (int)(character - (unsigned char)'a');
    }
    if (character >= (unsigned char)'A' && character <= (unsigned char)'F') {
        return 10 + (int)(character - (unsigned char)'A');
    }
    return -1;
}

static UmiStatus append_utf8(char *destination,
                             size_t capacity,
                             size_t *position,
                             uint32_t codepoint)
{
    unsigned char bytes[3];
    size_t count;
    if (codepoint <= 0x7fU) {
        bytes[0] = (unsigned char)codepoint;
        count = 1U;
    } else if (codepoint <= 0x7ffU) {
        bytes[0] = (unsigned char)(0xc0U | (codepoint >> 6U));
        bytes[1] = (unsigned char)(0x80U | (codepoint & 0x3fU));
        count = 2U;
    } else {
        bytes[0] = (unsigned char)(0xe0U | (codepoint >> 12U));
        bytes[1] = (unsigned char)(0x80U | ((codepoint >> 6U) & 0x3fU));
        bytes[2] = (unsigned char)(0x80U | (codepoint & 0x3fU));
        count = 3U;
    }
    if (count >= capacity - *position) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination + *position, bytes, count);
    *position += count;
    return UMI_STATUS_OK;
}

static UmiStatus json_copy_string(const char *json,
                                  const JsonToken *token,
                                  char *destination,
                                  size_t capacity)
{
    size_t source;
    size_t output = 0U;
    if (json == NULL || token == NULL || destination == NULL || capacity == 0U ||
        token->type != JSON_TOKEN_STRING) return UMI_STATUS_INVALID_ARGUMENT;
    for (source = token->start; source < token->end; ++source) {
        unsigned char character = (unsigned char)json[source];
        if (character != (unsigned char)'\\') {
            if (output + 1U >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
            destination[output++] = (char)character;
            continue;
        }
        if (++source >= token->end) return UMI_STATUS_PARSE_ERROR;
        character = (unsigned char)json[source];
        if (character == (unsigned char)'u') {
            uint32_t codepoint = 0U;
            size_t digit;
            if (source + 4U >= token->end) return UMI_STATUS_PARSE_ERROR;
            for (digit = 0U; digit < 4U; ++digit) {
                int value = hex_digit((unsigned char)json[++source]);
                if (value < 0) return UMI_STATUS_PARSE_ERROR;
                codepoint = codepoint * 16U + (uint32_t)value;
            }
            if (codepoint >= 0xd800U && codepoint <= 0xdfffU) codepoint = 0xfffdU;
            {
                UmiStatus status = append_utf8(destination, capacity, &output,
                                               codepoint);
                if (status != UMI_STATUS_OK) return status;
            }
        } else {
            char value;
            switch (character) {
                case '"': value = '"'; break;
                case '\\': value = '\\'; break;
                case '/': value = '/'; break;
                case 'b': value = '\b'; break;
                case 'f': value = '\f'; break;
                case 'n': value = '\n'; break;
                case 'r': value = '\r'; break;
                case 't': value = '\t'; break;
                default: return UMI_STATUS_PARSE_ERROR;
            }
            if (output + 1U >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
            destination[output++] = value;
        }
    }
    destination[output] = '\0';
    return UMI_STATUS_OK;
}

static int json_copy_optional_string(const char *json,
                                     const JsonToken *tokens,
                                     size_t count,
                                     size_t object,
                                     const char *name,
                                     char *destination,
                                     size_t capacity)
{
    size_t value = json_object_get(json, tokens, count, object, name);
    if (destination == NULL || capacity == 0U) return 0;
    destination[0] = '\0';
    if (value == SIZE_MAX || tokens[value].type != JSON_TOKEN_STRING) return 0;
    return json_copy_string(json, &tokens[value], destination, capacity) ==
           UMI_STATUS_OK;
}

static int json_primitive_equals(const char *json,
                                 const JsonToken *token,
                                 const char *text)
{
    size_t length;
    if (json == NULL || token == NULL || text == NULL ||
        token->type != JSON_TOKEN_PRIMITIVE) return 0;
    length = strlen(text);
    return token->end - token->start == length &&
           memcmp(json + token->start, text, length) == 0;
}

static uint64_t json_u64(const char *json,
                         const JsonToken *token,
                         uint64_t fallback)
{
    uint64_t value = 0U;
    size_t position;
    if (json == NULL || token == NULL || token->type != JSON_TOKEN_PRIMITIVE ||
        token->start >= token->end) return fallback;
    for (position = token->start; position < token->end; ++position) {
        unsigned char character = (unsigned char)json[position];
        unsigned int digit;
        if (character < (unsigned char)'0' || character > (unsigned char)'9') {
            return fallback;
        }
        digit = (unsigned int)(character - (unsigned char)'0');
        if (value > (UINT64_MAX - (uint64_t)digit) / UINT64_C(10)) {
            return fallback;
        }
        value = value * UINT64_C(10) + (uint64_t)digit;
    }
    return value;
}

static int json_int(const char *json, const JsonToken *token, int fallback)
{
    int sign = 1;
    uint64_t value = 0U;
    size_t position;
    if (json == NULL || token == NULL || token->type != JSON_TOKEN_PRIMITIVE ||
        token->start >= token->end) return fallback;
    position = token->start;
    if (json[position] == '-') {
        sign = -1;
        if (++position >= token->end) return fallback;
    }
    for (; position < token->end; ++position) {
        unsigned char character = (unsigned char)json[position];
        unsigned int digit;
        if (character < (unsigned char)'0' || character > (unsigned char)'9') {
            return fallback;
        }
        digit = (unsigned int)(character - (unsigned char)'0');
        if (value > (uint64_t)INT_MAX) return fallback;
        value = value * UINT64_C(10) + (uint64_t)digit;
    }
    if (value > (uint64_t)INT_MAX) return fallback;
    return sign * (int)value;
}

static int json_bool(const char *json, const JsonToken *token, int fallback)
{
    if (json_primitive_equals(json, token, "true")) return 1;
    if (json_primitive_equals(json, token, "false")) return 0;
    return fallback;
}

static UmiStatus tokenize_json(const char *json,
                               size_t size,
                               JsonToken **out_tokens,
                               size_t *out_count)
{
    size_t capacity = 256U;
    JsonToken *tokens = NULL;
    UmiStatus status;
    if (out_tokens == NULL || out_count == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_tokens = NULL;
    *out_count = 0U;
    while (capacity <= SARIF_MAX_JSON_TOKENS) {
        JsonToken *replacement = (JsonToken *)realloc(tokens,
            capacity * sizeof(*tokens));
        if (replacement == NULL) {
            free(tokens);
            return UMI_STATUS_OUT_OF_MEMORY;
        }
        tokens = replacement;
        (void)memset(tokens, 0, capacity * sizeof(*tokens));
        status = json_parse(json, size, tokens, capacity, out_count);
        if (status == UMI_STATUS_OK) {
            *out_tokens = tokens;
            return UMI_STATUS_OK;
        }
        if (status != UMI_STATUS_CAPACITY_EXCEEDED) {
            free(tokens);
            return status;
        }
        if (capacity > SARIF_MAX_JSON_TOKENS / 2U) break;
        capacity *= 2U;
    }
    free(tokens);
    return UMI_STATUS_CAPACITY_EXCEEDED;
}

UmiStatus umi_diagnostic_sarif_validate(
    const char *json,
    size_t json_size,
    char *out_message,
    size_t message_capacity)
{
    JsonToken *tokens = NULL;
    size_t count = 0U;
    size_t version;
    size_t runs;
    UmiStatus status;
    const char *message = "valid SARIF 2.1.0 document";
    if (json == NULL || json_size == 0U) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "SARIF JSON is empty";
    } else {
        status = tokenize_json(json, json_size, &tokens, &count);
        if (status == UMI_STATUS_OK &&
            (count == 0U || tokens[0].type != JSON_TOKEN_OBJECT)) {
            status = UMI_STATUS_PARSE_ERROR;
            message = "SARIF root must be an object";
        }
        version = status == UMI_STATUS_OK
            ? json_object_get(json, tokens, count, 0U, "version") : SIZE_MAX;
        if (status == UMI_STATUS_OK &&
            (version == SIZE_MAX ||
             !json_token_equals(json, &tokens[version], "2.1.0"))) {
            status = UMI_STATUS_PARSE_ERROR;
            message = "SARIF version must be 2.1.0";
        }
        runs = status == UMI_STATUS_OK
            ? json_object_get(json, tokens, count, 0U, "runs") : SIZE_MAX;
        if (status == UMI_STATUS_OK &&
            (runs == SIZE_MAX || tokens[runs].type != JSON_TOKEN_ARRAY)) {
            status = UMI_STATUS_PARSE_ERROR;
            message = "SARIF runs must be an array";
        }
    }
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity, "%s", message);
    }
    free(tokens);
    return status;
}

static UmiDiagnosticSeverity severity_from_level(const char *level)
{
    if (level == NULL) return UMI_DIAGNOSTIC_WARNING;
    if (strcmp(level, "error") == 0) return UMI_DIAGNOSTIC_ERROR;
    if (strcmp(level, "warning") == 0) return UMI_DIAGNOSTIC_WARNING;
    if (strcmp(level, "note") == 0) return UMI_DIAGNOSTIC_INFO;
    return UMI_DIAGNOSTIC_INFO;
}

static UmiDiagnosticProviderBaselineHint baseline_hint_from_text(const char *text)
{
    if (text == NULL) return UMI_DIAGNOSTIC_PROVIDER_BASELINE_NONE;
    if (strcmp(text, "new") == 0) return UMI_DIAGNOSTIC_PROVIDER_BASELINE_NEW;
    if (strcmp(text, "unchanged") == 0) {
        return UMI_DIAGNOSTIC_PROVIDER_BASELINE_UNCHANGED;
    }
    if (strcmp(text, "updated") == 0) {
        return UMI_DIAGNOSTIC_PROVIDER_BASELINE_UPDATED;
    }
    if (strcmp(text, "absent") == 0) {
        return UMI_DIAGNOSTIC_PROVIDER_BASELINE_ABSENT;
    }
    return UMI_DIAGNOSTIC_PROVIDER_BASELINE_NONE;
}

static void import_location(const char *json,
                            const JsonToken *tokens,
                            size_t count,
                            size_t result,
                            UmiDiagnosticProviderFinding *finding)
{
    size_t locations = json_object_get(json, tokens, count, result, "locations");
    size_t location = json_array_item(tokens, count, locations, 0U);
    size_t physical = json_object_get(json, tokens, count, location,
                                      "physicalLocation");
    size_t artifact = json_object_get(json, tokens, count, physical,
                                      "artifactLocation");
    size_t region = json_object_get(json, tokens, count, physical, "region");
    size_t logicals = json_object_get(json, tokens, count, location,
                                      "logicalLocations");
    size_t logical = json_array_item(tokens, count, logicals, 0U);
    size_t value;
    (void)json_copy_optional_string(json, tokens, count, artifact, "uri",
                                    finding->diagnostic.uri,
                                    sizeof(finding->diagnostic.uri));
    value = json_object_get(json, tokens, count, region, "startLine");
    if (value != SIZE_MAX) finding->diagnostic.line =
        (uint32_t)json_u64(json, &tokens[value], 0U);
    value = json_object_get(json, tokens, count, region, "startColumn");
    if (value != SIZE_MAX) finding->diagnostic.column =
        (uint32_t)json_u64(json, &tokens[value], 0U);
    value = json_object_get(json, tokens, count, region, "endLine");
    if (value != SIZE_MAX) finding->diagnostic.end_line =
        (uint32_t)json_u64(json, &tokens[value], 0U);
    value = json_object_get(json, tokens, count, region, "endColumn");
    if (value != SIZE_MAX) finding->diagnostic.end_column =
        (uint32_t)json_u64(json, &tokens[value], 0U);
    {
        size_t snippet = json_object_get(json, tokens, count, region, "snippet");
        (void)json_copy_optional_string(json, tokens, count, snippet, "text",
                                        finding->snippet,
                                        sizeof(finding->snippet));
    }
    (void)json_copy_optional_string(json, tokens, count, logical, "name",
                                    finding->logical_location,
                                    sizeof(finding->logical_location));
}

static void import_properties(const char *json,
                              const JsonToken *tokens,
                              size_t count,
                              size_t result,
                              UmiDiagnosticProviderFinding *finding)
{
    size_t properties = json_object_get(json, tokens, count, result, "properties");
    size_t value;
    (void)json_copy_optional_string(json, tokens, count, properties,
                                    "umicomDiagnosticId",
                                    finding->diagnostic.id,
                                    sizeof(finding->diagnostic.id));
    (void)json_copy_optional_string(json, tokens, count, properties,
                                    "umicomSource", finding->diagnostic.source,
                                    sizeof(finding->diagnostic.source));
    (void)json_copy_optional_string(json, tokens, count, properties,
                                    "umicomOwner", finding->diagnostic.owner,
                                    sizeof(finding->diagnostic.owner));
    (void)json_copy_optional_string(json, tokens, count, properties,
                                    "umicomTags", finding->diagnostic.tags,
                                    sizeof(finding->diagnostic.tags));
    (void)json_copy_optional_string(json, tokens, count, properties,
                                    "umicomRuleName", finding->rule_name,
                                    sizeof(finding->rule_name));
    (void)json_copy_optional_string(json, tokens, count, properties,
                                    "umicomHelpUri", finding->help_uri,
                                    sizeof(finding->help_uri));
    value = json_object_get(json, tokens, count, properties, "umicomKind");
    if (value != SIZE_MAX) {
        int kind = json_int(json, &tokens[value], (int)UMI_DIAGNOSTIC_KIND_GENERAL);
        if (kind >= (int)UMI_DIAGNOSTIC_KIND_GENERAL &&
            kind <= (int)UMI_DIAGNOSTIC_KIND_SECURITY) {
            finding->diagnostic.kind = (UmiDiagnosticKind)kind;
        }
    }
    value = json_object_get(json, tokens, count, properties, "umicomRunId");
    if (value != SIZE_MAX) finding->run_id = json_u64(json, &tokens[value], 0U);
    value = json_object_get(json, tokens, count, properties,
                            "umicomSourceRevision");
    if (value != SIZE_MAX) {
        finding->source_revision = json_u64(json, &tokens[value], 0U);
    }
    value = json_object_get(json, tokens, count, properties,
                            "umicomTimestampNs");
    if (value != SIZE_MAX) {
        finding->diagnostic.timestamp_ns = json_u64(json, &tokens[value], 0U);
    }
    value = json_object_get(json, tokens, count, properties, "umicomActive");
    if (value != SIZE_MAX) {
        finding->diagnostic.resolved = !json_bool(json, &tokens[value], 1);
    }
}

static void import_suppression(const char *json,
                               const JsonToken *tokens,
                               size_t count,
                               size_t result,
                               UmiDiagnosticProviderFinding *finding)
{
    char kind[32];
    char status_text[32];
    size_t suppressions = json_object_get(json, tokens, count, result,
                                          "suppressions");
    size_t suppression = json_array_item(tokens, count, suppressions, 0U);
    if (suppression == SIZE_MAX) return;
    kind[0] = '\0';
    status_text[0] = '\0';
    (void)json_copy_optional_string(json, tokens, count, suppression, "kind",
                                    kind, sizeof(kind));
    (void)json_copy_optional_string(json, tokens, count, suppression, "status",
                                    status_text, sizeof(status_text));
    (void)json_copy_optional_string(json, tokens, count, suppression,
                                    "justification",
                                    finding->suppression_justification,
                                    sizeof(finding->suppression_justification));
    finding->suppressed = 1;
    finding->suppression_kind = strcmp(kind, "inSource") == 0
        ? UMI_DIAGNOSTIC_PROVIDER_SUPPRESSION_IN_SOURCE
        : UMI_DIAGNOSTIC_PROVIDER_SUPPRESSION_EXTERNAL;
    if (strcmp(status_text, "underReview") == 0) {
        finding->suppression_status =
            UMI_DIAGNOSTIC_PROVIDER_SUPPRESSION_UNDER_REVIEW;
    } else if (strcmp(status_text, "rejected") == 0) {
        finding->suppression_status =
            UMI_DIAGNOSTIC_PROVIDER_SUPPRESSION_REJECTED;
    } else {
        finding->suppression_status = UMI_DIAGNOSTIC_PROVIDER_SUPPRESSION_ACCEPTED;
    }
}

static void import_fingerprints(const char *json,
                                const JsonToken *tokens,
                                size_t count,
                                size_t result,
                                UmiDiagnosticProviderFinding *finding)
{
    char text[32];
    size_t fingerprints = json_object_get(json, tokens, count, result,
                                          "partialFingerprints");
    text[0] = '\0';
    if (json_copy_optional_string(json, tokens, count, fingerprints,
                                  "umicomFingerprint/v1",
                                  text, sizeof(text))) {
        finding->fingerprint = parse_hex_u64(text);
    }
    text[0] = '\0';
    if (json_copy_optional_string(json, tokens, count, fingerprints,
                                  "umicomContentFingerprint/v1",
                                  text, sizeof(text))) {
        finding->content_fingerprint = parse_hex_u64(text);
    }
}

static void import_fix(const char *json,
                       const JsonToken *tokens,
                       size_t count,
                       size_t result,
                       UmiDiagnosticProviderFinding *finding)
{
    size_t fixes = json_object_get(json, tokens, count, result, "fixes");
    size_t fix = json_array_item(tokens, count, fixes, 0U);
    size_t description = json_object_get(json, tokens, count, fix, "description");
    size_t changes = json_object_get(json, tokens, count, fix, "artifactChanges");
    size_t change = json_array_item(tokens, count, changes, 0U);
    size_t replacements = json_object_get(json, tokens, count, change,
                                          "replacements");
    size_t replacement = json_array_item(tokens, count, replacements, 0U);
    size_t deleted = json_object_get(json, tokens, count, replacement,
                                     "deletedRegion");
    size_t inserted = json_object_get(json, tokens, count, replacement,
                                      "insertedContent");
    size_t value;
    if (fix == SIZE_MAX) return;
    (void)json_copy_optional_string(json, tokens, count, description, "text",
                                    finding->fix_description,
                                    sizeof(finding->fix_description));
    (void)json_copy_optional_string(json, tokens, count, inserted, "text",
                                    finding->replacement_text,
                                    sizeof(finding->replacement_text));
    value = json_object_get(json, tokens, count, deleted, "startLine");
    if (value != SIZE_MAX) finding->replacement_start_line =
        (uint32_t)json_u64(json, &tokens[value], 0U);
    value = json_object_get(json, tokens, count, deleted, "startColumn");
    if (value != SIZE_MAX) finding->replacement_start_column =
        (uint32_t)json_u64(json, &tokens[value], 0U);
    value = json_object_get(json, tokens, count, deleted, "endLine");
    if (value != SIZE_MAX) finding->replacement_end_line =
        (uint32_t)json_u64(json, &tokens[value], 0U);
    value = json_object_get(json, tokens, count, deleted, "endColumn");
    if (value != SIZE_MAX) finding->replacement_end_column =
        (uint32_t)json_u64(json, &tokens[value], 0U);
    finding->has_fix = finding->fix_description[0] != '\0';
}

static UmiStatus import_result(const char *json,
                               const JsonToken *tokens,
                               size_t count,
                               size_t result,
                               const char *provider_id,
                               uint64_t default_run_id,
                               size_t result_index,
                               UmiDiagnosticProviderBatch *batch,
                               UmiDiagnosticSarifImportSummary *summary)
{
    UmiDiagnosticSnapshot diagnostic;
    UmiDiagnosticProviderFinding finding;
    char rule_id[UMI_DIAGNOSTIC_CODE_CAPACITY];
    char level[32];
    char baseline[32];
    char message[SARIF_MESSAGE_CAPACITY];
    char detail[UMI_DIAGNOSTIC_DETAIL_CAPACITY];
    char identifier[UMI_DIAGNOSTIC_ID_CAPACITY];
    size_t message_object;
    UmiStatus status;
    (void)memset(&diagnostic, 0, sizeof(diagnostic));
    rule_id[0] = '\0'; level[0] = '\0'; baseline[0] = '\0';
    message[0] = '\0'; detail[0] = '\0'; identifier[0] = '\0';
    (void)json_copy_optional_string(json, tokens, count, result, "ruleId",
                                    rule_id, sizeof(rule_id));
    (void)json_copy_optional_string(json, tokens, count, result, "level",
                                    level, sizeof(level));
    (void)json_copy_optional_string(json, tokens, count, result, "baselineState",
                                    baseline, sizeof(baseline));
    message_object = json_object_get(json, tokens, count, result, "message");
    (void)json_copy_optional_string(json, tokens, count, message_object, "text",
                                    message, sizeof(message));
    (void)json_copy_optional_string(json, tokens, count, message_object,
                                    "markdown", detail, sizeof(detail));
    if (message[0] == '\0') return UMI_STATUS_PARSE_ERROR;
    (void)snprintf(identifier, sizeof(identifier), "sarif-%" PRIu64 "-%zu",
                   default_run_id, result_index + 1U);
    status = umi_diagnostic_snapshot_init(&diagnostic,
                                          identifier,
                                          severity_from_level(level),
                                          UMI_DIAGNOSTIC_KIND_GENERAL,
                                          provider_id,
                                          message);
    if (status != UMI_STATUS_OK) return status;
    if (rule_id[0] != '\0') (void)snprintf(diagnostic.code,
                                           sizeof(diagnostic.code), "%s",
                                           rule_id);
    if (detail[0] != '\0') (void)snprintf(diagnostic.detail,
                                          sizeof(diagnostic.detail), "%s",
                                          detail);
    status = umi_diagnostic_provider_finding_init(&finding, provider_id,
                                                   &diagnostic);
    if (status != UMI_STATUS_OK) return status;
    import_properties(json, tokens, count, result, &finding);
    import_location(json, tokens, count, result, &finding);
    import_suppression(json, tokens, count, result, &finding);
    import_fingerprints(json, tokens, count, result, &finding);
    import_fix(json, tokens, count, result, &finding);
    finding.baseline_hint = baseline_hint_from_text(baseline);
    if (finding.run_id == 0U) finding.run_id = default_run_id;
    if (finding.diagnostic.id[0] == '\0') {
        (void)snprintf(finding.diagnostic.id, sizeof(finding.diagnostic.id),
                       "%s", identifier);
    }
    if (finding.fingerprint == 0U) {
        finding.fingerprint = umi_diagnostic_provider_fingerprint(&finding);
    }
    if (finding.content_fingerprint == 0U) {
        finding.content_fingerprint =
            umi_diagnostic_provider_content_fingerprint(&finding);
    }
    status = umi_diagnostic_provider_batch_upsert(batch, &finding);
    if (status == UMI_STATUS_OK) {
        ++summary->result_count;
        if (finding.suppressed) ++summary->suppressed_count;
        if (finding.has_fix) ++summary->fix_count;
    }
    return status;
}

UmiStatus umi_diagnostic_sarif_import(
    const char *json,
    size_t json_size,
    const char *provider_id,
    UmiDiagnosticProviderDescriptor *out_provider,
    UmiDiagnosticProviderBatch *out_batch,
    UmiDiagnosticSarifImportSummary *out_summary)
{
    JsonToken *tokens = NULL;
    size_t token_count = 0U;
    size_t runs;
    size_t run_count;
    size_t run_index;
    UmiDiagnosticSarifImportSummary summary;
    UmiStatus status;
    if (json == NULL || json_size == 0U || provider_id == NULL ||
        provider_id[0] == '\0' || out_batch == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_diagnostic_sarif_validate(json, json_size, NULL, 0U);
    if (status != UMI_STATUS_OK) return status;
    status = tokenize_json(json, json_size, &tokens, &token_count);
    if (status != UMI_STATUS_OK) return status;
    (void)memset(&summary, 0, sizeof(summary));
    summary.struct_size = (uint32_t)sizeof(summary);
    summary.api_version = UMI_DIAGNOSTIC_SARIF_API_VERSION;
    status = umi_diagnostic_provider_batch_clear(out_batch);
    if (status != UMI_STATUS_OK) {
        free(tokens);
        return status;
    }
    if (out_provider != NULL) {
        (void)memset(out_provider, 0, sizeof(*out_provider));
        out_provider->struct_size = (uint32_t)sizeof(*out_provider);
        out_provider->api_version = UMI_DIAGNOSTIC_PROVIDER_API_VERSION;
        (void)snprintf(out_provider->id, sizeof(out_provider->id), "%s",
                       provider_id);
        (void)snprintf(out_provider->label, sizeof(out_provider->label), "%s",
                       provider_id);
        out_provider->category = UMI_DIAGNOSTIC_PROVIDER_OTHER;
        out_provider->default_kind = UMI_DIAGNOSTIC_KIND_GENERAL;
        out_provider->enabled = 1;
        out_provider->supports_fixes = 1;
        out_provider->supports_sarif = 1;
    }
    runs = json_object_get(json, tokens, token_count, 0U, "runs");
    run_count = json_array_count(tokens, token_count, runs);
    summary.run_count = run_count;
    for (run_index = 0U; run_index < run_count; ++run_index) {
        size_t run = json_array_item(tokens, token_count, runs, run_index);
        size_t tool = json_object_get(json, tokens, token_count, run, "tool");
        size_t driver = json_object_get(json, tokens, token_count, tool, "driver");
        size_t invocations = json_object_get(json, tokens, token_count, run,
                                             "invocations");
        size_t invocation = json_array_item(tokens, token_count, invocations, 0U);
        size_t properties = json_object_get(json, tokens, token_count, invocation,
                                            "properties");
        size_t run_value = json_object_get(json, tokens, token_count, properties,
                                           "umicomRunId");
        size_t results = json_object_get(json, tokens, token_count, run, "results");
        size_t result_count = json_array_count(tokens, token_count, results);
        size_t result_index;
        uint64_t run_id = run_value != SIZE_MAX
            ? json_u64(json, &tokens[run_value], (uint64_t)(run_index + 1U))
            : (uint64_t)(run_index + 1U);
        if (run_id > summary.run_id) summary.run_id = run_id;
        if (out_provider != NULL && run_index == 0U) {
            (void)json_copy_optional_string(json, tokens, token_count, driver,
                                            "name", out_provider->tool_name,
                                            sizeof(out_provider->tool_name));
            (void)json_copy_optional_string(json, tokens, token_count, driver,
                                            "version", out_provider->tool_version,
                                            sizeof(out_provider->tool_version));
            (void)json_copy_optional_string(json, tokens, token_count, driver,
                                            "informationUri",
                                            out_provider->information_uri,
                                            sizeof(out_provider->information_uri));
            if (out_provider->tool_name[0] != '\0') {
                (void)snprintf(out_provider->label, sizeof(out_provider->label),
                               "%s", out_provider->tool_name);
            }
        }
        for (result_index = 0U; result_index < result_count; ++result_index) {
            size_t result = json_array_item(tokens, token_count, results,
                                            result_index);
            status = import_result(json, tokens, token_count, result, provider_id,
                                   run_id, result_index, out_batch, &summary);
            if (status != UMI_STATUS_OK) {
                ++summary.skipped_count;
                status = UMI_STATUS_OK;
            }
        }
    }
    free(tokens);
    if (out_summary != NULL) *out_summary = summary;
    return status;
}

void umi_diagnostic_sarif_free(void *memory)
{
    free(memory);
}
