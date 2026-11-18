/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/education/nav_audit.c
 *
 * PURPOSE:
 *   Check the subset of MkDocs navigation used by the education site using
 *   bounded C23 buffers. Keep this separate from the YAML parser: no parser
 *   should silently claim that text inspection validates arbitrary YAML.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/education/nav_audit.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define UMI_NAV_LINE_CAPACITY 8192U
#define UMI_NAV_TARGET_CAPACITY 512U
/* The fixed-size index is retained for review of the former 2048-page limit.
 * UmiNavIndex below replaces it with owned paths and checked growth. */
#if 0
#define UMI_NAV_MAX_PAGES 2048U

typedef struct UmiNavTarget {
    char relative[UMI_NAV_TARGET_CAPACITY];
} UmiNavTarget;

#endif

static char *NavTrim(char *start)
{
    while (*start == ' ' || *start == '\t') ++start;
    size_t length = strlen(start);
    while (length != 0U && (start[length - 1U] == ' ' ||
            start[length - 1U] == '\t' || start[length - 1U] == '\r' ||
            start[length - 1U] == '\n'))
        start[--length] = '\0';
    return start;
}

static int NavHasSuffix(const char *text, const char *suffix)
{
    size_t textLength = strlen(text), suffixLength = strlen(suffix);
    return textLength >= suffixLength &&
           strcmp(text + textLength - suffixLength, suffix) == 0;
}

/* Navigation paths are repository-relative identifiers, not file-system
 * instructions. Check each path component before joining it to the root.
 * Explicitly refuse parent traversal even if it would remain in the project:
 * the whole navigation uses paths relative to content/ for consistency. */
static int NavTargetIsSafe(const char *path)
{
    if (path == NULL || *path == '\0' || path[0] == '/' ||
        !NavHasSuffix(path, ".md")) return 0;
    size_t componentStart = 0U, length = strlen(path);
    if (length >= UMI_NAV_TARGET_CAPACITY) return 0;
    for (size_t i = 0U; i <= length; ++i) {
        unsigned char value = (unsigned char)path[i];
        if (value == '/' || value == '\0') {
            size_t count = i - componentStart;
            if (count == 0U || (count == 1U && path[componentStart] == '.') ||
                (count == 2U && path[componentStart] == '.' &&
                 path[componentStart + 1U] == '.')) return 0;
            componentStart = i + 1U;
            continue;
        }
        /* Permit ordinary UTF-8 filename bytes. Backslash, colon, control
         * characters, URL metacharacters and quoted-file ambiguity do not
         * belong to these portable navigation target names. */
        if (value < 0x20U || value == 0x7fU || value == '\\' ||
            value == ':' || value == '?' || value == '#' || value == '%' ||
            value == '<' || value == '>' || value == '"' || value == '\'')
            return 0;
    }
    return 1;
}

static int NavIsRegularFile(const char *path)
{
    struct stat info;
    if (stat(path, &info) != 0) return 0;
#if defined(_WIN32)
    return (info.st_mode & _S_IFMT) == _S_IFREG;
#else
    return S_ISREG(info.st_mode);
#endif
}

/* Convert a MkDocs navigation item to its local Markdown target. Values
 * without a .md suffix represent groups or off-site URLs and are ignored.
 * Support a quoted path and harmless trailing YAML comments. The exact
 * parser for arbitrary quoted keys/escapes remains MkDocs, not this audit. */
static int NavReadMarkdownTarget(char *line, char **outTarget)
{
    *outTarget = NULL;
    char *entry = NavTrim(line);
    if (entry[0] != '-' || entry[1] != ' ') return 0;
    char *separator = strrchr(entry + 2, ':');
    if (separator == NULL) return 0;
    char *value = NavTrim(separator + 1);
    if (*value == '\0') return 0;
    if (*value == '"' || *value == '\'') {
        char quote = *value++;
        char *end = strchr(value, quote);
        if (end == NULL) return -1;
        *end = '\0';
        if (*NavTrim(end + 1U) != '\0' && *NavTrim(end + 1U) != '#') return -1;
    } else {
        /* Strip a trailing comment only when separated by whitespace. */
        for (char *p = value; *p != '\0'; ++p) {
            if (*p == '#' && p != value && (p[-1] == ' ' || p[-1] == '\t')) {
                *p = '\0';
                break;
            }
        }
        value = NavTrim(value);
    }
    if (!NavHasSuffix(value, ".md")) return 0;
    *outTarget = value;
    return 1;
}

