/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/scaffold/application_catalogue.c
 * PURPOSE: Build a bounded application inventory from registered paths and manifest snapshots.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/scaffold/application_catalogue.h"
#include "umicom/repository/git_command.h"
#include "umicom/repository/git_capture.h"
#include "umicom/platform/input_file.h"
#include "umicom/platform/output_file.h"
#include "umicom/platform/filesystem.h"
#include <stdlib.h>
#include <string.h>

struct UmiApplicationCatalogue {
    UmiApplicationCatalogueSummary summary;
    UmiApplicationCatalogueEntry entries[UMI_APPLICATION_CATALOGUE_LIMIT];
};

/* Portable identities compare case-insensitively, including on a case-sensitive
 * host, so the same catalogue cannot become ambiguous when moved to Windows. */
static int CatalogueEqual(const char *left, const char *right)
{
    while (*left != '\0' && *right != '\0') {
        unsigned char a = (unsigned char)*left++, b = (unsigned char)*right++;
        if (a >= 'A' && a <= 'Z') a = (unsigned char)(a + ('a' - 'A'));
        if (b >= 'A' && b <= 'Z') b = (unsigned char)(b + ('a' - 'A'));
        if (a != b) return 0;
    }
    return *left == *right;
}

/* Restrict machine output to the portable suite layout. Git interprets quoted
 * configuration values first; unusual subsection names are rejected rather than
 * treating an embedded separator as another registration. */
/* The space-delimited key grammar is retained for review. Git's NUL-delimited
 * output separates key and value with a newline, so a subsection name may
 * contain spaces without becoming another registration. */
#if 0
static int CatalogueKey(const char *key, size_t length)
{
    if (length < 16U || strncmp(key, "submodule.", 10U) != 0 ||
        memcmp(key + length - 5U, ".path", 5U) != 0) return 0;
    for (size_t i = 10U; i < length - 5U; ++i) {
        unsigned char c = (unsigned char)key[i];
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || c == '_' || c == '-' || c == '.' || c == '/')) return 0;
    }
    return 1;
}
#endif
static int CatalogueBinaryKey(const unsigned char *key, size_t length)
{
    if (length < 16U || memcmp(key, "submodule.", 10U) != 0 ||
        memcmp(key + length - 5U, ".path", 5U) != 0) return 0;
    for (size_t i = 10U; i < length - 5U; ++i)
        if (key[i] < 32U || key[i] == 127U) return 0;
    return 1;
}
/* Preserve the earlier local grammar for review. Shared validation also
 * rejects Windows device names, matching the native source creator. */
#if 0
static int CatalogueSlug(const char *slug)
{
    size_t length = strlen(slug);
    if (length == 0U || length >= 64U ||
        slug[0] < 'a' || slug[0] > 'z' || slug[length - 1U] == '-') return 0;
    for (size_t i = 1U; i < length; ++i)
        if (!((slug[i] >= 'a' && slug[i] <= 'z') ||
            (slug[i] >= '0' && slug[i] <= '9') || slug[i] == '-')) return 0;
    return 1;
}

#endif
static int CatalogueSlug(const char *slug)
{
    return umi_application_module_slug_validate(slug) == UMI_STATUS_OK;
}

/* Compare all advertised executable names, not just each module's primary name.
 * A native executable colliding with another module's console is equally unsafe. */
/* A reason accompanies each collision so command-line and native clients can
 * explain the same failure. The former boolean comparison is retained below
 * for review; catalogue loading and candidate checks now share this function. */
#if 0
static int CatalogueIdentityConflict(const UmiApplicationModuleDescriptor *a,
    const UmiApplicationModuleDescriptor *b)
{
    if (CatalogueEqual(a->id, b->id) || CatalogueEqual(a->option, b->option) ||
        CatalogueEqual(a->component, b->component)) return 1;
    const char *left[] = {a->executable, a->native_executable, a->console_executable};
    const char *right[] = {b->executable, b->native_executable, b->console_executable};
    for (size_t i = 0U; i < 3U; ++i)
        for (size_t j = 0U; j < 3U; ++j)
            if (left[i][0] != '\0' && right[j][0] != '\0' && CatalogueEqual(left[i], right[j]))
                return 1;
    return 0;
}

