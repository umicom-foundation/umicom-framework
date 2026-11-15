/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/codeguard/test_report_contract.c
 *
 * PURPOSE:
 *   Check report grammar, field boundaries, URI locations and observable output failures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/codeguard/report.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"
#include "umicom/language_runtime/json_document.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); return 1; } } while (0)

#ifdef UMICOM_REPORT_FAULTS
static int fail_write, fail_flush, fail_allocate, fail_file, fault_hits;
size_t __real_fwrite(const void *, size_t, size_t, FILE *);
int __real_fflush(FILE *);
void *__real_malloc(size_t);
UmiStatus __real_umi_fs_write_bytes(const char *, const void *, size_t);
size_t __wrap_fwrite(const void *data, size_t size, size_t count, FILE *stream)
{
    if (fail_write) { fail_write = 0; ++fault_hits; return 0U; }
    return __real_fwrite(data, size, count, stream);
}
int __wrap_fflush(FILE *stream)
{
    if (fail_flush) { fail_flush = 0; ++fault_hits; return EOF; }
    return __real_fflush(stream);
}
void *__wrap_malloc(size_t size)
{
    if (fail_allocate) { fail_allocate = 0; ++fault_hits; return NULL; }
    return __real_malloc(size);
}
UmiStatus __wrap_umi_fs_write_bytes(const char *path, const void *bytes, size_t size)
{
    if (fail_file) { fail_file = 0; ++fault_hits; return UMI_STATUS_IO_ERROR; }
    return __real_umi_fs_write_bytes(path, bytes, size);
}
#endif

static UmiCodeGuardFinding sample(void)
{
    UmiCodeGuardFinding finding = {0};
    strcpy(finding.rule_id, "CG.TEST");
    strcpy(finding.cwe, "CWE-20");
    strcpy(finding.path, "src/sample.c");
    strcpy(finding.message, "Check the input.");
    strcpy(finding.remediation, "Validate the boundary.");
    finding.severity = UMI_CODEGUARD_HIGH;
    finding.category = UMI_CODEGUARD_CATEGORY_INPUT;
    finding.confidence = 95U;
    finding.line = 7U; finding.column = 3U;
    return finding;
}

static int string_is(const UmiLanguageRuntimeJsonDocument *doc, int object,
    const char *key, const char *expected)
{
    int token = umi_language_runtime_json_object_get(doc, object, key);
    char decoded[8192];
    CHECK(token >= 0);
    CHECK(umi_language_runtime_json_string(doc, token, decoded, sizeof decoded) == UMI_STATUS_OK);
    CHECK(strcmp(decoded, expected) == 0);
    return 0;
}

static int finding_token(const UmiLanguageRuntimeJsonDocument *doc, int sarif, size_t index)
{
    int array;
    if (sarif) {
        int runs = umi_language_runtime_json_object_get(doc, 0, "runs");
        int run = umi_language_runtime_json_array_at(doc, runs, 0U);
        array = umi_language_runtime_json_object_get(doc, run, "results");
    } else array = umi_language_runtime_json_object_get(doc, 0, "findings");
    return umi_language_runtime_json_array_at(doc, array, index);
}

static int physical_token(const UmiLanguageRuntimeJsonDocument *doc, int finding)
{
    int locations = umi_language_runtime_json_object_get(doc, finding, "locations");
    int location = umi_language_runtime_json_array_at(doc, locations, 0U);
    return umi_language_runtime_json_object_get(doc, location, "physicalLocation");
}

static int file_is(const char *path, const char *expected)
{
    char *text_value = NULL; size_t size = 0U;
    CHECK(umi_fs_read_text(path, &text_value, &size) == UMI_STATUS_OK);
    CHECK(size == strlen(expected) && memcmp(text_value, expected, size) == 0);
    umi_fs_free_text(text_value);
    return 0;
}

