/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/file_filter.c
 *
 * PURPOSE:
 *   Build shared development filters and match simple wildcard extensions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/file_filter.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_document_file_filter_add_pattern(
    UmiDocumentFileFilter *filter,
    const char *pattern)
{
    if (filter == NULL || pattern == NULL || pattern[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (filter->pattern_count >= UMI_DOCUMENT_FILE_FILTER_PATTERN_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strlen(pattern) >= sizeof(filter->patterns[0])) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)snprintf(filter->patterns[filter->pattern_count],
                   sizeof(filter->patterns[filter->pattern_count]),
                   "%s", pattern);
    filter->pattern_count += 1U;
    return UMI_STATUS_OK;
}

static UmiStatus add_filter(UmiDocumentFileFilterSet *set,
                            const char *name,
                            const char *const *patterns,
                            size_t pattern_count)
{
    UmiDocumentFileFilter *filter;
    size_t index;
    UmiStatus status;
    if (set->count >= UMI_DOCUMENT_FILE_FILTER_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    filter = &set->filters[set->count++];
    (void)snprintf(filter->name, sizeof(filter->name), "%s", name);
    for (index = 0U; index < pattern_count; ++index) {
        status = umi_document_file_filter_add_pattern(filter, patterns[index]);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

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
    if (out_filters == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_filters = (UmiDocumentFileFilterSet){0};
    status = add_filter(out_filters, "C, C++ and Assembly", SOURCE,
                        sizeof(SOURCE) / sizeof(SOURCE[0]));
    if (status == UMI_STATUS_OK) status = add_filter(out_filters, "Scripts", SCRIPT,
                        sizeof(SCRIPT) / sizeof(SCRIPT[0]));
    if (status == UMI_STATUS_OK) status = add_filter(out_filters, "Data and configuration", DATA,
                        sizeof(DATA) / sizeof(DATA[0]));
    if (status == UMI_STATUS_OK) status = add_filter(out_filters, "Documents", TEXT,
                        sizeof(TEXT) / sizeof(TEXT[0]));
    if (status == UMI_STATUS_OK) status = add_filter(out_filters, "All files", ALL, 1U);
    return status;
}

int umi_document_file_filter_matches(const UmiDocumentFileFilter *filter,
                                     const char *file_name)
{
    size_t index;
    if (filter == NULL || file_name == NULL) return 0;
    for (index = 0U; index < filter->pattern_count; ++index) {
        const char *pattern = filter->patterns[index];
        if (strcmp(pattern, "*") == 0 || strcmp(pattern, "*.*") == 0) return 1;
        if (pattern[0] == '*' && pattern[1] == '.') {
            const char *extension = strrchr(file_name, '.');
            if (extension != NULL && strcmp(extension, pattern + 1U) == 0) return 1;
        }
    }
    return 0;
}
