/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/diff_hunk.h
 *
 * PURPOSE:
 *   Define side-by-side and inline diff hunks for editors, reviews and merge tools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This is a product-neutral C23 model. The registry owns snapshot copies by
 * value; callers own external resources and coordinate cross-thread mutation.
 */
#ifndef UMICOM_EDITOR_DIFF_HUNK_H
#define UMICOM_EDITOR_DIFF_HUNK_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_EDITOR_DIFF_HUNK_CAPACITY 8192U
typedef struct UmiEditorDiffHunkSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char left_uri[1024];
    char right_uri[1024];
    uint64_t old_start;
    uint64_t old_count;
    uint64_t new_start;
    uint64_t new_count;
    int state;
    uint64_t revision;
} UmiEditorDiffHunkSnapshot;
typedef struct UmiEditorDiffHunkRegistry UmiEditorDiffHunkRegistry;
UmiStatus umi_editor_diff_hunk_registry_create(UmiEditorDiffHunkRegistry **out_registry);
void umi_editor_diff_hunk_registry_destroy(UmiEditorDiffHunkRegistry *registry);
UmiStatus umi_editor_diff_hunk_registry_upsert(UmiEditorDiffHunkRegistry *registry,const UmiEditorDiffHunkSnapshot *item);
UmiStatus umi_editor_diff_hunk_registry_remove(UmiEditorDiffHunkRegistry *registry,const char *id);
UmiStatus umi_editor_diff_hunk_registry_find(const UmiEditorDiffHunkRegistry *registry,const char *id,UmiEditorDiffHunkSnapshot *out_item);
UmiStatus umi_editor_diff_hunk_registry_at(const UmiEditorDiffHunkRegistry *registry,size_t index,UmiEditorDiffHunkSnapshot *out_item);
size_t umi_editor_diff_hunk_registry_count(const UmiEditorDiffHunkRegistry *registry);
uint64_t umi_editor_diff_hunk_registry_revision(const UmiEditorDiffHunkRegistry *registry);
#ifdef __cplusplus
}
#endif
#endif
