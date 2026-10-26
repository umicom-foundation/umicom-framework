/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/education/course_companions.c
 * PURPOSE: Read-only native reconciliation of numbered HTML and Markdown
 *          course companions without moving or rewriting any source file.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/course_companions.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#if defined(_WIN32)
# include <io.h>
#else
# include <dirent.h>
#endif

typedef struct CourseNumberedFile {
    char slug[192];
    unsigned count;
} CourseNumberedFile;

static int CourseJoin(char *out, size_t capacity, const char *root, const char *relative)
{
    int n = snprintf(out, capacity, "%s/%s", root, relative);
    return n > 0 && (size_t)n < capacity;
}

static int CourseNameParse(const char *name, const char *extension,
                           uint32_t first, uint32_t last,
                           size_t *outOrdinal, char *outSlug)
{
    const size_t len = strlen(name), extLen = strlen(extension);
    size_t i = 0U;
    uint64_t number = 0U;
    if (len <= extLen + 2U || strcmp(name + len - extLen, extension) != 0)
        return 0;
    while (name[i] >= '0' && name[i] <= '9') {
        unsigned d = (unsigned)(name[i] - '0');
        if (number > (UINT32_MAX - d) / 10U) return 0;
        number = number * 10U + d;
        ++i;
    }
    if (i == 0U || name[i] != '-' || number < first || number > last) return 0;
    const size_t slugStart = i + 1U;
    size_t slugLength = len - extLen - slugStart;
    if (slugLength == 0U || slugLength >= 192U) return 0;
    for (size_t j = slugStart; j < len - extLen; ++j) {
        unsigned char c = (unsigned char)name[j];
        if (!((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-'))
            return 0;
    }
    memcpy(outSlug, name + slugStart, slugLength);
    outSlug[slugLength] = '\0';
    *outOrdinal = (size_t)(number - first);
    return 1;
}

static void CourseRecord(CourseNumberedFile *files, uint32_t first, uint32_t last,
                         const char *extension, const char *name)
{
    size_t ordinal = 0U;
    char slug[192];
    if (!CourseNameParse(name, extension, first, last, &ordinal, slug)) return;
    if (files[ordinal].count == 0U) {
        memcpy(files[ordinal].slug, slug, strlen(slug) + 1U);
    }
    if (files[ordinal].count != UINT32_MAX) ++files[ordinal].count;
}

/* A directory named like a lesson is not a published document.
 * stat also handles filesystems that return DT_UNKNOWN from readdir. */
static int CourseRegularAt(const char *directory, const char *filename)
{
    char path[4096];
    struct stat info;
    if (!CourseJoin(path, sizeof(path), directory, filename) ||
        stat(path, &info) != 0) return 0;
#if defined(_WIN32)
    return (info.st_mode & _S_IFMT) == _S_IFREG;
#else
    return S_ISREG(info.st_mode);
#endif
}

static UmiStatus CourseScan(const char *path, const char *extension,
                             uint32_t first, uint32_t last,
                             CourseNumberedFile *files)
{
#if defined(_WIN32)
    char pattern[4096];
    if (!CourseJoin(pattern, sizeof(pattern), path, "*"))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    struct _finddata_t item;
    intptr_t handle = _findfirst(pattern, &item);
    if (handle == -1) return UMI_STATUS_NOT_FOUND;
    do {
        if (CourseRegularAt(path, item.name))
            CourseRecord(files, first, last, extension, item.name);
    } while (_findnext(handle, &item) == 0);
    _findclose(handle);
#else
    DIR *dir = opendir(path);
    if (dir == NULL) return UMI_STATUS_NOT_FOUND;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (CourseRegularAt(path, entry->d_name))
            CourseRecord(files, first, last, extension, entry->d_name);
    }
    closedir(dir);
#endif
    return UMI_STATUS_OK;
}

UmiStatus UmiEducationCourseCompanionsAudit(
    const char *repositoryRoot, UmiEducationCourseKind kind,
    uint32_t firstNumber, uint32_t lastNumber,
    UmiEducationCourseCompanionReport *outReport)
{
    const char *markdownRelative = NULL, *htmlRelative = NULL;
    char markdownPath[4096], htmlPath[4096];
    CourseNumberedFile *markdown = NULL, *html = NULL;
    UmiEducationCourseCompanionReport candidate = {0};
    UmiStatus status;
    size_t count;
    if (repositoryRoot == NULL || repositoryRoot[0] == '\0' || outReport == NULL ||
        firstNumber == 0U || lastNumber < firstNumber ||
        lastNumber - firstNumber >= UMI_EDUCATION_COMPANION_MAX_SPAN)
        return UMI_STATUS_INVALID_ARGUMENT;
    switch (kind) {
    case UMI_EDUCATION_COURSE_NATIVE_AI:
        markdownRelative = "content/lessons/10-native-ai";
        htmlRelative = "content/courses/llm-from-scratch/lessons";
        break;
    case UMI_EDUCATION_COURSE_MATHEMATICS:
        markdownRelative = "content/lessons/11-maths-for-ai";
        htmlRelative = "content/courses/maths-for-ai/lessons";
        break;
    default: return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!CourseJoin(markdownPath, sizeof(markdownPath), repositoryRoot, markdownRelative) ||
        !CourseJoin(htmlPath, sizeof(htmlPath), repositoryRoot, htmlRelative))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    count = (size_t)(lastNumber - firstNumber) + 1U;
    markdown = calloc(count, sizeof(*markdown));
    html = calloc(count, sizeof(*html));
    if (markdown == NULL || html == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto finish;
    }
    status = CourseScan(markdownPath, ".md", firstNumber, lastNumber, markdown);
    if (status != UMI_STATUS_OK) goto finish;
    status = CourseScan(htmlPath, ".html", firstNumber, lastNumber, html);
    if (status != UMI_STATUS_OK) goto finish;
    candidate.firstNumber = firstNumber;
    candidate.lastNumber = lastNumber;
    for (size_t i = 0U; i < count; ++i) {
        if (markdown[i].count > 1U || html[i].count > 1U)
            ++candidate.duplicateNumberedPages;
        if (markdown[i].count == 0U && html[i].count == 0U) {
            ++candidate.bothMissing;
        } else if (markdown[i].count == 0U) {
            ++candidate.markdownMissing;
        } else if (html[i].count == 0U) {
            ++candidate.htmlMissing;
        } else if (strcmp(markdown[i].slug, html[i].slug) != 0) {
            ++candidate.differentSlugs;
        } else {
            ++candidate.completePairs;
        }
    }
    *outReport = candidate;
    status = UMI_STATUS_OK;
finish:
    free(html);
    free(markdown);
    return status;
}