static int invalid_output(UmiCodeGuardResult *result, const char *path,
    UmiCodeGuardReportFormat format, const UmiCodeGuardResult *input)
{
    (void)result;
    char *text_value = (char *)(uintptr_t)1U; size_t size = 99U;
    CHECK(umi_codeguard_report_render(format, input, SIZE_MAX - 1U, &text_value, &size)
        == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(text_value == (char *)(uintptr_t)1U && size == 99U);
    FILE *stream = tmpfile(); CHECK(stream != NULL);
    CHECK(fputs("sentinel", stream) >= 0 && fflush(stream) == 0);
    long position = ftell(stream); CHECK(position >= 0);
    CHECK(umi_codeguard_report_write(stream, format, input) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(ftell(stream) == position);
    CHECK(fclose(stream) == 0);
    CHECK(umi_fs_write_text(path, "sentinel") == UMI_STATUS_OK);
    CHECK(umi_codeguard_report_file(path, format, input) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(file_is(path, "sentinel") == 0);
    return 0;
}

int main(int argc, char **argv)
{
    CHECK(argc == 3);
    const char *name = argv[1];
    const char *known[] = { "empty-text", "empty-json", "empty-sarif", "json-fields", "json-controls", "sarif-controls", "sarif-multiple", "sarif-unknown-line", "sarif-unknown-column", "sarif-no-location", "uri-relative", "uri-unicode", "uri-reserved", "text-fields", "maximum-fields", "render-budget", "render-zero-budget", "arguments", "unterminated-rule", "unterminated-cwe", "unterminated-path", "unterminated-message", "unterminated-remediation", "invalid-utf8-rule", "invalid-utf8-cwe", "invalid-utf8-path", "invalid-utf8-message", "invalid-utf8-remediation", "invalid-severity", "invalid-category", "invalid-confidence", "invalid-format-file", "null-result-file", "invalid-finding-file", "unicode-file", "stream-readonly", "existing-stream-error", "stream-equivalence", "write-failure", "flush-failure", "allocation-failure", "file-write-failure" };
    int recognized = 0;
    for (size_t i = 0U; i < sizeof known / sizeof known[0]; ++i)
        if (strcmp(name, known[i]) == 0) recognized = 1;
    CHECK(recognized);
    char destination[UMI_PATH_CAPACITY], leaf[160];
    CHECK(snprintf(leaf, sizeof leaf, "%s-report.txt", name) > 0);
    CHECK(umi_path_join(argv[2], leaf, destination, sizeof destination) == UMI_STATUS_OK);
    UmiCodeGuardResult *result = NULL;
    CHECK(umi_codeguard_result_create(4U, &result) == UMI_STATUS_OK);
    UmiCodeGuardFinding finding = sample();
    UmiCodeGuardReportFormat format = strstr(name, "sarif") != NULL ||
        strncmp(name, "uri-", 4U) == 0 ? UMI_CODEGUARD_REPORT_SARIF : UMI_CODEGUARD_REPORT_JSON;
    if (strcmp(name, "text-fields") == 0 || strcmp(name, "empty-text") == 0)
        format = UMI_CODEGUARD_REPORT_TEXT;
    if (strncmp(name, "unterminated-", 13U) == 0 || strncmp(name, "invalid-utf8-", 13U) == 0) {
        const char *field = name + 13U;
        char *bytes = NULL; size_t capacity = 0U;
        if (strcmp(field, "rule") == 0) { bytes = finding.rule_id; capacity = sizeof finding.rule_id; }
        if (strcmp(field, "cwe") == 0) { bytes = finding.cwe; capacity = sizeof finding.cwe; }
        if (strcmp(field, "path") == 0) { bytes = finding.path; capacity = sizeof finding.path; }
        if (strcmp(field, "message") == 0) { bytes = finding.message; capacity = sizeof finding.message; }
        if (strcmp(field, "remediation") == 0) { bytes = finding.remediation; capacity = sizeof finding.remediation; }
        CHECK(bytes != NULL);
        if (name[0] == 'u') memset(bytes, 'x', capacity);
        else { bytes[0] = (char)0xc0; bytes[1] = (char)0x80; bytes[2] = '\0'; }
        CHECK(umi_codeguard_result_add(result, &finding) == UMI_STATUS_OK);
        for (int f = 0; f < 3; ++f)
            CHECK(invalid_output(result, destination, (UmiCodeGuardReportFormat)f, result) == 0);
        umi_codeguard_result_destroy(result); return 0;
    }
    if (strcmp(name, "invalid-severity") == 0) finding.severity = (UmiCodeGuardSeverity)99;
    if (strcmp(name, "invalid-category") == 0) finding.category = (UmiCodeGuardCategory)99;
    if (strcmp(name, "invalid-confidence") == 0) finding.confidence = 101U;
    if (strcmp(name, "invalid-finding-file") == 0) memset(finding.message, 'x', sizeof finding.message);
    if (strstr(name, "controls") != NULL) {
        for (size_t i = 1U; i <= 31U; ++i) finding.message[i - 1U] = (char)i;
        strcpy(finding.message + 31U, "\"\\ UTF-8: \xc3\xa9 \xf0\x9f\x98\x80");
    }
    if (strcmp(name, "sarif-unknown-line") == 0) finding.line = 0U;
    if (strcmp(name, "sarif-unknown-column") == 0) finding.column = 0U;
    if (strcmp(name, "sarif-no-location") == 0) finding.path[0] = '\0';
    if (strcmp(name, "uri-unicode") == 0) strcpy(finding.path, "src/caf\xc3\xa9.c");
    if (strcmp(name, "uri-reserved") == 0) strcpy(finding.path, "src/a b#c%d?.c");
    if (strcmp(name, "maximum-fields") == 0) {
        memset(finding.rule_id, 'r', sizeof finding.rule_id - 1U);
        memset(finding.cwe, 'c', sizeof finding.cwe - 1U);
        memset(finding.path, '"', sizeof finding.path - 1U);
        memset(finding.message, '\001', sizeof finding.message - 1U);
        memset(finding.remediation, 'f', sizeof finding.remediation - 1U);
        finding.rule_id[sizeof finding.rule_id - 1U] = '\0';
        finding.cwe[sizeof finding.cwe - 1U] = '\0';
        finding.path[sizeof finding.path - 1U] = '\0';
        finding.message[sizeof finding.message - 1U] = '\0';
        finding.remediation[sizeof finding.remediation - 1U] = '\0';
    }
    if (strncmp(name, "empty-", 6U) != 0)
        CHECK(umi_codeguard_result_add(result, &finding) == UMI_STATUS_OK);
    if (strcmp(name, "sarif-multiple") == 0) {
        finding.severity = UMI_CODEGUARD_MEDIUM;
        CHECK(umi_codeguard_result_add(result, &finding) == UMI_STATUS_OK);
        finding.severity = UMI_CODEGUARD_INFO;
        CHECK(umi_codeguard_result_add(result, &finding) == UMI_STATUS_OK);
    }
    if (strncmp(name, "invalid-", 8U) == 0 || strcmp(name, "null-result-file") == 0) {
        CHECK(invalid_output(result, destination,
            strcmp(name, "invalid-format-file") == 0 ? (UmiCodeGuardReportFormat)99 : format,
            strcmp(name, "null-result-file") == 0 ? NULL : result) == 0);
        umi_codeguard_result_destroy(result); return 0;
    }
    if (strcmp(name, "arguments") == 0) {
        char *sentinel = (char *)(uintptr_t)1U; size_t size = 99U;
        CHECK(umi_codeguard_report_render(format, result, 99U, NULL, &size) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_report_render(format, result, 99U, &sentinel, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(sentinel == (char *)(uintptr_t)1U && size == 99U);
        CHECK(umi_codeguard_report_write(NULL, format, result) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_report_file(NULL, format, result) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_report_file("", format, result) == UMI_STATUS_INVALID_ARGUMENT);
        umi_codeguard_report_free(NULL);
        umi_codeguard_result_destroy(result); return 0;
    }
#ifdef UMICOM_REPORT_FAULTS
    if (strstr(name, "-failure") != NULL) {
        FILE *stream = tmpfile(); CHECK(stream != NULL);
        char *sentinel = (char *)(uintptr_t)1U; size_t size = 99U;
        UmiStatus status;
        if (strcmp(name, "allocation-failure") == 0) {
            fail_allocate = 1;
            status = umi_codeguard_report_render(format, result, SIZE_MAX - 1U, &sentinel, &size);
            CHECK(status == UMI_STATUS_OUT_OF_MEMORY);
            CHECK(sentinel == (char *)(uintptr_t)1U && size == 99U);
        } else if (strcmp(name, "file-write-failure") == 0) {
            CHECK(umi_fs_write_text(destination, "sentinel") == UMI_STATUS_OK);
            fail_file = 1; status = umi_codeguard_report_file(destination, format, result);
            CHECK(status == UMI_STATUS_IO_ERROR && file_is(destination, "sentinel") == 0);
        } else {
            fail_write = strcmp(name, "write-failure") == 0;
            fail_flush = strcmp(name, "flush-failure") == 0;
            status = umi_codeguard_report_write(stream, format, result);
            CHECK(status == UMI_STATUS_IO_ERROR);
        }
        CHECK(fault_hits == 1); CHECK(fclose(stream) == 0);
        umi_codeguard_result_destroy(result); return 0;
    }
#endif
    char *output = NULL; size_t output_size = 0U;
    CHECK(umi_codeguard_report_render(format, result, SIZE_MAX - 1U, &output, &output_size) == UMI_STATUS_OK);
    CHECK(output != NULL && output_size == strlen(output));
    if (strcmp(name, "render-budget") == 0 || strcmp(name, "render-zero-budget") == 0) {
        char *sentinel = (char *)(uintptr_t)1U; size_t size = 99U;
        size_t budget = strcmp(name, "render-budget") == 0 ? output_size - 1U : 0U;
        CHECK(umi_codeguard_report_render(format, result, budget, &sentinel, &size) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(sentinel == (char *)(uintptr_t)1U && size == 99U);
        CHECK(umi_codeguard_report_render(format, result, output_size, &sentinel, &size) == UMI_STATUS_OK);
        CHECK(size == output_size && strcmp(sentinel, output) == 0);
        umi_codeguard_report_free(sentinel);
    }
    if (strcmp(name, "unicode-file") == 0) {
        CHECK(umi_path_join(argv[2], "report-caf\xc3\xa9.txt", destination, sizeof destination) == UMI_STATUS_OK);
        CHECK(umi_codeguard_report_file(destination, format, result) == UMI_STATUS_OK);
        CHECK(file_is(destination, output) == 0);
    }
    if (strcmp(name, "stream-readonly") == 0 || strcmp(name, "existing-stream-error") == 0) {
        FILE *stream = fopen(destination, "wb"); CHECK(stream != NULL); CHECK(fclose(stream) == 0);
        stream = fopen(destination, "rb"); CHECK(stream != NULL);
        if (strcmp(name, "existing-stream-error") == 0) {
            CHECK(fputc('x', stream) == EOF && ferror(stream));
        }
        CHECK(umi_codeguard_report_write(stream, format, result) == UMI_STATUS_IO_ERROR);
        (void)fclose(stream);
    }
    if (strcmp(name, "stream-equivalence") == 0) {
        for (int f = 0; f < 3; ++f) {
            char *expected = NULL; size_t size = 0U;
            CHECK(umi_codeguard_report_render((UmiCodeGuardReportFormat)f, result, SIZE_MAX - 1U, &expected, &size) == UMI_STATUS_OK);
            FILE *stream = tmpfile(); CHECK(stream != NULL);
            CHECK(umi_codeguard_report_write(stream, (UmiCodeGuardReportFormat)f, result) == UMI_STATUS_OK);
            CHECK(ftell(stream) == (long)size);
            rewind(stream);
            char *actual = calloc(size + 1U, 1U); CHECK(actual != NULL);
            CHECK(fread(actual, 1U, size, stream) == size && strcmp(actual, expected) == 0);
            CHECK(fclose(stream) == 0); free(actual); umi_codeguard_report_free(expected);
        }
    }
    if (format == UMI_CODEGUARD_REPORT_TEXT) {
        if (strcmp(name, "empty-text") == 0)
            CHECK(strcmp(output, "Umicom CodeGuard: 0 findings (critical=0 high=0 medium=0 low=0 info=0)\n") == 0);
        else {
            CHECK(strstr(output, "CG.TEST src/sample.c:7:3 Check the input. (CWE-20)") != NULL);
            CHECK(strstr(output, "\n  Fix: Validate the boundary.\n") != NULL);
        }
    } else {
        UmiLanguageRuntimeJsonDocument *doc = malloc(sizeof *doc); CHECK(doc != NULL);
        CHECK(UmiLanguageRuntimeJsonParseComplete(output, doc) == UMI_STATUS_OK);
        int sarif_format = format == UMI_CODEGUARD_REPORT_SARIF;
        if (sarif_format) CHECK(string_is(doc, 0, "version", "2.1.0") == 0);
        else CHECK(string_is(doc, 0, "tool", "Umicom CodeGuard") == 0);
        int token = finding_token(doc, sarif_format, 0U);
        if (strncmp(name, "empty-", 6U) == 0) CHECK(token < 0);
        else {
            CHECK(token >= 0);
            CHECK(string_is(doc, token, sarif_format ? "ruleId" : "rule", finding.rule_id) == 0);
            int message = sarif_format ? umi_language_runtime_json_object_get(doc, token, "message") : token;
            CHECK(string_is(doc, message, sarif_format ? "text" : "message", finding.message) == 0);
            if (!sarif_format) CHECK(string_is(doc, token, "path", finding.path) == 0);
            else {
                int physical = physical_token(doc, token);
                if (strcmp(name, "sarif-no-location") == 0) CHECK(physical < 0);
                else {
                    CHECK(physical >= 0);
                    int artifact = umi_language_runtime_json_object_get(doc, physical, "artifactLocation");
                    const char *expected = strcmp(name, "uri-unicode") == 0 ? "src/caf%C3%A9.c" :
                        strcmp(name, "uri-reserved") == 0 ? "src/a%20b%23c%25d%3F.c" : "src/sample.c";
                    CHECK(string_is(doc, artifact, "uri", expected) == 0);
                    int region = umi_language_runtime_json_object_get(doc, physical, "region");
                    if (strcmp(name, "sarif-unknown-line") == 0) CHECK(region < 0);
                    else {
                        int64_t number = 0;
                        CHECK(region >= 0);
                        int line = umi_language_runtime_json_object_get(doc, region, "startLine");
                        CHECK(umi_language_runtime_json_int64(doc, line, &number) == UMI_STATUS_OK && number == 7);
                        int column = umi_language_runtime_json_object_get(doc, region, "startColumn");
                        if (strcmp(name, "sarif-unknown-column") == 0) CHECK(column < 0);
                        else CHECK(umi_language_runtime_json_int64(doc, column, &number) == UMI_STATUS_OK && number == 3);
                    }
                }
            }
            if (strcmp(name, "sarif-multiple") == 0) {
                CHECK(string_is(doc, finding_token(doc, 1, 0U), "level", "error") == 0);
                CHECK(string_is(doc, finding_token(doc, 1, 1U), "level", "warning") == 0);
                CHECK(string_is(doc, finding_token(doc, 1, 2U), "level", "note") == 0);
                CHECK(finding_token(doc, 1, 3U) < 0);
            }
        }
        free(doc);
    }
    umi_codeguard_report_free(output);
    umi_codeguard_result_destroy(result);
    return 0;
}
