/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/diff_document.h
 *
 * PURPOSE:
 *   Publish the public diff document contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_DIFF_DOCUMENT_H
#define UMICOM_VCS_DIFF_DOCUMENT_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/vcs/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the vcs diff document data shared with callers of this public contract.
 */
typedef struct UmiVcsDiffDocument UmiVcsDiffDocument;
/**
 * Represent the vcs diff document snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsDiffDocumentSnapshot {
    char path[UMI_VCS_PATH_CAPACITY];
    int staged;
    int binary;
    size_t lines;
    size_t additions;
    size_t deletions;
    size_t hunks;
    uint64_t revision;
} UmiVcsDiffDocumentSnapshot;
/**
 * Initialise vcs diff document from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_vcs_diff_document_create(UmiVcsDiffDocument **out_document);
/**
 * Release or reset state held by vcs diff document so the same storage can be reused
 * safely.
 */
void umi_vcs_diff_document_destroy(UmiVcsDiffDocument *document);
/**
 * Release or reset state held by vcs diff document so the same storage can be reused
 * safely.
 */
void umi_vcs_diff_document_clear(UmiVcsDiffDocument *document);
/**
 * Read vcs diff document into validated module state and return a status when input cannot
 * be used.
 */
UmiStatus umi_vcs_diff_document_parse(UmiVcsDiffDocument *document, const char *path, int staged, const char *unified_diff);
/**
 * Provide the vcs diff document snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_diff_document_snapshot(const UmiVcsDiffDocument *document, UmiVcsDiffDocumentSnapshot *out_snapshot);
/**
 * Return the number of records represented by vcs diff document line without changing
 * their state.
 */
size_t umi_vcs_diff_document_line_count(const UmiVcsDiffDocument *document);
/**
 * Find vcs diff document line while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiVcsDiffLine *umi_vcs_diff_document_line_at(const UmiVcsDiffDocument *document, size_t index);
#ifdef __cplusplus
}
#endif
#endif
