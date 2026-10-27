/*-----------------------------------------------------------------------------
 * Umicom Framework | Native C23 course publication quality gate
 * File: src/education/course_release.c
 * Reuse Umicom Education's companion inspector; never rewrite published files.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation | LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/course_release.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#if defined(_WIN32)
# include <io.h>
#else
# include <dirent.h>
#endif

#define UMI_COURSE_RELEASE_PATH 4096U
#define UMI_COURSE_RELEASE_NAME 256U
#define UMI_COURSE_RELEASE_FILE_MAX (512U * 1024U)

static int CourseReleaseJoin(char *out, size_t capacity,
                             const char *parent, const char *leaf)
{
    const int n = snprintf(out, capacity, "%s/%s", parent, leaf);
    return n > 0 && (size_t)n < capacity;
}

static int CourseReleaseRegular(const char *path)
{
    struct stat info;
    if (stat(path, &info) != 0) return 0;
#if defined(_WIN32)
    return (info.st_mode & _S_IFMT) == _S_IFREG;
#else
    return S_ISREG(info.st_mode);
#endif
}

/* Normalize relative components. Reject any path which climbs outside the
 * repository, including malformed drive/absolute/URL inputs. Filesystem
 * access remains read-only and the trusted checkout path is caller-owned. */
static int CourseReleaseResolve(const char *base, const char *href,
                                char *out, size_t capacity)
{
    if (base == NULL || href == NULL || href[0] == '\0' ||
        href[0] == '/' || href[0] == '\\') return 0;
    char joined[UMI_COURSE_RELEASE_PATH];
    if (!CourseReleaseJoin(joined, sizeof(joined), base, href)) return 0;
    size_t len = 0U;
    for (size_t i = 0U; joined[i] != '\0' &&
         joined[i] != '?' && joined[i] != '#'; ) {
        size_t start = i;
        while (joined[i] != '\0' && joined[i] != '/' &&
               joined[i] != '?' && joined[i] != '#') {
            const unsigned char c = (unsigned char)joined[i];
            if (c < 0x20U || c == '\\' || c == ':' || c == '%' ||
                c == '<' || c == '>' || c == '"') return 0;
            ++i;
        }
        const size_t segment = i - start;
        if (segment == 0U || (segment == 1U && joined[start] == '.')) {
            /* Empty and current-directory segments are harmless. */
        } else if (segment == 2U && joined[start] == '.' &&
                   joined[start + 1U] == '.') {
            if (len == 0U) return 0;
            while (len != 0U && out[len - 1U] != '/') --len;
            if (len != 0U) --len;
        } else {
            if (len + segment + 2U > capacity) return 0;
            if (len != 0U) out[len++] = '/';
            memcpy(out + len, joined + start, segment);
            len += segment;
        }
        if (joined[i] == '/') ++i;
    }
    if (len == 0U) return 0;
    out[len] = '\0';
    return 1;
}

static int CourseReleaseInternalMarker(const char *content)
{
    /* The match set is deliberately narrow; generic lessons may legitimately
     * explain Git, Python, security checks or software development. */
    static const char *const markers[] = {
        "Engineering follow-up",
        "Do not stage",
        "Umicom-Private",
        "MERGE-GUIDE",
        "git push",
        "Set-Location",
        "PRIVATE-NOTES"
    };
    for (size_t i = 0U; i < sizeof(markers) / sizeof(markers[0]); ++i)
        if (strstr(content, markers[i]) != NULL) return 1;
    return 0;
}

static int CourseReleaseRead(const char *path, char **outContent)
{
    FILE *input = fopen(path, "rb");
    if (input == NULL) return 0;
    if (fseek(input, 0L, SEEK_END) != 0) { fclose(input); return 0; }
    const long length = ftell(input);
    if (length < 0L || (unsigned long)length > UMI_COURSE_RELEASE_FILE_MAX ||
        fseek(input, 0L, SEEK_SET) != 0) { fclose(input); return 0; }
    char *content = malloc((size_t)length + 1U);
    if (content == NULL) { fclose(input); return 0; }
    const size_t received = fread(content, 1U, (size_t)length, input);
    const int readError = ferror(input);
    fclose(input);
    if (readError || received != (size_t)length ||
        memchr(content, '\0', received) != NULL) {
        free(content);
        return 0;
    }
    content[received] = '\0';
    *outContent = content;
    return 1;
}

