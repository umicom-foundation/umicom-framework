/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/selection.h
 * PURPOSE: Bind copied debugger rows to their workspace and registry generations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_SELECTION_H
#define UMICOM_DEBUG_SELECTION_H
#include "umicom/debug/workspace.h"
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
/** Independent generations prevent a maximum/sum revision from hiding changes
 * to a smaller counter. Read on the workspace owner thread, with no concurrent
 * registry writes. Console output is excluded because it is not a detail row. */
typedef struct UmiDebugViewStamp {
    uint64_t owner, selection, controller, configurations, sessions, threads;
    uint64_t frames, scopes, variables, watches, breakpoints;
    char selectedThread[128], selectedFrame[128], selectedScope[128];
} UmiDebugViewStamp;
UmiStatus UmiDebugWorkspaceViewStamp(UmiDebugWorkspace *workspace, UmiDebugViewStamp *out);
bool UmiDebugViewStampEqual(const UmiDebugViewStamp *left, const UmiDebugViewStamp *right);
typedef enum UmiDebugSelectionKind { UMI_DEBUG_SELECT_THREAD=1, UMI_DEBUG_SELECT_FRAME=2 } UmiDebugSelectionKind;
typedef struct UmiDebugSelection UmiDebugSelection;
typedef struct UmiDebugSelectionSnapshot {
    UmiDebugSelectionKind kind;
    UmiDebugThreadSnapshot thread;
    UmiDebugStackFrameSnapshot frame; /* Zero for a thread row. */
    char sourceBase[1024]; /* Copied session launch directory, or empty if unavailable. */
} UmiDebugSelectionSnapshot;
/** Capture an owned row from current workspace ordering. Frame indexes are
 * relative to the selected thread. A frame must belong to a retained stopped
 * thread. A thread row may describe a running thread; action policy is separate.
 * Capture performs normal workspace selection repair but sends no DAP request.
 * On failure *out is NULL. The capture may outlive its workspace. */
UmiStatus UmiDebugSelectionCapture(UmiDebugWorkspace *workspace, UmiDebugSelectionKind kind,
    size_t index, UmiDebugSelection **out);
void UmiDebugSelectionDestroy(UmiDebugSelection *selection);
UmiStatus UmiDebugSelectionRead(const UmiDebugSelection *selection, UmiDebugSelectionSnapshot *out);
/** Validate immediately before a host action, on the same owner thread.
 * BUSY means selection, session, thread/frame records, configuration or
 * controller changed, including removal followed by reuse of the same ID.
 * Direct variable/watch registry edits refresh presentation without invalidating
 * a navigation row. Workspace actions conservatively invalidate it. This validates
 * evidence, not process liveness, user authority or whether the source changed
 * since compilation. Failure leaves out unchanged; out may be NULL. */
UmiStatus UmiDebugSelectionValidate(UmiDebugWorkspace *workspace,
    const UmiDebugSelection *selection, UmiDebugSelectionSnapshot *out);
#ifdef __cplusplus
}
#endif
#endif
