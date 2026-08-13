/* Umicom Framework unified-diff document. Created by Sammy Hegab, Umicom Foundation. MIT. */
#ifndef UMICOM_VCS_DIFF_DOCUMENT_H
#define UMICOM_VCS_DIFF_DOCUMENT_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/vcs/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiVcsDiffDocument UmiVcsDiffDocument;
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
UmiStatus umi_vcs_diff_document_create(UmiVcsDiffDocument **out_document);
void umi_vcs_diff_document_destroy(UmiVcsDiffDocument *document);
void umi_vcs_diff_document_clear(UmiVcsDiffDocument *document);
UmiStatus umi_vcs_diff_document_parse(UmiVcsDiffDocument *document, const char *path, int staged, const char *unified_diff);
UmiStatus umi_vcs_diff_document_snapshot(const UmiVcsDiffDocument *document, UmiVcsDiffDocumentSnapshot *out_snapshot);
size_t umi_vcs_diff_document_line_count(const UmiVcsDiffDocument *document);
const UmiVcsDiffLine *umi_vcs_diff_document_line_at(const UmiVcsDiffDocument *document, size_t index);
#ifdef __cplusplus
}
#endif
#endif
