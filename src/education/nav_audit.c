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
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/education/nav_audit.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define UMI_NAV_LINE_CAPACITY 8192U
#define UMI_NAV_TARGET_CAPACITY 512U
#define UMI_NAV_MAX_PAGES 2048U

typedef struct UmiNavTarget {
    char relative[UMI_NAV_TARGET_CAPACITY];
} UmiNavTarget;

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
