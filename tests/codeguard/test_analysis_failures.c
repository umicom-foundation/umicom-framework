/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/codeguard/test_analysis_failures.c
 *
 * PURPOSE:
 *   Verify transactional source measurements and architecture evidence failure propagation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/codeguard/architecture.h"
#include "umicom/codeguard/source.h"
#include "../../src/codeguard/source_reader.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)

static const char *source_text = "#include <sample.h>\nvoid sample(void) {}\n";
static unsigned open_count, close_count, reads_in_stream, insertions;
static unsigned fail_open, fail_read, fail_close, fail_insert;
static UmiStatus storage_failure = UMI_STATUS_OUT_OF_MEMORY;
static FILE *OpenInput(const char *path, const char *mode)
{
    (void)path;
    (void)mode;
    ++open_count;
    reads_in_stream = 0U;
    if (open_count == fail_open) return NULL;
    FILE *file = tmpfile();
    if (file == NULL) return NULL;
    size_t bytes = strlen(source_text);
    if (fwrite(source_text, 1U, bytes, file) != bytes || fflush(file) != 0) {
        (void)fclose(file);
        return NULL;
    }
    rewind(file);
    return file;
}
static char *ReadInput(char *buffer, int capacity, FILE *file)
{
    ++reads_in_stream;
    if (open_count == fail_read && reads_in_stream > 1U) return NULL;
    return fgets(buffer, capacity, file);
}
static int InputError(FILE *file)
{
    return (open_count == fail_read && reads_in_stream != 0U) || ferror(file);
}
static int CloseInput(FILE *file)
{
    ++close_count;
    int status = fclose(file);
    return open_count == fail_close ? EOF : status;
}
static UmiStatus StoreFinding(UmiCodeGuardResult *result, const UmiCodeGuardFinding *item)
{
    ++insertions;
    if (insertions == fail_insert) return storage_failure;
    return umi_codeguard_result_add(result, item);
}


/* The architecture path now uses native chunk input. Feed the same controlled
 * streams through that boundary so all existing late-failure scenarios remain
 * exercised. Whole-file measurement's legacy stdio cases remain unchanged. */
static UmiStatus NativeAnalysisInput(const char *path, uint64_t maximum,
    UmiInputChunkVisitor visitor, void *context, uint64_t *out_size)
{
    *out_size = 0U;
    FILE *file = OpenInput(path, "rb");
    if (file == NULL) return UMI_STATUS_IO_ERROR;
    UmiStatus status = strlen(source_text) > maximum
        ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
    char buffer[8192];
    size_t bytes = 0U;
    while (status == UMI_STATUS_OK && ReadInput(buffer, sizeof buffer, file) != NULL) {
        if (InputError(file)) { status = UMI_STATUS_IO_ERROR; break; }
        size_t size = strlen(buffer);
        status = visitor((const unsigned char *)buffer, size, context);
        bytes += size;
    }
    if (InputError(file) && status == UMI_STATUS_OK) status = UMI_STATUS_IO_ERROR;
    if (CloseInput(file) != 0 && status == UMI_STATUS_OK) status = UMI_STATUS_IO_ERROR;
    if (status == UMI_STATUS_OK) *out_size = bytes;
    return status;
}

#define UmiInputFileVisit NativeAnalysisInput
#include "../../src/codeguard/source_reader.c"
#undef UmiInputFileVisit

/* Real measurement and rule logic run against disposable streams. Hooks
 * control read/close/insertion outcomes; no host permission trick or sleep
 * is needed to produce a deterministic partial-input failure. */
#define fopen OpenInput
#define fgets ReadInput
#define ferror InputError
#define fclose CloseInput
#define umi_codeguard_result_add StoreFinding
#include "../../src/codeguard/source.c"
#include "../../src/codeguard/architecture.c"
#undef umi_codeguard_result_add
#undef fclose
#undef ferror
#undef fgets
#undef fopen

