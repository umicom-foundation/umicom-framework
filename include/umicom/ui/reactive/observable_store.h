/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/observable_store.h
 *
 * PURPOSE:
 *   Hold bounded revisioned UI state keyed by canonical property paths.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_OBSERVABLE_STORE_H
#define UMICOM_UI_REACTIVE_OBSERVABLE_STORE_H
#include "umicom/ui/reactive/types.h"
#include "umicom/ui/reactive/batch_update.h"
#include "umicom/ui/reactive/change_event.h"
#include "umicom/ui/reactive/change_transaction.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive observable store data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveObservableStore {
    UmiUiReactiveKeyValue items[UMI_UI_REACTIVE_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiUiReactiveObservableStore;
/**
 * Initialise ui reactive observable store from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_observable_store_init(UmiUiReactiveObservableStore *item);
/**
 * Provide the ui reactive observable store put operation used by this module and its
 * client applications.
 */
/* Caller-serialized operation. Nonempty keys must terminate within PATH_CAPACITY.
 * Malformed input is INVALID_ARGUMENT; corrupt active rows/count are INVALID_STATE.
 * Full insertion or exhausted revision returns CAPACITY_EXCEEDED. Every refusal
 * preserves the store. Existing rows remain replaceable at full row capacity.
 * Source values may borrow a complete row; the operation stages their payload.
 * Successful updates advance both store and changed-row revisions exactly once. */
UmiStatus umi_ui_reactive_observable_store_put(UmiUiReactiveObservableStore *item,const char *key,const UmiUiValue *value);
/**
 * Provide the ui reactive observable store get operation used by this module and its
 * client applications.
 */
/* Copy an accepted value to caller storage distinct from the store. Refusal
 * leaves output unchanged. Reads remain available at UINT64_MAX revision.
 * Callers serialize reads with writes; this public value object has no lock. */
UmiStatus umi_ui_reactive_observable_store_get(const UmiUiReactiveObservableStore *item,const char *key,UmiUiValue *out_value);

/** Apply a complete set of edits against one observed store revision.
 * Begin an ACTIVE change transaction with the revision sent to the client.
 * Each event must name a distinct existing key and include its previous
 * value. The sequence field is transport evidence; the store owns revisions.
 * All admitted edits use one new revision, including their changed rows.
 * Empty commits publish evidence at the current revision.
 *
 * The host authorizes the edits, uses the correct per-session store and
 * serializes the call. Store, transaction, events and report are disjoint.
 * A refusal leaves the store, transaction and report intact. */
UmiStatus umi_ui_reactive_observable_store_apply(UmiUiReactiveObservableStore *store,
    UmiUiReactiveChangeTransaction *transaction, const UmiUiReactiveChangeEvent *events,
    size_t count, UmiUiReactiveBatchUpdate *out_batch);

typedef struct UmiUiReactiveStoreRead {
    uint64_t observed_revision;
    size_t row_count;
} UmiUiReactiveStoreRead;

/** Copy the latest changed values for the exact keys selected by a view.
 * Use revision zero for the first read. Later reads use observed_revision
 * from the same store lifetime and the same key set. Results follow requested
 * key order and include each key at most once, with its newest active value.
 * A successful empty result carries the current store revision.
 *
 * Callers select the keys allowed for their view and serialize this entire
 * operation with all store access. An unknown key, duplicate selection, future
 * revision or insufficient capacity leaves rows and out_read unchanged.
 * All inputs and outputs must be disjoint. NULL rows is allowed at zero
 * capacity. Only the successful row_count slots are written.
 */
UmiStatus umi_ui_reactive_observable_store_read_changes(const UmiUiReactiveObservableStore *store,
    uint64_t known_revision, const char *const *keys, size_t key_count,
    UmiUiReactiveKeyValue *rows, size_t capacity, UmiUiReactiveStoreRead *out_read);
#ifdef __cplusplus
}
#endif
#endif
