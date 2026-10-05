/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/installed_files.c
 * PURPOSE: Keep installed-file review and launch selection in the shared developer service.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "umicom/developer_project/installed_files.h"
#include "umicom/platform/input_file.h"
#include "umicom/platform/output_file.h"
#include "umicom/platform/path.h"
#include "umicom/document/text_encoding.h"
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#endif

typedef struct InstalledRecord
{
    char *path;
    UmiStatus status;
    UmiFileKind kind;
    uint64_t size, modified;
    bool candidate;
} InstalledRecord;
struct UmiProjectInstalledFiles
{
    UmiProjectInstalledSummary summary;
    char manifest_path[UMI_PATH_CAPACITY];
    unsigned char *manifest;
    size_t manifest_size;
    InstalledRecord *records;
};
/* Resolve every path from the selected project, never from the editor's own
 * working folder. Keep Windows drive-relative spellings from acquiring an
 * unintended meaning when a contributor adds a new UI frontend. */
static UmiStatus InstalledFolder(const char *input, const char *source, char *out)
{
    if (input == NULL || input[0] == '\0' ||
        !umi_document_utf8_validate((const unsigned char *)input, strlen(input), NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; input[i] != '\0'; ++i)
        if ((unsigned char)input[i] < 32U || (unsigned char)input[i] == 127U)
            return UMI_STATUS_INVALID_ARGUMENT;
#ifdef _WIN32
    if (!umi_path_is_absolute(input) && (input[0] == '/' || input[0] == '\\' || strchr(input, ':') != NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
#else
    if (strchr(input, '\\') != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
#endif
    UmiStatus status = umi_path_absolute(input, source, out, UMI_BUILD_PATH_CAPACITY);
    if (status == UMI_STATUS_OK)
        status = UmiOutputFileValidatePath(out);
    return status;
}
static UmiStatus InstalledRoots(const UmiBuildProfile *profile, UmiProjectInstalledSummary *out)
{
    if (profile == NULL ||
        memchr(profile->source_directory, '\0', sizeof(profile->source_directory)) == NULL ||
        memchr(profile->build_directory, '\0', sizeof(profile->build_directory)) == NULL ||
        memchr(profile->install_directory, '\0', sizeof(profile->install_directory)) == NULL ||
        !umi_path_is_absolute(profile->source_directory))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status =
        InstalledFolder(profile->source_directory, profile->source_directory, out->source_directory);
    if (status == UMI_STATUS_OK)
        status = InstalledFolder(profile->build_directory, out->source_directory, out->build_directory);
    if (status == UMI_STATUS_OK)
        status = InstalledFolder(profile->install_directory, out->source_directory, out->install_directory);
    return status;
}
/* Manifest order remains visible. Sort a temporary index to catch duplicates
 * without quadratic comparisons in installations with thousands of headers.
 * Windows ASCII case folds match ordinary drive-path spelling; Unicode case
 * aliases and filesystem aliases are not resolved by this lexical reader. */
static int InstalledCompare(const void *left, const void *right)
{
    const unsigned char *a = (const unsigned char *)*(const char *const *)left;
    const unsigned char *b = (const unsigned char *)*(const char *const *)right;
    for (;;)
    {
        unsigned char x = *a++, y = *b++;
#ifdef _WIN32
        if (x >= 'A' && x <= 'Z')
            x = (unsigned char)(x + ('a' - 'A'));
        if (y >= 'A' && y <= 'Z')
            y = (unsigned char)(y + ('a' - 'A'));
#endif
        if (x != y)
            return x < y ? -1 : 1;
        if (x == 0U)
            return 0;
    }
}
static bool InstalledCandidate(const char *path, UmiFileKind kind)
{
    if (kind != UMI_FILE_KIND_REGULAR)
        return false;
#ifdef _WIN32
    size_t length = strlen(path);
    if (length < 4U)
        return false;
    const char *tail = path + length - 4U;
    return tail[0] == '.' && (tail[1] == 'e' || tail[1] == 'E') && (tail[2] == 'x' || tail[2] == 'X') &&
           (tail[3] == 'e' || tail[3] == 'E');
#else
    return access(path, X_OK) == 0;
#endif
}
static void InstalledInspect(InstalledRecord *record)
{
    UmiFileInfo info;
    record->status = umi_directory_stat(record->path, &info);
    if (record->status != UMI_STATUS_OK)
        return;
    record->kind = info.kind;
    record->size = info.size;
    record->modified = info.modified_nanoseconds;
    record->candidate = InstalledCandidate(record->path, record->kind);
}
void UmiProjectInstalledFilesDestroy(UmiProjectInstalledFiles *files)
{
    if (files == NULL)
        return;
    for (size_t i = 0U; i < files->summary.count; ++i)
        free(files->records[i].path);
    free(files->records);
    UmiInputFileFree(files->manifest);
    free(files);
}
/* Parse first and publish once. A malformed late line must not leave a usable
 * prefix that appears to describe a complete installation. The original bytes
 * stay owned by the snapshot so selection can detect a later reinstall. */
static UmiStatus InstalledParse(UmiProjectInstalledFiles *files, const UmiCancellationToken *cancel)
{
    const unsigned char *bytes = files->manifest;
    size_t length = files->manifest_size;
    if (!umi_document_utf8_validate(bytes, length, NULL))
        return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 0U; i < length; ++i)
        if ((bytes[i] < 32U && bytes[i] != '\r' && bytes[i] != '\n') || bytes[i] == 127U)
            return UMI_STATUS_PARSE_ERROR;
    size_t lines = 0U;
    for (size_t i = 0U; i < length; ++i)
        if (bytes[i] == '\n')
            ++lines;
    if (length != 0U && bytes[length - 1U] != '\n')
        ++lines;
    if (lines > UMI_PROJECT_INSTALLED_FILE_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (lines == 0U)
        return UMI_STATUS_OK;
    files->records = calloc(lines, sizeof(*files->records));
    char **sorted = calloc(lines, sizeof(*sorted));
    if (files->records == NULL || sorted == NULL)
    {
        free(sorted);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    UmiStatus status = UMI_STATUS_OK;
    size_t offset = 0U;
    while (offset < length)
    {
        if (umi_cancellation_token_is_requested(cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        size_t end = offset;
        while (end < length && bytes[end] != '\n')
            ++end;
        size_t count = end - offset;
        if (count != 0U && bytes[offset + count - 1U] == '\r' && end < length)
            --count;
        if (count == 0U)
        {
            status = UMI_STATUS_PARSE_ERROR;
            break;
        }
        if (count >= UMI_PATH_CAPACITY)
        {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        char path[UMI_PATH_CAPACITY], normalized[UMI_PATH_CAPACITY];
        memcpy(path, bytes + offset, count);
        path[count] = '\0';
        if (strchr(path, '\r') != NULL)
        {
            status = UMI_STATUS_PARSE_ERROR;
            break;
        }
#ifndef _WIN32
        if (strchr(path, '\\') != NULL)
        {
            status = UMI_STATUS_PARSE_ERROR;
            break;
        }
#endif
        status = UmiOutputFileValidatePath(path);
        if (status != UMI_STATUS_OK)
            break;
        status = umi_path_normalise(path, normalized, sizeof(normalized));
        if (status != UMI_STATUS_OK)
            break;
        if (!umi_path_is_within(files->summary.install_directory, normalized) ||
            umi_path_equal(files->summary.install_directory, normalized))
        {
            status = UMI_STATUS_INVALID_ARGUMENT;
            break;
        }
        InstalledRecord *record = &files->records[files->summary.count];
        record->path = malloc(strlen(normalized) + 1U);
        if (record->path == NULL)
        {
            status = UMI_STATUS_OUT_OF_MEMORY;
            break;
        }
        strcpy(record->path, normalized);
        sorted[files->summary.count++] = record->path;
        InstalledInspect(record);
        offset = end < length ? end + 1U : end;
    }
    if (status == UMI_STATUS_OK)
    {
        qsort(sorted, lines, sizeof(*sorted), InstalledCompare);
        for (size_t i = 1U; i < lines; ++i)
            if (InstalledCompare(&sorted[i - 1U], &sorted[i]) == 0)
            {
                status = UMI_STATUS_ALREADY_EXISTS;
                break;
            }
    }
    free(sorted);
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    return status;
}
UmiStatus UmiProjectInstalledFilesRead(const UmiBuildProfile *profile, const UmiCancellationToken *cancel,
                                       UmiProjectInstalledFiles **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    UmiProjectInstalledFiles *files = calloc(1U, sizeof(*files));
    if (files == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = InstalledRoots(profile, &files->summary);
    if (status == UMI_STATUS_OK)
        status = umi_path_join(files->summary.build_directory, "install_manifest.txt", files->manifest_path,
                               sizeof(files->manifest_path));
    if (status == UMI_STATUS_OK)
        status = UmiInputFileRead(files->manifest_path, UMI_PROJECT_INSTALL_MANIFEST_LIMIT, &files->manifest,
                                  &files->manifest_size);
    if (status == UMI_STATUS_OK)
        status = InstalledParse(files, cancel);
    if (umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status != UMI_STATUS_OK)
    {
        UmiProjectInstalledFilesDestroy(files);
        return status;
    }
    *out = files;
    return UMI_STATUS_OK;
}
UmiStatus UmiProjectInstalledFilesSummary(const UmiProjectInstalledFiles *files,
                                          UmiProjectInstalledSummary *out)
{
    if (files == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = files->summary;
    return UMI_STATUS_OK;
}
UmiStatus UmiProjectInstalledFilesAt(const UmiProjectInstalledFiles *files, size_t index,
                                     UmiProjectInstalledFile *out)
{
    if (files == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= files->summary.count)
        return UMI_STATUS_NOT_FOUND;
    const InstalledRecord *record = &files->records[index];
    UmiProjectInstalledFile value = {0};
    strcpy(value.path, record->path);
    value.inspection_status = record->status;
    value.kind = record->kind;
    value.size = record->size;
    value.modified_nanoseconds = record->modified;
    value.program_candidate = record->candidate;
    *out = value;
    return UMI_STATUS_OK;
}
UmiStatus UmiProjectInstalledFilesSelect(const UmiProjectInstalledFiles *files, size_t index,
                                         const UmiBuildProfile *profile, const UmiCancellationToken *cancel,
                                         UmiBuildProfile *out)
{
    if (files == NULL || profile == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    if (index >= files->summary.count)
        return UMI_STATUS_NOT_FOUND;
    UmiProjectInstalledSummary roots = {0};
    UmiStatus status = InstalledRoots(profile, &roots);
    if (status != UMI_STATUS_OK)
        return status;
    if (!umi_path_equal(roots.source_directory, files->summary.source_directory) ||
        !umi_path_equal(roots.build_directory, files->summary.build_directory) ||
        !umi_path_equal(roots.install_directory, files->summary.install_directory))
        return UMI_STATUS_INVALID_STATE;
    const InstalledRecord *previous = &files->records[index];
    if (!previous->candidate)
        return UMI_STATUS_INVALID_STATE;
    unsigned char *bytes = NULL;
    size_t size = 0U;
    status = UmiInputFileRead(files->manifest_path, UMI_PROJECT_INSTALL_MANIFEST_LIMIT, &bytes, &size);
    if (status == UMI_STATUS_OK &&
        (size != files->manifest_size || memcmp(bytes, files->manifest, size) != 0))
        status = UMI_STATUS_INVALID_STATE;
    UmiInputFileFree(bytes);
    if (status != UMI_STATUS_OK)
        return status;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    InstalledRecord current = {0};
    current.path = previous->path;
    InstalledInspect(&current);
    if (current.status != UMI_STATUS_OK)
        return current.status;
    if (!current.candidate || current.kind != previous->kind || current.size != previous->size ||
        current.modified != previous->modified)
        return UMI_STATUS_INVALID_STATE;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    UmiBuildProfile selected = *profile;
    strcpy(selected.run_program, previous->path);
    *out = selected;
    return UMI_STATUS_OK;
}
