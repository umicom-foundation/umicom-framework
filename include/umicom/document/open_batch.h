/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/open_batch.h
 *
 * PURPOSE:
 *   Captured, incremental opening of independent local source files.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Captured, incremental opening of independent local source files. */
#ifndef UMICOM_DOCUMENT_OPEN_BATCH_H
#define UMICOM_DOCUMENT_OPEN_BATCH_H
#include "umicom/document/coordinator.h"
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_DOCUMENT_OPEN_BATCH_MAXIMUM 32U
typedef struct UmiDocumentOpenBatch UmiDocumentOpenBatch;
typedef enum UmiDocumentOpenItemState {
  UMI_DOCUMENT_OPEN_PENDING,
  UMI_DOCUMENT_OPEN_SUCCEEDED,
  UMI_DOCUMENT_OPEN_FAILED,
  UMI_DOCUMENT_OPEN_CANCELLED
} UmiDocumentOpenItemState;
typedef struct UmiDocumentOpenItem {
  char path[UMI_PATH_CAPACITY];
  char view_id[UMI_UI_ID_CAPACITY];
  UmiDocumentOpenItemState state;
  UmiStatus status;
} UmiDocumentOpenItem;
typedef struct UmiDocumentOpenBatchSummary {
  size_t count, processed, succeeded, failed, cancelled, pending;
  bool busy;
} UmiDocumentOpenBatchSummary;
/* Capture every path before any I/O. Absolute local paths only: a later current
 * directory change cannot redirect a pending item. Null, empty, relative or
 * overlong members become individual failures when stepped. Whole-request
 * errors (null array, zero count, over limit, allocation) clear out_batch.
 * Paths are copied; callers may release or change their input afterwards. */
UmiStatus UmiDocumentOpenBatchCreate(const char *const *paths, size_t count,
                                     UmiDocumentOpenBatch **out_batch);
/* Owner-thread only. Do not destroy from a provider callback or while Step runs. */
void UmiDocumentOpenBatchDestroy(UmiDocumentOpenBatch *batch);
UmiStatus UmiDocumentOpenBatchInspect(const UmiDocumentOpenBatch *batch,
                                      UmiDocumentOpenBatchSummary *out_summary);
UmiStatus UmiDocumentOpenBatchAt(const UmiDocumentOpenBatch *batch, size_t index,
                                 UmiDocumentOpenItem *out_item);
/* Process exactly one member. OK means a result was recorded, even if opening
 * that file failed; inspect At for its status. NOT_FOUND means no pending item.
 * All steps use the same live coordinator. Keep its store, workbench and
 * provider alive through the call; callbacks must not destroy them.
 * Exact and equivalent paths reuse the coordinator's existing document.
 * A failure does not close successful tabs or discard existing unsaved text.
 * The host may yield between steps but never run this concurrently. Reentry
 * returns BUSY. Only success changes out_index. */
UmiStatus UmiDocumentOpenBatchStep(UmiDocumentOpenBatch *batch, UmiDocumentCoordinator *coordinator,
                                   size_t *out_index);
/* Cancel remaining members only. Already opened files stay open. Reentry during
 * a provider operation returns BUSY; the admitted read cannot be undone here. */
UmiStatus UmiDocumentOpenBatchCancel(UmiDocumentOpenBatch *batch);
#ifdef __cplusplus
}
#endif
#endif
