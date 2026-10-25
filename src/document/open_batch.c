/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/open_batch.c
 *
 * PURPOSE:
 *   Keep multi-file admission, result ownership and cancellation toolkit-neutral.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Keep multi-file admission, result ownership and cancellation toolkit-neutral. */
#include "umicom/document/open_batch.h"
#include <stdlib.h>
#include <string.h>
struct UmiDocumentOpenBatch {
  UmiDocumentOpenItem items[UMI_DOCUMENT_OPEN_BATCH_MAXIMUM];
  UmiStatus admission[UMI_DOCUMENT_OPEN_BATCH_MAXIMUM];
  UmiDocumentCoordinator *coordinator;
  size_t count, next, succeeded, failed, cancelled;
  bool busy;
};
static UmiStatus capture_path(const char *path, char *out) {
  if (path == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t length = 0U;
  while (length < UMI_PATH_CAPACITY && path[length] != '\0')
    ++length;
  if (length == UMI_PATH_CAPACITY)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  if (length == 0U)
    return UMI_STATUS_INVALID_ARGUMENT;
  memcpy(out, path, length + 1U);
  if (!umi_path_is_absolute(out))
    return UMI_STATUS_INVALID_ARGUMENT;
  return UMI_STATUS_OK;
}
UmiStatus UmiDocumentOpenBatchCreate(const char *const *paths, size_t count,
                                     UmiDocumentOpenBatch **out_batch) {
  if (out_batch == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out_batch = NULL;
  if (paths == NULL || count == 0U)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (count > UMI_DOCUMENT_OPEN_BATCH_MAXIMUM)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  UmiDocumentOpenBatch *batch = calloc(1U, sizeof *batch);
  if (batch == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  batch->count = count;
  for (size_t i = 0U; i < count; ++i) {
    batch->admission[i] = capture_path(paths[i], batch->items[i].path);
    batch->items[i].state = UMI_DOCUMENT_OPEN_PENDING;
    batch->items[i].status = UMI_STATUS_UNAVAILABLE;
  }
  *out_batch = batch;
  return UMI_STATUS_OK;
}
void UmiDocumentOpenBatchDestroy(UmiDocumentOpenBatch *batch) { free(batch); }
UmiStatus UmiDocumentOpenBatchInspect(const UmiDocumentOpenBatch *batch,
                                      UmiDocumentOpenBatchSummary *out_summary) {
  if (batch == NULL || out_summary == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out_summary =
      (UmiDocumentOpenBatchSummary){batch->count,  batch->next,      batch->succeeded,
                                    batch->failed, batch->cancelled, batch->count - batch->next,
                                    batch->busy};
  return UMI_STATUS_OK;
}
UmiStatus UmiDocumentOpenBatchAt(const UmiDocumentOpenBatch *batch, size_t index,
                                 UmiDocumentOpenItem *out_item) {
  if (batch == NULL || out_item == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (index >= batch->count)
    return UMI_STATUS_NOT_FOUND;
  *out_item = batch->items[index];
  return UMI_STATUS_OK;
}
UmiStatus UmiDocumentOpenBatchStep(UmiDocumentOpenBatch *batch, UmiDocumentCoordinator *coordinator,
                                   size_t *out_index) {
  if (batch == NULL || coordinator == NULL || out_index == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (batch->busy)
    return UMI_STATUS_BUSY;
  if (batch->coordinator != NULL && batch->coordinator != coordinator)
    return UMI_STATUS_INVALID_STATE;
  if (batch->next == batch->count)
    return UMI_STATUS_NOT_FOUND;
  batch->coordinator = coordinator;
  batch->busy = true;
  const size_t index = batch->next;
  UmiDocumentOpenItem *item = &batch->items[index];
  UmiStatus status = batch->admission[index];
  if (status == UMI_STATUS_OK)
    status =
        umi_document_coordinator_open(coordinator, item->path, item->view_id, sizeof item->view_id);
  item->status = status;
  item->state = status == UMI_STATUS_OK ? UMI_DOCUMENT_OPEN_SUCCEEDED : UMI_DOCUMENT_OPEN_FAILED;
  if (status == UMI_STATUS_OK)
    ++batch->succeeded;
  else {
    ++batch->failed;
    item->view_id[0] = '\0';
  }
  ++batch->next;
  batch->busy = false;
  *out_index = index;
  return UMI_STATUS_OK;
}
UmiStatus UmiDocumentOpenBatchCancel(UmiDocumentOpenBatch *batch) {
  if (batch == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (batch->busy)
    return UMI_STATUS_BUSY;
  while (batch->next < batch->count) {
    UmiDocumentOpenItem *item = &batch->items[batch->next++];
    item->state = UMI_DOCUMENT_OPEN_CANCELLED;
    item->status = UMI_STATUS_CANCELLED;
    ++batch->cancelled;
  }
  return UMI_STATUS_OK;
}
