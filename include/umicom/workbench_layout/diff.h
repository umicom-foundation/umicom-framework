/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/diff.h
 *
 * PURPOSE:
 *   Produce deterministic semantic differences between layout revisions for Beyond Compare-style inspection, review, audit and conflict resolution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DIFF_H
#define UMICOM_WORKBENCH_LAYOUT_DIFF_H

#include "umicom/workbench_layout/document.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchLayoutDiffEntry {
    uint32_t structure_size;
    UmiWorkbenchLayoutDiffKind kind;
    char node_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char field[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char before_value[UMI_WORKBENCH_LAYOUT_TEXT_CAPACITY];
    char after_value[UMI_WORKBENCH_LAYOUT_TEXT_CAPACITY];
    size_t before_index;
    size_t after_index;
} UmiWorkbenchLayoutDiffEntry;

typedef struct UmiWorkbenchLayoutDiff {
    uint32_t structure_size;
    UmiWorkbenchLayoutDiffEntry
        entries[UMI_WORKBENCH_LAYOUT_MAX_DIFF_ENTRIES];
    size_t entry_count;
    size_t metadata_change_count;
    size_t added_count;
    size_t removed_count;
    size_t moved_count;
    size_t changed_count;
    bool truncated;
} UmiWorkbenchLayoutDiff;

typedef struct UmiWorkbenchLayoutDiffOptions {
    uint32_t structure_size;
    bool include_metadata;
    bool include_geometry;
    bool include_order;
    bool include_revision;
    bool include_unchanged_nodes;
} UmiWorkbenchLayoutDiffOptions;

UmiWorkbenchLayoutDiffOptions
umi_workbench_layout_diff_options_default(void);

void umi_workbench_layout_diff_init(
    UmiWorkbenchLayoutDiff *diff);

UmiStatus umi_workbench_layout_compare(
    const UmiWorkbenchLayoutDocument *before,
    const UmiWorkbenchLayoutDocument *after,
    const UmiWorkbenchLayoutDiffOptions *options,
    UmiWorkbenchLayoutDiff *out_diff);

UmiStatus umi_workbench_layout_diff_add(
    UmiWorkbenchLayoutDiff *diff,
    UmiWorkbenchLayoutDiffKind kind,
    const char *node_id,
    const char *field,
    const char *before_value,
    const char *after_value,
    size_t before_index,
    size_t after_index);

const UmiWorkbenchLayoutDiffEntry *
umi_workbench_layout_diff_at(
    const UmiWorkbenchLayoutDiff *diff,
    size_t index);

bool umi_workbench_layout_diff_is_empty(
    const UmiWorkbenchLayoutDiff *diff);

UmiStatus umi_workbench_layout_diff_format(
    const UmiWorkbenchLayoutDiff *diff,
    char *buffer,
    size_t capacity,
    size_t *out_required);

#ifdef __cplusplus
}
#endif

#endif
