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

#include <stdio.h>
#include <string.h>

/*
 * Provide the document file filter add pattern operation used by this module and its
 * client applications.
 */
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
        "*.c", "*.h", "*.cc", "*.cpp", "*.hpp", "*.S", "*.s", "*.asm"
    };
    static const char *const SCRIPT[] = {
        "*.py", "*.js", "*.ts", "*.sh", "*.ps1", "*.cmake"
    };
    static const char *const DATA[] = {
        "*.json", "*.xml", "*.yaml", "*.yml", "*.sql", "*.csv"
    };
    static const char *const TEXT[] = {"*.txt", "*.md", "*.html", "*.css"};
    static const char *const ALL[] = {"*"};
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
    return status;
}

/*
 * Provide the document file filter matches operation used by this module and its client
 * applications.
 */
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
