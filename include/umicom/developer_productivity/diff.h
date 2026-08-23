/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/diff.h
 *
 * PURPOSE:
 *   Provide a toolkit-neutral side-by-side text comparison engine suitable for
 *   IDE editors, source control, generated-code review and Beyond Compare-style
 *   comparison surfaces.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_DIFF_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_DIFF_H

#include "umicom/developer_productivity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_DIFF_MAX_LINES 512U
#define UMI_DEVELOPER_DIFF_MAX_ROWS 1024U
#define UMI_DEVELOPER_DIFF_MAX_HUNKS 256U

typedef enum UmiDeveloperDiffRowKind {
    UMI_DEVELOPER_DIFF_EQUAL = 0,
    UMI_DEVELOPER_DIFF_DELETE = 1,
    UMI_DEVELOPER_DIFF_INSERT = 2,
    UMI_DEVELOPER_DIFF_REPLACE = 3
} UmiDeveloperDiffRowKind;

typedef struct UmiDeveloperDiffOptions {
    int ignore_case;
    int ignore_all_whitespace;
    int ignore_trailing_whitespace;
    size_t context_lines;
} UmiDeveloperDiffOptions;

typedef struct UmiDeveloperDiffRow {
    UmiDeveloperDiffRowKind kind;
    size_t left_line;
    size_t right_line;
    char left_text[UMI_DEVELOPER_PRODUCTIVITY_LINE_CAPACITY];
    char right_text[UMI_DEVELOPER_PRODUCTIVITY_LINE_CAPACITY];
} UmiDeveloperDiffRow;

typedef struct UmiDeveloperDiffStats {
    size_t equal_rows;
    size_t inserted_rows;
    size_t deleted_rows;
    size_t replaced_rows;
    size_t changed_rows;
} UmiDeveloperDiffStats;

typedef struct UmiDeveloperDiffDocument UmiDeveloperDiffDocument;

void umi_developer_diff_options_init(UmiDeveloperDiffOptions *options);

UmiStatus umi_developer_diff_document_create(
    const char *left_text,
    const char *right_text,
    const UmiDeveloperDiffOptions *options,
    UmiDeveloperDiffDocument **out_document);

void umi_developer_diff_document_destroy(
    UmiDeveloperDiffDocument *document);

size_t umi_developer_diff_document_row_count(
    const UmiDeveloperDiffDocument *document);

UmiStatus umi_developer_diff_document_row_at(
    const UmiDeveloperDiffDocument *document,
    size_t index,
    UmiDeveloperDiffRow *out_row);

UmiDeveloperDiffStats umi_developer_diff_document_stats(
    const UmiDeveloperDiffDocument *document);

#ifdef __cplusplus
}
#endif

#endif
