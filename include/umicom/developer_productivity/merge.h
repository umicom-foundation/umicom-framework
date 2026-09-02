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

/**
 * List the named developer merge resolution values accepted by this public contract.
 */
typedef enum UmiDeveloperMergeResolution {
    UMI_DEVELOPER_MERGE_UNRESOLVED = 0,
    UMI_DEVELOPER_MERGE_USE_BASE = 1,
    UMI_DEVELOPER_MERGE_USE_LEFT = 2,
    UMI_DEVELOPER_MERGE_USE_RIGHT = 3,
    UMI_DEVELOPER_MERGE_USE_BOTH = 4
} UmiDeveloperMergeResolution;

/**
 * Represent the developer merge block data shared with callers of this public contract.
 */
typedef struct UmiDeveloperMergeBlock {
    size_t index;
    int conflict;
    char base_text[UMI_DEVELOPER_PRODUCTIVITY_LINE_CAPACITY];
    char left_text[UMI_DEVELOPER_PRODUCTIVITY_LINE_CAPACITY];
    char right_text[UMI_DEVELOPER_PRODUCTIVITY_LINE_CAPACITY];
    UmiDeveloperMergeResolution resolution;
} UmiDeveloperMergeBlock;

/**
 * Represent the developer merge document data shared with callers of this public contract.
 */
typedef struct UmiDeveloperMergeDocument UmiDeveloperMergeDocument;

/**
 * Initialise developer merge document from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_developer_merge_document_create(
    const char *base_text,
    const char *left_text,
    const char *right_text,
    UmiDeveloperMergeDocument **out_document);

/**
 * Release or reset state held by developer merge document so the same storage can be
 * reused safely.
 */
void umi_developer_merge_document_destroy(
    UmiDeveloperMergeDocument *document);

/**
 * Return the number of records represented by developer merge document block without
 * changing their state.
 */
size_t umi_developer_merge_document_block_count(
    const UmiDeveloperMergeDocument *document);

/**
 * Return the number of records represented by developer merge document conflict without
 * changing their state.
 */
size_t umi_developer_merge_document_conflict_count(
    const UmiDeveloperMergeDocument *document);

/**
 * Find developer merge document block while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_developer_merge_document_block_at(
    const UmiDeveloperMergeDocument *document,
    size_t index,
    UmiDeveloperMergeBlock *out_block);

/**
 * Provide the developer merge document resolve operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_merge_document_resolve(
    UmiDeveloperMergeDocument *document,
    size_t index,
    UmiDeveloperMergeResolution resolution);

/**
 * Provide the developer merge document render operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_merge_document_render(
    const UmiDeveloperMergeDocument *document,
    char *out_text,
    size_t capacity,
    int include_conflict_markers);

#ifdef __cplusplus
}
#endif

#endif
