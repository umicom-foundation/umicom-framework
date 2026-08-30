/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/merge.h
 *
 * PURPOSE:
 *   Provide a bounded three-way merge foundation for source control and
 *   Beyond Compare-style conflict resolution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_MERGE_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_MERGE_H

#include "umicom/developer_productivity/diff.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_MERGE_MAX_BLOCKS 512U

typedef enum UmiDeveloperMergeResolution {
    UMI_DEVELOPER_MERGE_UNRESOLVED = 0,
    UMI_DEVELOPER_MERGE_USE_BASE = 1,
    UMI_DEVELOPER_MERGE_USE_LEFT = 2,
    UMI_DEVELOPER_MERGE_USE_RIGHT = 3,
    UMI_DEVELOPER_MERGE_USE_BOTH = 4
} UmiDeveloperMergeResolution;

typedef struct UmiDeveloperMergeBlock {
    size_t index;
    int conflict;
    char base_text[UMI_DEVELOPER_PRODUCTIVITY_LINE_CAPACITY];
    char left_text[UMI_DEVELOPER_PRODUCTIVITY_LINE_CAPACITY];
    char right_text[UMI_DEVELOPER_PRODUCTIVITY_LINE_CAPACITY];
    UmiDeveloperMergeResolution resolution;
} UmiDeveloperMergeBlock;

typedef struct UmiDeveloperMergeDocument UmiDeveloperMergeDocument;

UmiStatus umi_developer_merge_document_create(
    const char *base_text,
    const char *left_text,
    const char *right_text,
    UmiDeveloperMergeDocument **out_document);

void umi_developer_merge_document_destroy(
    UmiDeveloperMergeDocument *document);

size_t umi_developer_merge_document_block_count(
    const UmiDeveloperMergeDocument *document);

size_t umi_developer_merge_document_conflict_count(
    const UmiDeveloperMergeDocument *document);

UmiStatus umi_developer_merge_document_block_at(
    const UmiDeveloperMergeDocument *document,
    size_t index,
    UmiDeveloperMergeBlock *out_block);

UmiStatus umi_developer_merge_document_resolve(
    UmiDeveloperMergeDocument *document,
    size_t index,
    UmiDeveloperMergeResolution resolution);

UmiStatus umi_developer_merge_document_render(
    const UmiDeveloperMergeDocument *document,
    char *out_text,
    size_t capacity,
    int include_conflict_markers);

#ifdef __cplusplus
}
#endif

#endif
