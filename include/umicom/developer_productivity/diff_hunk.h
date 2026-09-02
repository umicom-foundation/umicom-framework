/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/diff_hunk.h
 *
 * PURPOSE:
 *   Group changed diff rows into navigable hunks with configurable context.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_DIFF_HUNK_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_DIFF_HUNK_H

#include "umicom/developer_productivity/diff.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the developer diff hunk data shared with callers of this public contract.
 */
typedef struct UmiDeveloperDiffHunk {
    size_t first_row;
    size_t last_row;
    size_t changed_rows;
    size_t left_start;
    size_t right_start;
} UmiDeveloperDiffHunk;

/**
 * Represent the developer diff hunk list data shared with callers of this public contract.
 */
typedef struct UmiDeveloperDiffHunkList {
    UmiDeveloperDiffHunk hunks[UMI_DEVELOPER_DIFF_MAX_HUNKS];
    size_t count;
} UmiDeveloperDiffHunkList;

/**
 * Provide the developer diff hunks build operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_diff_hunks_build(
    const UmiDeveloperDiffDocument *document,
    size_t context_lines,
    UmiDeveloperDiffHunkList *out_hunks);

#ifdef __cplusplus
}
#endif

#endif
