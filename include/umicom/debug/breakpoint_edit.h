/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/breakpoint_edit.h
 * PURPOSE: Bind explicit source breakpoint edits to copied workspace evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_BREAKPOINT_EDIT_H
#define UMICOM_DEBUG_BREAKPOINT_EDIT_H
#include "umicom/debug/selection.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDebugBreakpointSettings {
    int enabled;
    char condition[512];
    char logMessage[512];
} UmiDebugBreakpointSettings;
typedef struct UmiDebugBreakpointEdit UmiDebugBreakpointEdit;
typedef struct UmiDebugBreakpointChange {
    UmiDebugBreakpointSnapshot before, after;
    bool changed, removed;
} UmiDebugBreakpointChange;
/** Copy strings of at most 511 bytes each, without evaluating expressions.
 * NULL text, overlong input or enabled other than 0/1 is rejected.
 * Failure leaves out unchanged. Empty text clears the corresponding property. */
UmiStatus UmiDebugBreakpointSettingsInit(UmiDebugBreakpointSettings *out,
    int enabled, const char *condition, const char *logMessage);
/** Capture a copied row, not a borrowed registry pointer. The edit can outlive
 * its workspace. Invalid source location or index returns an error and NULL. */
UmiStatus UmiDebugBreakpointEditCapture(UmiDebugWorkspace *workspace, size_t index,
    UmiDebugBreakpointEdit **out);
void UmiDebugBreakpointEditDestroy(UmiDebugBreakpointEdit *edit);
UmiStatus UmiDebugBreakpointEditRead(const UmiDebugBreakpointEdit *edit,
    UmiDebugBreakpointSnapshot *out);
/** Compare owner, breakpoint, session and configuration generations. BUSY
 * rejects removal/reuse, changed rows and a replacement workspace, including
 * identical IDs. Thread, frame, watch and console refreshes alone do not
 * invalidate a source property edit. Serialize all access on the owner thread. */
UmiStatus UmiDebugBreakpointEditValidate(UmiDebugWorkspace *workspace,
    const UmiDebugBreakpointEdit *edit);
/** Apply only enabled/condition/log message; identity, session and location
 * remain unchanged. A real edit clears adapter verification. A no-op preserves
 * revisions. Remove deletes the selected breakpoint on explicit request.
 * These are in-memory desired-state operations: no adapter, disk or debuggee
 * is contacted. The host must synchronize and report that separate outcome.
 * out is required, zero on failure, and must not overlap input storage. */
UmiStatus UmiDebugBreakpointEditApply(UmiDebugWorkspace *workspace,
    const UmiDebugBreakpointEdit *edit, const UmiDebugBreakpointSettings *settings,
    UmiDebugBreakpointChange *out);
UmiStatus UmiDebugBreakpointEditRemove(UmiDebugWorkspace *workspace,
    const UmiDebugBreakpointEdit *edit, UmiDebugBreakpointChange *out);
#ifdef __cplusplus
}
#endif
#endif
