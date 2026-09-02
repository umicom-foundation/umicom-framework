/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/unified_diff.h
 *
 * PURPOSE:
 *   Parse unified-diff text into bounded navigation records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_UNIFIED_DIFF_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_UNIFIED_DIFF_H

#include "umicom/developer_productivity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_UNIFIED_DIFF_LINE_CAPACITY 512U

/**
 * List the named developer unified diff line kind values accepted by this public contract.
 */
typedef enum UmiDeveloperUnifiedDiffLineKind {
    UMI_DEVELOPER_UNIFIED_DIFF_CONTEXT = 0,
    UMI_DEVELOPER_UNIFIED_DIFF_ADDITION = 1,
    UMI_DEVELOPER_UNIFIED_DIFF_DELETION = 2,
    UMI_DEVELOPER_UNIFIED_DIFF_HEADER = 3,
    UMI_DEVELOPER_UNIFIED_DIFF_HUNK = 4
} UmiDeveloperUnifiedDiffLineKind;

/**
 * Represent the developer unified diff line data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperUnifiedDiffLine {
    UmiDeveloperUnifiedDiffLineKind kind;
    size_t old_line;
    size_t new_line;
    char text[UMI_DEVELOPER_PRODUCTIVITY_LINE_CAPACITY];
} UmiDeveloperUnifiedDiffLine;

/**
 * Represent the developer unified diff data shared with callers of this public contract.
 */
typedef struct UmiDeveloperUnifiedDiff {
    UmiDeveloperUnifiedDiffLine
        lines[UMI_DEVELOPER_UNIFIED_DIFF_LINE_CAPACITY];
    size_t line_count;
    size_t additions;
    size_t deletions;
    size_t hunks;
} UmiDeveloperUnifiedDiff;

/**
 * Read developer unified diff into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_developer_unified_diff_parse(
    const char *text,
    UmiDeveloperUnifiedDiff *out_diff);

#ifdef __cplusplus
}
#endif

#endif
