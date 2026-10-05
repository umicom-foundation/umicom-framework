/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/context_changes.h
 * PURPOSE: Publish related context keys as one revision-checked UI state change.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_CONTEXT_CHANGES_H
#define UMICOM_UI_CONTEXT_CHANGES_H
#include "umicom/ui/context.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiUiContextChangeKind {
    UMI_UI_CONTEXT_CHANGE_SET = 1,
    UMI_UI_CONTEXT_CHANGE_REMOVE = 2
} UmiUiContextChangeKind;
/** SET uses the typed value; REMOVE uses only value.key. Strings must be
 * terminated within their fixed arrays. Each key may appear only once. */
typedef struct UmiUiContextChange {
    UmiUiContextChangeKind operation;
    UmiUiContextSnapshot value;
} UmiUiContextChange;

/* A missing key has found == 0 and a zeroed value. Present typed values are
 * independent copies that remain readable after the live store changes. */
typedef struct UmiUiContextObservation {
    int found;
    UmiUiContextSnapshot value;
} UmiUiContextObservation;

/* Observe up to UMI_UI_CONTEXT_MAX named keys under one lock, in input order.
 * Repeated keys are allowed. The returned revision belongs to the whole read,
 * avoiding a mix of values from different edits. Empty reads return a revision.
 * On failure, output rows and optional out_revision remain unchanged. Keep
 * input strings readable for the call; input and output storage must not overlap. */
UmiStatus UmiUiContextReadKeys(const UmiUiContextStore *store,
    const char *const *keys, size_t count, UmiUiContextObservation *out_items,
    uint64_t *out_revision);

/** Apply at most UMI_UI_CONTEXT_MAX changes under one store lock. Validate and
 * stage the complete candidate before publishing it. Removals free capacity
 * before additions, regardless of input order. A missing removal returns
 * NOT_FOUND; duplicate keys return ALREADY_EXISTS. Any failure preserves the
 * complete store, its revision and optional out_revision.
 * expected_revision must equal the current store revision. A successful
 * nonempty change set advances it once; an empty set only checks the revision.
 * Counter exhaustion refuses mutation. This calls no observers or commands.
 * Keep inputs readable and unchanged for the call; destruction needs external
 * lifetime coordination, like the existing context-store operations. */
UmiStatus UmiUiContextApplyChanges(UmiUiContextStore *store, uint64_t expected_revision,
    const UmiUiContextChange *changes, size_t count, uint64_t *out_revision);
#ifdef __cplusplus
}
#endif
#endif