#endif
static UmiApplicationCandidateReason CatalogueConflictReason(
    const UmiApplicationModuleDescriptor *a, const UmiApplicationModuleDescriptor *b)
{
    if (CatalogueEqual(a->id, b->id)) return UMI_APPLICATION_CANDIDATE_ID;
    if (CatalogueEqual(a->option, b->option)) return UMI_APPLICATION_CANDIDATE_OPTION;
    if (CatalogueEqual(a->component, b->component)) return UMI_APPLICATION_CANDIDATE_COMPONENT;
    const char *left[] = {a->executable, a->native_executable, a->console_executable};
    const char *right[] = {b->executable, b->native_executable, b->console_executable};
    for (size_t i = 0U; i < 3U; ++i)
        for (size_t j = 0U; j < 3U; ++j)
            if (left[i][0] != '\0' && right[j][0] != '\0' && CatalogueEqual(left[i], right[j]))
                return UMI_APPLICATION_CANDIDATE_EXECUTABLE;
    return UMI_APPLICATION_CANDIDATE_READY;
}
/* Keep the existing loader call shape while deriving its answer from the same
 * explanation used by a registration preview. */
static int CatalogueIdentityConflict(const UmiApplicationModuleDescriptor *a,
    const UmiApplicationModuleDescriptor *b)
{
    return CatalogueConflictReason(a, b) != UMI_APPLICATION_CANDIDATE_READY;
}

