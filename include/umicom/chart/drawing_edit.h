/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/drawing_edit.h
 * PURPOSE: Apply explicit version-checked drawing edits to the canonical registry.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CHART_DRAWING_EDIT_H
#define UMICOM_CHART_DRAWING_EDIT_H
#include "umicom/chart/drawing_tools.h"
#ifdef __cplusplus
extern "C" {
#endif
/** All operations are owner-thread confined. pane and expectedRevision bind an
 * action to the displayed record. Changed evidence returns BUSY; another pane
 * returns INVALID_STATE. No operation touches market data, orders or files.
 * No-op locking succeeds without advancing the registry revision. */
UmiStatus UmiChartDrawingSetLocked(UmiChartDrawingRegistry *registry,const char *pane,
    const char *id,uint64_t expectedRevision,int locked);
/** Locked geometry cannot move. Levels use first.value; boxes/rays retain both
 * anchors. Style, identity and selection are preserved. */
UmiStatus UmiChartDrawingSetGeometry(UmiChartDrawingRegistry *registry,const char *pane,
    const char *id,uint64_t expectedRevision,UmiChartPoint first,UmiChartPoint second);
/** Duplicate to an explicitly unused ID, retaining geometry and opaque style.
 * The copy is unselected and unlocked so its anchors can be edited separately;
 * the original remains unchanged, including its lock. newId is copied. */
UmiStatus UmiChartDrawingDuplicate(UmiChartDrawingRegistry *registry,const char *pane,
    const char *id,uint64_t expectedRevision,const char *newId);
#ifdef __cplusplus
}
#endif
#endif