static void CourseReleaseCheckLinks(const char *repo, const char *htmlRelative,
                                     const char *htmlText,
                                     UmiEducationCourseReleaseReport *report)
{
    const char *at = htmlText;
    while ((at = strstr(at, "href=\"")) != NULL) {
        at += sizeof("href=\"") - 1U;
        const char *end = strchr(at, '"');
        if (end == NULL) { ++report->unsafeLocalLinks; break; }
        const size_t length = (size_t)(end - at);
        if (length != 0U && at[0] != '#' &&
            strncmp(at, "https://", 8U) != 0 &&
            strncmp(at, "http://", 7U) != 0 &&
            strncmp(at, "mailto:", 7U) != 0 &&
            strncmp(at, "data:", 5U) != 0) {
            char reference[UMI_COURSE_RELEASE_PATH];
            char relative[UMI_COURSE_RELEASE_PATH];
            char complete[UMI_COURSE_RELEASE_PATH];
            if (length >= sizeof(reference)) {
                ++report->unsafeLocalLinks;
            } else {
                memcpy(reference, at, length);
                reference[length] = '\0';
                if (!CourseReleaseResolve(htmlRelative, reference,
                                          relative, sizeof(relative)) ||
                    !CourseReleaseJoin(complete, sizeof(complete), repo, relative)) {
                    ++report->unsafeLocalLinks;
                } else {
                    ++report->localLinksChecked;
                    if (!CourseReleaseRegular(complete)) ++report->missingLocalLinks;
                }
            }
        }
        at = end + 1U;
    }
}

static int CourseReleaseNumberedHtml(const char *name,
                                      uint32_t first, uint32_t last)
{
    unsigned long number = 0UL;
    size_t i = 0U;
    while (name[i] >= '0' && name[i] <= '9') {
        const unsigned d = (unsigned)(name[i] - '0');
        if (number > (UINT32_MAX - d) / 10U) return 0;
        number = number * 10UL + d;
        ++i;
    }
    if (i == 0U || name[i] != '-' || number < first || number > last) return 0;
    const size_t length = strlen(name);
    return length > i + 6U && length < UMI_COURSE_RELEASE_NAME &&
        strcmp(name + length - 5U, ".html") == 0;
}

static void CourseReleaseVisit(const char *root, const char *htmlRelative,
                               const char *markdownRelative, const char *name,
                               UmiEducationCourseReleaseReport *report)
{
    char htmlFile[UMI_COURSE_RELEASE_PATH], markdownFile[UMI_COURSE_RELEASE_PATH];
    char htmlAbsolute[UMI_COURSE_RELEASE_PATH];
    char markdownAbsolute[UMI_COURSE_RELEASE_PATH];
    const size_t length = strlen(name);
    if (length < 6U) return;
    char markdownName[UMI_COURSE_RELEASE_NAME];
    memcpy(markdownName, name, length - 5U);
    memcpy(markdownName + length - 5U, ".md", 4U);
    if (!CourseReleaseJoin(htmlFile, sizeof(htmlFile), htmlRelative, name) ||
        !CourseReleaseJoin(markdownFile, sizeof(markdownFile), markdownRelative, markdownName) ||
        !CourseReleaseJoin(htmlAbsolute, sizeof(htmlAbsolute), root, htmlFile) ||
        !CourseReleaseJoin(markdownAbsolute, sizeof(markdownAbsolute), root, markdownFile)) {
        ++report->unreadableOrOversizedFiles;
        return;
    }
    char *content = NULL;
    if (CourseReleaseRead(htmlAbsolute, &content)) {
        ++report->htmlPagesRead;
        if (CourseReleaseInternalMarker(content)) ++report->internalEditorialMarkers;
        CourseReleaseCheckLinks(root, htmlRelative, content, report);
        free(content);
    } else ++report->unreadableOrOversizedFiles;
    if (CourseReleaseRegular(markdownAbsolute)) {
        if (CourseReleaseRead(markdownAbsolute, &content)) {
            ++report->markdownPagesRead;
            if (CourseReleaseInternalMarker(content)) ++report->internalEditorialMarkers;
            free(content);
        } else ++report->unreadableOrOversizedFiles;
    }
}