/* The original capped audit is preserved for engineering review. The active
 * implementation below retains the same parser and path checks, replacing
 * only index ownership, duplicate lookup and resource-failure reporting. */
#if 0
int UmiEducationAuditNavigation(const char *repositoryRoot,
                                UmiEducationNavReport *outReport,
                                FILE *diagnostics)
{
    if (repositoryRoot == NULL || repositoryRoot[0] == '\0' || outReport == NULL)
        return 2;
    *outReport = (UmiEducationNavReport){0};
    char configuration[4096];
    int count = snprintf(configuration, sizeof(configuration), "%s/mkdocs.yml", repositoryRoot);
    if (count < 0 || (size_t)count >= sizeof(configuration)) return 2;
    FILE *input = fopen(configuration, "rb");
    if (input == NULL) {
        if (diagnostics != NULL) fprintf(diagnostics, "Cannot read: %s\n", configuration);
        return 2;
    }
    /* A bounded, heap-owned index avoids large stacks and disallows excessive
     * navigation before it can exhaust memory or overflow its count. */
    UmiNavTarget *seen = calloc(UMI_NAV_MAX_PAGES, sizeof(*seen));
    if (seen == NULL) { fclose(input); return 2; }
    char line[UMI_NAV_LINE_CAPACITY];
    size_t lineNumber = 0U;
    int insideNav = 0, foundNav = 0;
    while (fgets(line, (int)sizeof(line), input) != NULL) {
        ++lineNumber;
        size_t lineLength = strlen(line);
        if (lineLength != 0U && line[lineLength - 1U] != '\n' && !feof(input)) {
            if (diagnostics != NULL)
                fprintf(diagnostics, "mkdocs.yml:%zu: navigation line is too long.\n", lineNumber);
            ++outReport->structuralErrors;
            int ch;
            while ((ch = fgetc(input)) != EOF && ch != '\n') { /* Discard remainder. */ }
            continue;
        }
        char *trimmed = NavTrim(line);
        if (!insideNav) {
            if (strcmp(trimmed, "nav:") == 0 && line[0] == 'n') {
                insideNav = 1;
                foundNav = 1;
            }
            continue;
        }
        if (line[0] != ' ' && line[0] != '\t' && trimmed[0] != '\0' &&
            trimmed[0] != '#') break; /* Next top-level YAML key. */
        if (*trimmed == '\0' || *trimmed == '#') continue;
        char *target = NULL;
        int parsed = NavReadMarkdownTarget(trimmed, &target);
        if (parsed < 0) {
            ++outReport->structuralErrors;
            if (diagnostics != NULL)
                fprintf(diagnostics, "mkdocs.yml:%zu: incomplete quoted target.\n", lineNumber);
            continue;
        }
        if (parsed == 0) continue;
        if (!NavTargetIsSafe(target)) {
            ++outReport->unsafeCount;
            if (diagnostics != NULL)
                fprintf(diagnostics, "mkdocs.yml:%zu: unsafe navigation target: %s\n",
                        lineNumber, target);
            continue;
        }
        if (outReport->pageCount == UMI_NAV_MAX_PAGES) {
            ++outReport->structuralErrors;
            if (diagnostics != NULL)
                fputs("mkdocs.yml: too many navigation targets.\n", diagnostics);
            break;
        }
        int duplicate = 0;
        for (size_t i = 0U; i < outReport->pageCount; ++i)
            if (strcmp(seen[i].relative, target) == 0) { duplicate = 1; break; }
        if (duplicate) {
            ++outReport->duplicateCount;
            if (diagnostics != NULL)
                fprintf(diagnostics, "mkdocs.yml:%zu: duplicate target: %s\n", lineNumber, target);
        }
        (void)strcpy(seen[outReport->pageCount].relative, target); /* Size checked above. */
        ++outReport->pageCount;
        char path[4096];
        int written = snprintf(path, sizeof(path), "%s/content/%s", repositoryRoot, target);
        if (written < 0 || (size_t)written >= sizeof(path) || !NavIsRegularFile(path)) {
            ++outReport->missingCount;
            if (diagnostics != NULL)
                fprintf(diagnostics, "mkdocs.yml:%zu: missing Markdown: content/%s\n",
                        lineNumber, target);
        }
    }
    int readFailure = ferror(input);
    (void)fclose(input);
    free(seen);
    if (!foundNav || outReport->pageCount == 0U || readFailure)
        ++outReport->structuralErrors;
    if (diagnostics != NULL) {
        fprintf(diagnostics,
                "Navigation audit: %zu page(s), %zu missing, %zu duplicate, "
                "%zu unsafe, %zu structural issue(s).\n",
                outReport->pageCount, outReport->missingCount,
                outReport->duplicateCount, outReport->unsafeCount,
                outReport->structuralErrors);
        fputs("Full YAML syntax and rendering still require MkDocs.\n", diagnostics);
    }
    return (outReport->missingCount || outReport->duplicateCount ||
            outReport->unsafeCount || outReport->structuralErrors) ? 1 : 0;
}
#endif

