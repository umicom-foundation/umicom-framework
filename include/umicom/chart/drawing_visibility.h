/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/drawing_visibility.h
 * PURPOSE: Hide retained drawings without deleting their geometry or ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CHART_DRAWING_VISIBILITY_H
#define UMICOM_CHART_DRAWING_VISIBILITY_H
#include "umicom/chart/drawing.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Toggle a displayed drawing using its exact pane, ID and revision. hidden
 * must be 0 or 1. A geometry lock does not prevent hiding/showing. Geometry,
 * opaque style, selection and lock remain unchanged. Same visibility is a
 * successful no-op. A stale revision returns BUSY; another pane returns
 * INVALID_STATE. Only the registry is changed, never orders or files. */
UmiStatus UmiChartDrawingSetHidden(UmiChartDrawingRegistry *registry, const char *pane,
    const char *id, uint64_t expectedRevision, int hidden);
/** Change all drawings in one pane as an atomic batch. Bind the action to the
 * registry revision shown to the user; even another pane's intervening edit
 * returns BUSY. Unrelated panes and already-matching rows retain revisions.
 * Unknown future tool names retain their metadata and are included, provided
 * their saved geometry is valid. Empty panes succeed with zero changed rows.
 * On any failure, registry and optional outChanged stay unchanged. A no-op
 * does not allocate a replacement batch or advance a revision.
 * Calls are confined to the registry owner thread, with no callbacks or I/O. */
UmiStatus UmiChartDrawingSetPaneHidden(UmiChartDrawingRegistry *registry, const char *pane,
    uint64_t expectedRegistryRevision, int hidden, size_t *outChanged);
#ifdef __cplusplus
}
#endif
#endif