static UmiStatus CatalogueAdd(UmiApplicationCatalogue *catalogue, const char *path,
    UmiApplicationCatalogueIssue *issue)
{
    if (strncmp(path, "applications/", 13U) != 0) return UMI_STATUS_OK;
    issue->path[0] = issue->conflicting_path[0] = '\0';
    UmiStatus path_status = umi_path_copy(issue->path, sizeof issue->path, path);
    if (path_status != UMI_STATUS_OK) return path_status;
    if (!CatalogueSlug(path + 13U)) return UMI_STATUS_PARSE_ERROR;
    if (catalogue->summary.count == UMI_APPLICATION_CATALOGUE_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiApplicationCatalogueEntry *entry = &catalogue->entries[catalogue->summary.count];
    UmiStatus status = umi_path_copy(entry->path, sizeof entry->path, path);
    if (status != UMI_STATUS_OK) return status;
    (void)umi_path_copy(issue->path, sizeof issue->path, path);
    for (size_t i = 0U; i < catalogue->summary.count; ++i) {
        if (CatalogueEqual(path, catalogue->entries[i].path)) {
            (void)umi_path_copy(issue->conflicting_path, sizeof issue->conflicting_path,
                catalogue->entries[i].path);
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    char directory[UMI_PATH_CAPACITY], manifest[UMI_PATH_CAPACITY];
    status = umi_path_join(catalogue->summary.root, path, directory, sizeof directory);
    if (status == UMI_STATUS_OK)
        status = umi_path_join(directory, "application.umicom.yaml", manifest, sizeof manifest);
    if (status != UMI_STATUS_OK) return status;
    entry->manifest_status = umi_application_module_load(manifest, &entry->module);
    if (entry->manifest_status == UMI_STATUS_OK) {
        for (size_t i = 0U; i < catalogue->summary.count; ++i) {
            const UmiApplicationCatalogueEntry *previous = &catalogue->entries[i];
            if (previous->manifest_status == UMI_STATUS_OK &&
                CatalogueIdentityConflict(&entry->module, &previous->module)) {
                (void)umi_path_copy(issue->conflicting_path, sizeof issue->conflicting_path, previous->path);
                return UMI_STATUS_INVALID_STATE;
            }
        }
        ++catalogue->summary.ready;
    } else ++catalogue->summary.unavailable;
    ++catalogue->summary.count;
    return UMI_STATUS_OK;
}

/* Consume one complete Git capture. A final newline is optional; unexpected
 * blank records, control characters and malformed keys are never ignored. */
/* Keep the text parser for comparison. Binary capture below consumes exact
 * Git records, preserving NUL delimiters and rejecting incomplete output. */
#if 0
static UmiStatus CatalogueParse(UmiApplicationCatalogue *catalogue, char *text,
    UmiApplicationCatalogueIssue *issue)
{
    char *cursor = text;
    while (*cursor != '\0') {
        char *line = cursor, *end = strchr(cursor, '\n');
        if (end != NULL) { *end = '\0'; cursor = end + 1; }
        else cursor += strlen(cursor);
        size_t length = strlen(line);
        if (length != 0U && line[length - 1U] == '\r') line[--length] = '\0';
        char *separator = strchr(line, ' ');
        if (separator == NULL || !CatalogueKey(line, (size_t)(separator - line)))
            return UMI_STATUS_PARSE_ERROR;
        const char *path = separator + 1;
        if (*path == '\0') return UMI_STATUS_PARSE_ERROR;
        for (const char *p = path; *p != '\0'; ++p)
            if ((unsigned char)*p < 32U || (unsigned char)*p == 127U) return UMI_STATUS_PARSE_ERROR;
        UmiStatus status = CatalogueAdd(catalogue, path, issue);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

#endif
static UmiStatus CatalogueParseBytes(UmiApplicationCatalogue *catalogue,
    const unsigned char *bytes, size_t length, UmiApplicationCatalogueIssue *issue)
{
    size_t position = 0U;
    while (position < length) {
        const unsigned char *record = bytes + position;
        const unsigned char *end = memchr(record, 0, length - position);
        if (end == NULL) return UMI_STATUS_PARSE_ERROR;
        size_t recordLength = (size_t)(end - record);
        const unsigned char *separator = memchr(record, '\n', recordLength);
        if (separator == NULL || !CatalogueBinaryKey(record, (size_t)(separator - record)))
            return UMI_STATUS_PARSE_ERROR;
        const unsigned char *path = separator + 1;
        size_t pathLength = (size_t)(end - path);
        if (pathLength == 0U || pathLength >= UMI_PATH_CAPACITY) return UMI_STATUS_PARSE_ERROR;
        for (size_t i = 0U; i < pathLength; ++i)
            if (path[i] < 32U || path[i] == 127U) return UMI_STATUS_PARSE_ERROR;
        /* The record terminator is already a bounded NUL. CatalogueAdd copies
         * the selected path and never retains this capture's temporary bytes. */
        UmiStatus status = CatalogueAdd(catalogue, (const char *)path, issue);
        if (status != UMI_STATUS_OK) return status;
        position += recordLength + 1U;
    }
    return UMI_STATUS_OK;
}

/* Retain the text-capture loader for review. The replacement uses the shared
 * bounded byte capture so valid Git subsection names need no lossy splitting. */
#if 0
UmiStatus UmiApplicationCatalogueLoad(const char *root, const char *gitProgram,
    UmiApplicationCatalogue **out, UmiApplicationCatalogueIssue *issue)
{
    UmiApplicationCatalogueIssue localIssue = {0};
    UmiApplicationCatalogueIssue *diagnostic = issue != NULL ? issue : &localIssue;
    memset(diagnostic, 0, sizeof *diagnostic);
    if (out == NULL) { diagnostic->status = UMI_STATUS_INVALID_ARGUMENT; return diagnostic->status; }
    *out = NULL;
    if (root == NULL || UmiOutputFileValidatePath(root) != UMI_STATUS_OK) {
        diagnostic->status = UMI_STATUS_INVALID_ARGUMENT; return diagnostic->status;
    }
    UmiApplicationCatalogue *catalogue = calloc(1U, sizeof *catalogue);
    UmiRepositoryGitCommandResult *result = calloc(1U, sizeof *result);
    if (catalogue == NULL || result == NULL) {
        free(catalogue); free(result); diagnostic->status = UMI_STATUS_OUT_OF_MEMORY;
        return diagnostic->status;
    }
    UmiStatus status = umi_path_copy(catalogue->summary.root, sizeof catalogue->summary.root, root);
    char configuration[UMI_PATH_CAPACITY];
    if (status == UMI_STATUS_OK)
        status = umi_path_join(root, ".gitmodules", configuration, sizeof configuration);
    /* An absent registration file means an empty catalogue, not an implicit
     * directory scan. Read it once first to distinguish absence from I/O errors. */
    unsigned char *bytes = NULL;
    size_t length = 0U;
    if (status == UMI_STATUS_OK) status = UmiInputFileRead(configuration, 1024U * 1024U, &bytes, &length);
    if (status == UMI_STATUS_NOT_FOUND && umi_fs_is_directory(root)) status = UMI_STATUS_OK;
    else if (status == UMI_STATUS_OK) {
        if (memchr(bytes, '\0', length) != NULL) status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK) {
            UmiRepositoryInspectionContext context = {gitProgram, root, 15000U};
            const char *arguments[] = {"--no-optional-locks", "config", "--no-includes",
                "--file", configuration, "--get-regexp", "^submodule\\..*\\.path$"};
            status = umi_repository_git_command_execute(&context, arguments, 7U, result);
            /* Git's documented no-match exit has no output. No other nonzero
             * result, cancellation, timeout or truncated record is an empty list. */
            if (result->launched && result->exit_code == 1 && !result->timed_out &&
                !result->cancelled && !result->termination_requested && !result->output_truncated &&
                result->output[0] == '\0' &&
                (status == UMI_STATUS_INTERNAL_ERROR || status == UMI_STATUS_IO_ERROR))
                status = UMI_STATUS_OK;
            if (status == UMI_STATUS_OK) status = CatalogueParse(catalogue, result->output, diagnostic);
        }
    }
    /* Detect edits to registration while manifests were being collected.
     * This is a bounded observation, not a repository-wide read transaction. */
    if (status == UMI_STATUS_OK && bytes != NULL) {
        unsigned char *latest = NULL;
        size_t latestLength = 0U;
        status = UmiInputFileRead(configuration, 1024U * 1024U, &latest, &latestLength);
        if (status == UMI_STATUS_OK &&
            (latestLength != length || memcmp(bytes, latest, length) != 0))
            status = UMI_STATUS_INVALID_STATE;
        UmiInputFileFree(latest);
    }
    UmiInputFileFree(bytes);
    free(result);
    diagnostic->status = status;
    if (status != UMI_STATUS_OK) { free(catalogue); return status; }
    diagnostic->path[0] = diagnostic->conflicting_path[0] = '\0';
    *out = catalogue;
    return UMI_STATUS_OK;
}

#endif
UmiStatus UmiApplicationCatalogueLoad(const char *root, const char *gitProgram,
    UmiApplicationCatalogue **out, UmiApplicationCatalogueIssue *issue)
{
    UmiApplicationCatalogueIssue localIssue = {0};
    UmiApplicationCatalogueIssue *diagnostic = issue != NULL ? issue : &localIssue;
    memset(diagnostic, 0, sizeof *diagnostic);
    if (out == NULL) { diagnostic->status = UMI_STATUS_INVALID_ARGUMENT; return diagnostic->status; }
    *out = NULL;
    if (root == NULL || UmiOutputFileValidatePath(root) != UMI_STATUS_OK) {
        diagnostic->status = UMI_STATUS_INVALID_ARGUMENT; return diagnostic->status;
    }
    UmiApplicationCatalogue *catalogue = calloc(1U, sizeof *catalogue);
    UmiRepositoryGitCaptureResult *result = calloc(1U, sizeof *result);
    if (catalogue == NULL || result == NULL) {
        free(catalogue); free(result); diagnostic->status = UMI_STATUS_OUT_OF_MEMORY;
        return diagnostic->status;
    }
    UmiStatus status = umi_path_copy(catalogue->summary.root, sizeof catalogue->summary.root, root);
    char configuration[UMI_PATH_CAPACITY];
    if (status == UMI_STATUS_OK)
        status = umi_path_join(root, ".gitmodules", configuration, sizeof configuration);
    /* An absent registration file means an empty catalogue, not an implicit
     * directory scan. Read it once first to distinguish absence from I/O errors. */
    unsigned char *bytes = NULL;
    size_t length = 0U;
    if (status == UMI_STATUS_OK) status = UmiInputFileRead(configuration, UMI_APPLICATION_MODULE_MAX_BYTES, &bytes, &length);
    if (status == UMI_STATUS_NOT_FOUND && umi_fs_is_directory(root)) status = UMI_STATUS_OK;
    else if (status == UMI_STATUS_OK) {
        if (memchr(bytes, '\0', length) != NULL) status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK) {
            UmiRepositoryInspectionContext context = {gitProgram, root, 15000U};
            const char *arguments[] = {"--no-optional-locks", "config", "--no-includes",
                "--file", configuration, "--null", "--get-regexp", "^submodule\\..*\\.path$"};
            status = UmiRepositoryGitCapture(&context, arguments, 8U, 1024U * 1024U, NULL, result);
            /* Git's documented no-match exit has no output. No other nonzero
             * result, cancellation, timeout or truncated record is an empty list. */
            if (result->launched && result->exit_code == 1 && !result->timed_out &&
                !result->cancelled && !result->termination_requested && !result->output_truncated &&
                result->length == 0U &&
                (status == UMI_STATUS_INTERNAL_ERROR || status == UMI_STATUS_IO_ERROR))
                status = UMI_STATUS_OK;
            if (status == UMI_STATUS_OK) status = CatalogueParseBytes(catalogue, result->bytes, result->length, diagnostic);
        }
    }
    /* Detect edits to registration while manifests were being collected.
     * This is a bounded observation, not a repository-wide read transaction. */
    if (status == UMI_STATUS_OK && bytes != NULL) {
        unsigned char *latest = NULL;
        size_t latestLength = 0U;
        status = UmiInputFileRead(configuration, UMI_APPLICATION_MODULE_MAX_BYTES, &latest, &latestLength);
        if (status == UMI_STATUS_OK &&
            (latestLength != length || memcmp(bytes, latest, length) != 0))
            status = UMI_STATUS_INVALID_STATE;
        UmiInputFileFree(latest);
    }
    UmiInputFileFree(bytes);
    UmiRepositoryGitCaptureRelease(result);
    free(result);
    diagnostic->status = status;
    if (status != UMI_STATUS_OK) { free(catalogue); return status; }
    diagnostic->path[0] = diagnostic->conflicting_path[0] = '\0';
    *out = catalogue;
    return UMI_STATUS_OK;
}

UmiStatus UmiApplicationCatalogueSummaryRead(const UmiApplicationCatalogue *catalogue,
    UmiApplicationCatalogueSummary *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof *out);
    if (catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = catalogue->summary;
    return UMI_STATUS_OK;
}
const UmiApplicationCatalogueEntry *UmiApplicationCatalogueAt(
    const UmiApplicationCatalogue *catalogue, size_t index)
{
    return catalogue == NULL || index >= catalogue->summary.count ? NULL : &catalogue->entries[index];
}
void UmiApplicationCatalogueDestroy(UmiApplicationCatalogue *catalogue)
{
    free(catalogue);
}

UmiStatus UmiApplicationCatalogueCheckCandidate(
    const UmiApplicationCatalogue *catalogue, const char *path,
    const UmiApplicationModuleDescriptor *module, UmiApplicationCandidateReport *report)
{
    if (report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(report, 0, sizeof *report);
    report->reason = UMI_APPLICATION_CANDIDATE_INVALID;
    report->status = UMI_STATUS_INVALID_ARGUMENT;
    if (catalogue == NULL || path == NULL || module == NULL) return report->status;
    if (strncmp(path, "applications/", 13U) != 0 || !CatalogueSlug(path + 13U))
        return report->status;
    report->status = umi_application_module_validate(module);
    if (report->status != UMI_STATUS_OK) return report->status;
    /* Report known conflicts first, then incomplete evidence. A missing checkout
     * cannot be treated as an unused identity simply because it was not loaded. */
    const UmiApplicationCatalogueEntry *unavailable = NULL;
    for (size_t i = 0U; i < catalogue->summary.count; ++i) {
        const UmiApplicationCatalogueEntry *entry = &catalogue->entries[i];
        UmiApplicationCandidateReason reason = UMI_APPLICATION_CANDIDATE_READY;
        if (CatalogueEqual(path, entry->path)) reason = UMI_APPLICATION_CANDIDATE_PATH;
        else if (entry->manifest_status == UMI_STATUS_OK) {
            ++report->checked;
            reason = CatalogueConflictReason(module, &entry->module);
        } else if (unavailable == NULL) unavailable = entry;
        if (reason != UMI_APPLICATION_CANDIDATE_READY) {
            report->reason = reason;
            report->status = UMI_STATUS_ALREADY_EXISTS;
            (void)umi_path_copy(report->conflicting_path, sizeof report->conflicting_path, entry->path);
            return report->status;
        }
    }
    if (unavailable != NULL) {
        report->reason = UMI_APPLICATION_CANDIDATE_UNAVAILABLE;
        report->status = UMI_STATUS_INVALID_STATE;
        (void)umi_path_copy(report->conflicting_path, sizeof report->conflicting_path, unavailable->path);
    } else if (catalogue->summary.count >= UMI_APPLICATION_CATALOGUE_LIMIT) {
        report->reason = UMI_APPLICATION_CANDIDATE_CAPACITY;
        report->status = UMI_STATUS_CAPACITY_EXCEEDED;
    } else {
        report->reason = UMI_APPLICATION_CANDIDATE_READY;
        report->status = UMI_STATUS_OK;
    }
    return report->status;
}

const char *UmiApplicationCandidateReasonText(UmiApplicationCandidateReason reason)
{
    switch (reason) {
    case UMI_APPLICATION_CANDIDATE_READY: return "No registration conflict observed";
    case UMI_APPLICATION_CANDIDATE_PATH: return "The application path is already registered";
    case UMI_APPLICATION_CANDIDATE_ID: return "The application identity is already registered";
    case UMI_APPLICATION_CANDIDATE_OPTION: return "The build option is already used";
    case UMI_APPLICATION_CANDIDATE_COMPONENT: return "The installer component is already used";
    case UMI_APPLICATION_CANDIDATE_EXECUTABLE: return "An executable name is already used";
    case UMI_APPLICATION_CANDIDATE_UNAVAILABLE: return "An existing manifest is unavailable; collision review is incomplete";
    case UMI_APPLICATION_CANDIDATE_CAPACITY: return "The application catalogue is full";
    case UMI_APPLICATION_CANDIDATE_INVALID:
    default: return "The proposed application metadata or path is invalid";
    }
}