/* The index owns only the bytes actually present in each admitted path. Its
 * small record vector grows geometrically; checked budget arithmetic bounds
 * retained payload independently of the number of pages in a normal site.
 * Allocator metadata and transient realloc overhead are outside this budget. */
#ifndef UMI_NAV_INDEX_BUDGET
#define UMI_NAV_INDEX_BUDGET (64U * 1024U * 1024U)
#endif

typedef struct UmiNavOwnedTarget {
    char *relative;
    size_t lineNumber;
} UmiNavOwnedTarget;

typedef struct UmiNavIndex {
    UmiNavOwnedTarget *items;
    size_t count;
    size_t capacity;
    size_t textBytes;
} UmiNavIndex;

static int NavIndexAppend(UmiNavIndex *index, const char *target, size_t lineNumber)
{
    /* NavTargetIsSafe already bounded the target, including its terminator. */
    size_t bytes = strlen(target) + 1U;
    size_t budget = (size_t)UMI_NAV_INDEX_BUDGET;
    if (index->textBytes > budget || bytes > budget - index->textBytes) return 0;
    size_t maximum = (budget - index->textBytes - bytes) / sizeof(*index->items);
    if (index->capacity > maximum) return 0;
    if (index->count == index->capacity) {
        size_t next = index->capacity == 0U ? 64U :
            (index->capacity > maximum / 2U ? maximum : index->capacity * 2U);
        if (next > maximum) next = maximum;
        if (next <= index->count) return 0;
        /* Assign only after success: realloc failure leaves the original
         * allocation owned by the index for normal cleanup. */
        UmiNavOwnedTarget *grown = realloc(index->items, next * sizeof(*grown));
        if (grown == NULL) return 0;
        index->items = grown;
        index->capacity = next;
    }
    char *owned = malloc(bytes);
    if (owned == NULL) return 0;
    memcpy(owned, target, bytes);
    index->items[index->count++] = (UmiNavOwnedTarget){owned, lineNumber};
    index->textBytes += bytes;
    return 1;
}

static int NavTargetCompare(const void *left, const void *right)
{
    const UmiNavOwnedTarget *a = left, *b = right;
    int compared = strcmp(a->relative, b->relative);
    if (compared != 0) return compared;
    /* Keep the first source occurrence first without subtracting size_t values. */
    return (a->lineNumber > b->lineNumber) - (a->lineNumber < b->lineNumber);
}

static void NavIndexFindDuplicates(UmiNavIndex *index,
                                   UmiEducationNavReport *report, FILE *diagnostics)
{
    if (index->count < 2U) return;
    /* Sorting avoids comparing every new page with every preceding page. Store
     * source lines with owned paths so deferred diagnostics still identify the
     * later occurrence, including duplicates beyond the old page ceiling. */
    qsort(index->items, index->count, sizeof(*index->items), NavTargetCompare);
    for (size_t i = 1U; i < index->count; ++i) {
        if (strcmp(index->items[i - 1U].relative, index->items[i].relative) != 0)
            continue;
        ++report->duplicateCount;
        if (diagnostics != NULL)
            fprintf(diagnostics, "mkdocs.yml:%zu: duplicate target: %s\n",
                    index->items[i].lineNumber, index->items[i].relative);
    }
}

static void NavIndexRelease(UmiNavIndex *index)
{
    for (size_t i = 0U; i < index->count; ++i) free(index->items[i].relative);
    free(index->items);
    *index = (UmiNavIndex){0};
}

