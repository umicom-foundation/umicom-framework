/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/file_filter.h
 *
 * PURPOSE:
 *   Define bounded reusable file-dialog filter descriptions for UI adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_FILE_FILTER_H
#define UMICOM_DOCUMENT_FILE_FILTER_H

#include <stddef.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_FILE_FILTER_MAX 16U
#define UMI_DOCUMENT_FILE_FILTER_PATTERN_MAX 16U
#define UMI_DOCUMENT_FILE_FILTER_TEXT_CAPACITY 128U

/**
 * Represent the document file filter data shared with callers of this public contract.
 */
typedef struct UmiDocumentFileFilter {
    char name[UMI_DOCUMENT_FILE_FILTER_TEXT_CAPACITY];
    char patterns[UMI_DOCUMENT_FILE_FILTER_PATTERN_MAX]
                 [UMI_DOCUMENT_FILE_FILTER_TEXT_CAPACITY];
    size_t pattern_count;
} UmiDocumentFileFilter;

/**
 * Represent the document file filter set data shared with callers of this public contract.
 */
typedef struct UmiDocumentFileFilterSet {
    UmiDocumentFileFilter filters[UMI_DOCUMENT_FILE_FILTER_MAX];
    size_t count;
} UmiDocumentFileFilterSet;

/**
 * Provide the document file filters default operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_file_filters_default(
    UmiDocumentFileFilterSet *out_filters);
/**
 * Provide the document file filter add pattern operation used by this module and its
 * client applications.
 */
UmiStatus umi_document_file_filter_add_pattern(
    UmiDocumentFileFilter *filter,
    const char *pattern);
/**
 * Provide the document file filter matches operation used by this module and its client
 * applications.
 */
int umi_document_file_filter_matches(const UmiDocumentFileFilter *filter,
                                     const char *file_name);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_FILE_FILTER_H */