static int MeasureCase(const char *name)
{
    UmiCodeGuardSourceInfo info;
    memset(&info, 0xa5, sizeof info);
    unsigned char prior[sizeof info];
    memcpy(prior, &info, sizeof prior);
    if (!strcmp(name, "measure-arguments")) {
        CHECK(umi_codeguard_source_measure(NULL, &info) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_source_measure("input.c", NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(open_count == 0U && memcmp(prior, &info, sizeof prior) == 0);
        return 0;
    }
    char path[1025];
    const char *input = "input.c";
    if (!strcmp(name, "measure-longpath") || !strcmp(name, "measure-boundary")) {
        memset(path, 'x', sizeof path);
        path[!strcmp(name, "measure-boundary") ? 1023U : 1024U] = '\0';
        input = path;
    }
    if (!strcmp(name, "measure-open")) fail_open = 1U;
    if (!strcmp(name, "measure-read")) fail_read = 1U;
    if (!strcmp(name, "measure-close")) fail_close = 1U;
    if (!strcmp(name, "measure-alias")) {
        memcpy(info.path, "input.c", sizeof "input.c");
        input = info.path;
    }
    if (!strcmp(name, "measure-empty")) source_text = "";
    UmiStatus status = umi_codeguard_source_measure(input, &info);
    if (!strcmp(name, "measure-longpath")) {
        CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED && open_count == 0U);
        CHECK(memcmp(prior, &info, sizeof prior) == 0);
    } else if (fail_open || fail_read || fail_close) {
        CHECK(status == UMI_STATUS_IO_ERROR && memcmp(prior, &info, sizeof prior) == 0);
        CHECK(open_count == 1U && close_count == (fail_open ? 0U : 1U));
    } else {
        CHECK(status == UMI_STATUS_OK && open_count == 1U && close_count == 1U);
        CHECK(info.bytes == strlen(source_text));
        CHECK(info.lines == (!strcmp(name, "measure-empty") ? 0U : 2U));
        CHECK(info.include_count == (!strcmp(name, "measure-empty") ? 0U : 1U));
        CHECK(info.function_hint_count == (!strcmp(name, "measure-empty") ? 0U : 1U));
        CHECK(!strcmp(info.path, !strcmp(name, "measure-boundary") ? path : "input.c"));
    }
    return 0;
}

static int ArchitectureCase(const char *name)
{
    UmiCodeGuardProfile profile = umi_codeguard_profile_default();
    profile.max_include_count = SIZE_MAX;
    profile.max_source_lines = SIZE_MAX;
    UmiCodeGuardResult *result = NULL;
    CHECK(umi_codeguard_result_create(8U, &result) == UMI_STATUS_OK);
    if (!strcmp(name, "architecture-arguments")) {
        CHECK(umi_codeguard_architecture_scan_file(NULL, NULL, &profile, result) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_architecture_scan_file(NULL, "input.c", NULL, result) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_architecture_scan_file(NULL, "input.c", &profile, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(open_count == 0U);
        umi_codeguard_result_destroy(result);
        return 0;
    }
    const char *path = "framework/include/widget.h";
    source_text = "#include <gtk/gtk.h>\nGtkWidget *window;\n";
    if (!strncmp(name, "metrics-", 8U)) {
        path = "input.c";
        if (strcmp(name, "metrics-lines") != 0) profile.max_include_count = 0U;
        if (strcmp(name, "metrics-includes") != 0) profile.max_source_lines = 0U;
        if (!strcmp(name, "metrics-second-failure")) fail_insert = 2U;
        if (!strcmp(name, "metrics-first-failure")) fail_insert = 1U;
        if (!strcmp(name, "metrics-capacity")) { fail_insert = 1U; storage_failure = UMI_STATUS_CAPACITY_EXCEEDED; }
    } else if (!strncmp(name, "studio-", 7U)) {
        path = "applications/studio/view.c";
        source_text = "#include \"sqlite3.h\"\n#include \"framework/src/private.h\"\n";
        if (!strcmp(name, "studio-first-failure")) fail_insert = 1U;
        if (!strcmp(name, "studio-second-failure")) fail_insert = 2U;
    } else if (!strcmp(name, "measurement-read")) fail_read = 1U;
    else if (!strcmp(name, "measurement-close")) fail_close = 1U;
    else if (!strcmp(name, "architecture-open")) fail_open = 2U;
    else if (!strcmp(name, "architecture-read")) fail_read = 2U;
    else if (!strcmp(name, "architecture-close")) fail_close = 2U;
    else if (!strcmp(name, "header-first-failure")) fail_insert = 1U;
    else if (!strcmp(name, "header-second-failure")) fail_insert = 2U;
    else if (strcmp(name, "header-success")) return 2;

    UmiStatus status = umi_codeguard_architecture_scan_file(NULL, path, &profile, result);
    if (fail_insert) {
        CHECK(status == storage_failure && insertions == fail_insert);
        CHECK(umi_codeguard_result_count(result) == fail_insert - 1U);
    } else if (fail_open || fail_read || fail_close) {
        CHECK(status == UMI_STATUS_IO_ERROR);
        if (fail_read == 1U || fail_close == 1U || fail_open == 2U)
            CHECK(umi_codeguard_result_count(result) == 0U && insertions == 0U);
    } else {
        CHECK(status == UMI_STATUS_OK);
        size_t expected = (!strcmp(name, "metrics-includes") || !strcmp(name, "metrics-lines")) ? 1U : 2U;
        CHECK(umi_codeguard_result_count(result) == expected);
        const char *first_id = !strcmp(name, "metrics-includes") ? "CODEGUARD-ARCH-COUPLING-002" :
            !strcmp(name, "metrics-lines") ? "CODEGUARD-ARCH-GOD-001" :
            !strncmp(name, "studio-", 7U) ? "CODEGUARD-ARCH-DATA-002" : "CODEGUARD-ARCH-UI-002";
        CHECK(!strcmp(umi_codeguard_result_at(result, 0U)->rule_id, first_id));
        if (!strncmp(name, "studio-", 7U))
            CHECK(!strcmp(umi_codeguard_result_at(result, 1U)->rule_id, "CODEGUARD-ARCH-PRIVATE-002"));
        if (!strcmp(name, "header-success"))
            CHECK(umi_codeguard_result_at(result, 1U)->line == 2U);
    }
    CHECK(close_count == open_count - (fail_open ? 1U : 0U));
    umi_codeguard_result_destroy(result);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    return !strncmp(argv[1], "measure-", 8U) ? MeasureCase(argv[1]) : ArchitectureCase(argv[1]);
}