int UmiEducationAuditNavigation(const char *repositoryRoot,
                                UmiEducationNavReport *outReport,
                                FILE *diagnostics)
{
    if (repositoryRoot == NULL || repositoryRoot[0] == '\0' || outReport == NULL)
        return 2;
    *outReport = (UmiEducationNavReport){0};
    char configuration[4096];
    int count = snprintf(configuration, sizeof(configuration), "%s/mkdocs.yml", repositoryRoot);
    if (count < 0 || (size_t)count >= sizeof(configuration)) return 2;
    FILE *input = fopen(configuration, "rb");
    if (input == NULL) {
        if (diagnostics != NULL) fprintf(diagnostics, "Cannot read: %s\n", configuration);
        return 2;
    }
    UmiNavIndex index = {0};
    int resourceFailure = 0;
    char line[UMI_NAV_LINE_CAPACITY];
    size_t lineNumber = 0U;
    int insideNav = 0, foundNav = 0;
    while (fgets(line, (int)sizeof(line), input) != NULL) {
        ++lineNumber;
        size_t lineLength = strlen(line);
        if (lineLength != 0U && line[lineLength - 1U] != '\n' && !feof(input)) {
            if (diagnostics != NULL)
                fprintf(diagnostics, "mkdocs.yml:%zu: navigation line is too long.\n", lineNumber);
            ++outReport->structuralErrors;
            int ch;
            while ((ch = fgetc(input)) != EOF && ch != '\n') { /* Discard remainder. */ }
            continue;
        }
        char *trimmed = NavTrim(line);
        if (!insideNav) {
            if (strcmp(trimmed, "nav:") == 0 && line[0] == 'n') {
                insideNav = 1;
                foundNav = 1;
            }
            continue;
        }
        if (line[0] != ' ' && line[0] != '\t' && trimmed[0] != '\0' &&
            trimmed[0] != '#') break; /* Next top-level YAML key. */
        if (*trimmed == '\0' || *trimmed == '#') continue;
        char *target = NULL;
        int parsed = NavReadMarkdownTarget(trimmed, &target);
        if (parsed < 0) {
            ++outReport->structuralErrors;
            if (diagnostics != NULL)
                fprintf(diagnostics, "mkdocs.yml:%zu: incomplete quoted target.\n", lineNumber);
            continue;
        }
        if (parsed == 0) continue;
        if (!NavTargetIsSafe(target)) {
            ++outReport->unsafeCount;
            if (diagnostics != NULL)
                fprintf(diagnostics, "mkdocs.yml:%zu: unsafe navigation target: %s\n",
                        lineNumber, target);
            continue;
        }
        if (!NavIndexAppend(&index, target, lineNumber)) {
            resourceFailure = 1;
            if (diagnostics != NULL)
                fprintf(diagnostics, "mkdocs.yml:%zu: navigation audit incomplete: "
                        "unable to retain targets within the %zu-byte index budget "
                        "or allocate memory.\n", lineNumber, (size_t)UMI_NAV_INDEX_BUDGET);
            break;
        }
        ++outReport->pageCount;
        char path[4096];
        int written = snprintf(path, sizeof(path), "%s/content/%s", repositoryRoot, target);
        if (written < 0 || (size_t)written >= sizeof(path) || !NavIsRegularFile(path)) {
            ++outReport->missingCount;
            if (diagnostics != NULL)
                fprintf(diagnostics, "mkdocs.yml:%zu: missing Markdown: content/%s\n",
                        lineNumber, target);
        }
    }
    int readFailure = ferror(input);
    (void)fclose(input);
    NavIndexFindDuplicates(&index, outReport, diagnostics);
    NavIndexRelease(&index);
    if (!foundNav || (outReport->pageCount == 0U && !resourceFailure) || readFailure)
        ++outReport->structuralErrors;
    if (diagnostics != NULL) {
        fprintf(diagnostics,
                "%s: %zu page(s), %zu missing, %zu duplicate, "
                "%zu unsafe, %zu structural issue(s).\n",
                resourceFailure ? "Navigation audit (incomplete)" : "Navigation audit",
                outReport->pageCount, outReport->missingCount,
                outReport->duplicateCount, outReport->unsafeCount,
                outReport->structuralErrors);
        fputs("Full YAML syntax and rendering still require MkDocs.\n", diagnostics);
    }
    if (resourceFailure) return 2;
    return (outReport->missingCount || outReport->duplicateCount ||
            outReport->unsafeCount || outReport->structuralErrors) ? 1 : 0;
}
