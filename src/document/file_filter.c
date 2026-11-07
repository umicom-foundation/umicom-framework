/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/file_filter.c
 *
 * PURPOSE:
 *   Build shared development filters and match simple wildcard extensions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/file_filter.h"
#include "umicom/platform/path.h"

#include <stdio.h>
#include <string.h>

/*
 * Provide the document file filter add pattern operation used by this module and its
 * client applications.
 */
/* The unbounded strlen implementation is retained for engineering review.
 * The replacement limits inspection to the public pattern capacity and supports
 * overlapping patterns while preserving the output on failure. */
#if 0
UmiStatus umi_document_file_filter_add_pattern(
    UmiDocumentFileFilter *filter,
    const char *pattern)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (filter == NULL || pattern == NULL || pattern[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (filter->pattern_count >= UMI_DOCUMENT_FILE_FILTER_PATTERN_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strlen(pattern) >= sizeof(filter->patterns[0])) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)snprintf(filter->patterns[filter->pattern_count],
                   sizeof(filter->patterns[filter->pattern_count]),
                   "%s", pattern);
    filter->pattern_count += 1U;
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_document_file_filter_add_pattern(
    UmiDocumentFileFilter *filter,
    const char *pattern)
{
    if (filter == NULL || pattern == NULL || pattern[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    if (filter->pattern_count >= UMI_DOCUMENT_FILE_FILTER_PATTERN_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t length = 0U;
    while (length < UMI_DOCUMENT_FILE_FILTER_TEXT_CAPACITY && pattern[length] != '\0')
        ++length;
    if (length == UMI_DOCUMENT_FILE_FILTER_TEXT_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* A caller may reuse a pattern already in this value object. Move the bytes
     * before incrementing the count so overlapping storage is well-defined. */
    memmove(filter->patterns[filter->pattern_count], pattern, length + 1U);
    ++filter->pattern_count;
    return UMI_STATUS_OK;
}

/* Provide the add filter operation used by this module and its client applications. */
static UmiStatus add_filter(UmiDocumentFileFilterSet *set,
                            const char *name,
                            const char *const *patterns,
                            size_t pattern_count)
{
    UmiDocumentFileFilter *filter;
    size_t index;
    UmiStatus status;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (set->count >= UMI_DOCUMENT_FILE_FILTER_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    filter = &set->filters[set->count++];
    (void)snprintf(filter->name, sizeof(filter->name), "%s", name);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < pattern_count; ++index) {
        status = umi_document_file_filter_add_pattern(filter, patterns[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the document file filters default operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_file_filters_default(UmiDocumentFileFilterSet *out_filters)
{
    static const char *const SOURCE[] = {
        "*.c", "*.h", "*.cc", "*.cpp", "*.hpp", "*.S", "*.s", "*.asm", "*.uc", "*.cxx", "*.hxx"
    };
    static const char *const SCRIPT[] = {
        "*.py", "*.js", "*.ts", "*.sh", "*.ps1", "*.cmake", "*.php", "*.jsx", "*.tsx", "*.pyi"
    };
    static const char *const DATA[] = {
        "*.json", "*.xml", "*.yaml", "*.yml", "*.sql", "*.csv"
    };
    static const char *const TEXT[] = {"*.txt", "*.md", "*.html", "*.css"};
    static const char *const ALL[] = {"*"};
    static const char *const PROGRAMMING[] = {
        "*.java", "*.cs", "*.kt", "*.kts", "*.swift", "*.rs", "*.go",
        "*.zig", "*.d", "*.di", "*.dart", "*.pas", "*.pp", "*.lpr"
    };
    static const char *const HARDWARE[] = {"*.vhd", "*.vhdl", "*.v", "*.vh", "*.sv", "*.svh"};
    static const char *const BUILD[] = {"CMakeLists.txt", "*.cmake", "Makefile", "GNUmakefile", "build.ninja"};
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_filters == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_filters = (UmiDocumentFileFilterSet){0};
    status = add_filter(out_filters, "C, C++ and Assembly", SOURCE,
                        sizeof(SOURCE) / sizeof(SOURCE[0]));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = add_filter(out_filters, "Scripts", SCRIPT,
                        sizeof(SCRIPT) / sizeof(SCRIPT[0]));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = add_filter(out_filters, "Data and configuration", DATA,
                        sizeof(DATA) / sizeof(DATA[0]));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = add_filter(out_filters, "Documents", TEXT,
                        sizeof(TEXT) / sizeof(TEXT[0]));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = add_filter(out_filters, "All files", ALL, 1U);
    /* Preserve the original five filter positions for existing adapters. New
     * categories are appended; language support remains owned by Framework. */
    if (status == UMI_STATUS_OK) status = add_filter(out_filters, "Programming languages", PROGRAMMING,
                        sizeof(PROGRAMMING) / sizeof(PROGRAMMING[0]));
    if (status == UMI_STATUS_OK) status = add_filter(out_filters, "Hardware descriptions", HARDWARE,
                        sizeof(HARDWARE) / sizeof(HARDWARE[0]));
    if (status == UMI_STATUS_OK) status = add_filter(out_filters, "Build files", BUILD,
                        sizeof(BUILD) / sizeof(BUILD[0]));
    return status;
}

/*
 * Provide the document file filter matches operation used by this module and its client
 * applications.
 */
/* Retain the suffix-only matcher for review. The replacement validates public
 * bounds, matches a basename rather than dotted directories, and supports named
 * build files and ASCII suffix casing consistently across desktop hosts. */
#if 0
int umi_document_file_filter_matches(const UmiDocumentFileFilter *filter,
                                     const char *file_name)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (filter == NULL || file_name == NULL) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < filter->pattern_count; ++index) {
        const char *pattern = filter->patterns[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(pattern, "*") == 0 || strcmp(pattern, "*.*") == 0) return 1;
        /* Apply this branch only when its contract condition is satisfied. */
        if (pattern[0] == '*' && pattern[1] == '.') {
            const char *extension = strrchr(file_name, '.');
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (extension != NULL && strcmp(extension, pattern + 1U) == 0) return 1;
        }
    }
    return 0;
}
#endif

static int FilterAsciiEqual(const char *left, const char *right)
{
    while (*left != '\0' && *right != '\0') {
        unsigned char a = (unsigned char)*left++, b = (unsigned char)*right++;
        if (a >= 'A' && a <= 'Z') a = (unsigned char)(a + ('a' - 'A'));
        if (b >= 'A' && b <= 'Z') b = (unsigned char)(b + ('a' - 'A'));
        if (a != b) return 0;
    }
    return *left == *right;
}
int umi_document_file_filter_matches(const UmiDocumentFileFilter *filter,
                                     const char *file_name)
{
    if (filter == NULL || file_name == NULL ||
        filter->pattern_count > UMI_DOCUMENT_FILE_FILTER_PATTERN_MAX) return 0;
    size_t length = 0U;
    while (length < UMI_PATH_CAPACITY && file_name[length] != '\0') ++length;
    if (length == UMI_PATH_CAPACITY || length == 0U) return 0;
    const char *base = file_name;
    for (size_t i = 0U; i < length; ++i)
        if (file_name[i] == '/' || file_name[i] == '\\') base = file_name + i + 1U;
    if (*base == '\0') return 0;
    /* Validate every stored pattern before any match. A malformed later record
     * must not be concealed by an earlier wildcard that matches everything. */
    for (size_t i = 0U; i < filter->pattern_count; ++i)
        if (memchr(filter->patterns[i], '\0', UMI_DOCUMENT_FILE_FILTER_TEXT_CAPACITY) == NULL)
            return 0;
    const char *extension = strrchr(base, '.');
    for (size_t i = 0U; i < filter->pattern_count; ++i) {
        const char *pattern = filter->patterns[i];
        if (!strcmp(pattern, "*") || !strcmp(pattern, "*.*")) return 1;
        if (pattern[0] == '*' && pattern[1] == '.') {
            if (extension != NULL && FilterAsciiEqual(extension, pattern + 1U)) return 1;
        } else if (pattern[0] != '\0' && FilterAsciiEqual(base, pattern)) {
            return 1;
        }
    }
    return 0;
}