UmiStatus UmiEducationCourseReleaseAudit(
    const char *repositoryRoot, UmiEducationCourseKind kind,
    uint32_t firstNumber, uint32_t lastNumber,
    UmiEducationCourseReleaseReport *outReport)
{
    if (repositoryRoot == NULL || *repositoryRoot == '\0' ||
        outReport == NULL || firstNumber == 0U || lastNumber < firstNumber ||
        lastNumber - firstNumber >= UMI_EDUCATION_COMPANION_MAX_SPAN)
        return UMI_STATUS_INVALID_ARGUMENT;
    const char *htmlRelative = NULL, *markdownRelative = NULL;
    if (kind == UMI_EDUCATION_COURSE_NATIVE_AI) {
        htmlRelative = "content/courses/llm-from-scratch/lessons";
        markdownRelative = "content/lessons/10-native-ai";
    } else if (kind == UMI_EDUCATION_COURSE_MATHEMATICS) {
        htmlRelative = "content/courses/maths-for-ai/lessons";
        markdownRelative = "content/lessons/11-maths-for-ai";
    } else return UMI_STATUS_INVALID_ARGUMENT;
    UmiEducationCourseReleaseReport staged = {0};
    UmiStatus status = UmiEducationCourseCompanionsAudit(repositoryRoot,
        kind, firstNumber, lastNumber, &staged.companions);
    if (status != UMI_STATUS_OK) return status;
    char htmlAbsolute[UMI_COURSE_RELEASE_PATH];
    if (!CourseReleaseJoin(htmlAbsolute, sizeof(htmlAbsolute),
                           repositoryRoot, htmlRelative))
        return UMI_STATUS_CAPACITY_EXCEEDED;
#if defined(_WIN32)
    char pattern[UMI_COURSE_RELEASE_PATH];
    if (!CourseReleaseJoin(pattern, sizeof(pattern), htmlAbsolute, "*"))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    struct _finddata_t entry;
    intptr_t search = _findfirst(pattern, &entry);
    if (search == -1) return UMI_STATUS_NOT_FOUND;
    do {
        if (CourseReleaseNumberedHtml(entry.name, firstNumber, lastNumber))
            CourseReleaseVisit(repositoryRoot, htmlRelative, markdownRelative,
                               entry.name, &staged);
    } while (_findnext(search, &entry) == 0);
    _findclose(search);
#else
    DIR *dir = opendir(htmlAbsolute);
    if (dir == NULL) return UMI_STATUS_NOT_FOUND;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (CourseReleaseNumberedHtml(entry->d_name, firstNumber, lastNumber))
            CourseReleaseVisit(repositoryRoot, htmlRelative, markdownRelative,
                               entry->d_name, &staged);
    }
    closedir(dir);
#endif
    *outReport = staged;
    return UMI_STATUS_OK;
}

int UmiEducationCourseReleaseReady(const UmiEducationCourseReleaseReport *report)
{
    if (report == NULL) return 0;
    const UmiEducationCourseCompanionReport *pairs = &report->companions;
    const size_t expected = (size_t)(pairs->lastNumber - pairs->firstNumber) + 1U;
    return pairs->completePairs == expected &&
           pairs->markdownMissing == 0U && pairs->htmlMissing == 0U &&
           pairs->bothMissing == 0U && pairs->differentSlugs == 0U &&
           pairs->duplicateNumberedPages == 0U &&
           report->htmlPagesRead == expected && report->markdownPagesRead == expected &&
           report->missingLocalLinks == 0U && report->unsafeLocalLinks == 0U &&
           report->internalEditorialMarkers == 0U &&
           report->unreadableOrOversizedFiles == 0U;
}
