/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/document_text_batch_internal.h
 * PURPOSE: Coordinate prepared document-view publication without exposing locks to applications.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_DOCUMENT_TEXT_BATCH_INTERNAL_H
#define UMICOM_UI_DOCUMENT_TEXT_BATCH_INTERNAL_H
#include "umicom/ui/document_view.h"

/* Private bridge for the document coordinator, not an installed SDK contract.
 * Input and its text remain stable until Publish or Abort. The coordinator
 * serializes all document mutations on its owner thread. The lock order is
 * view model first, document store second; no callbacks run while either is held. */
typedef struct UmiUiDocumentTextBatchItem
{
    const UmiUiDocumentViewSnapshot *after;
    uint64_t expected_text_revision;
    const char *before_text;
    size_t before_bytes;
    const char *after_text;
    size_t after_bytes;
} UmiUiDocumentTextBatchItem;
typedef struct UmiUiDocumentTextBatch UmiUiDocumentTextBatch;

/* Acquire validates the entire set and reserves capacity before returning a
 * locked guard. Failure leaves semantic state unchanged and returns no guard;
 * successful reservations may retain reusable storage after a later failure.
 * Until the guard is released, do not call ANY other view-model API, run a
 * callback, or dispatch a native event. Publish cannot allocate or fail. */
UmiStatus UmiUiDocumentTextBatchAcquire(UmiUiDocumentViewModel *model, uint64_t expected_model_revision,
                                        const UmiUiDocumentTextBatchItem *items, size_t count,
                                        UmiUiDocumentTextBatch **out_batch);
void UmiUiDocumentTextBatchPublish(UmiUiDocumentTextBatch *batch);
void UmiUiDocumentTextBatchAbort(UmiUiDocumentTextBatch *batch);
#endif
