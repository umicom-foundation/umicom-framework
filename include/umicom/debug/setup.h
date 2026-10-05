/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/setup.h
 * PURPOSE: Save desired debugger settings separately from process and adapter state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_SETUP_H
#define UMICOM_DEBUG_SETUP_H
#include "umicom/debug/workspace.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_DEBUG_SETUP_CAPACITY 64U
    typedef struct UmiDebugSetup UmiDebugSetup;
    typedef struct UmiDebugSetupReview UmiDebugSetupReview;
    typedef struct UmiDebugSetupBreakpoint
    {
        char source[1024], condition[512], logMessage[512];
        uint32_t line, column;
        int enabled;
    } UmiDebugSetupBreakpoint;
    typedef struct UmiDebugSetupWatch
    {
        char expression[1024];
        int enabled;
    } UmiDebugSetupWatch;
    typedef struct UmiDebugSetupSummary
    {
        char title[256];
        size_t breakpoints, watches;
    } UmiDebugSetupSummary;
    /* An owned setup contains at most 64 source locations and 64 expressions.
 * It never contains adapter executables, credentials, process IDs, evaluated
 * values or verification claims. Creation clears out on refusal. All access
 * is serialized by the caller; copies may be transferred to a file worker. */
    UmiStatus UmiDebugSetupCreate(const char *title, UmiDebugSetup **out);
    void UmiDebugSetupDestroy(UmiDebugSetup *setup);
    UmiStatus UmiDebugSetupCopy(const UmiDebugSetup *setup, UmiDebugSetup **out);
    UmiStatus UmiDebugSetupInspect(const UmiDebugSetup *setup, UmiDebugSetupSummary *out);
    /* Append copied, validated UTF-8 settings without evaluating expressions.
 * Source strings are adapter locations, not proof that a file exists. Lines
 * are one-based; zero column leaves the column unspecified. Identical source,
 * line and column entries are refused. Watch ordering and duplicates remain
 * intentional. A refusal leaves the setup unchanged. */
    UmiStatus UmiDebugSetupAddBreakpoint(UmiDebugSetup *setup, const UmiDebugSetupBreakpoint *value);
    UmiStatus UmiDebugSetupAddWatch(UmiDebugSetup *setup, const UmiDebugSetupWatch *value);
    UmiStatus UmiDebugSetupBreakpointAt(const UmiDebugSetup *setup, size_t index,
                                        UmiDebugSetupBreakpoint *out);
    UmiStatus UmiDebugSetupWatchAt(const UmiDebugSetup *setup, size_t index, UmiDebugSetupWatch *out);
    /* Capture desired settings on the workspace owner thread. Larger collections
 * are refused in full rather than silently truncated. The copy may outlive
 * the workspace. No debugger request, evaluation or filesystem access occurs. */
    UmiStatus UmiDebugSetupCapture(UmiDebugWorkspace *workspace, const char *title, UmiDebugSetup **out);
    /* A review owns both the current and proposed settings plus owner/generation
 * evidence. Before/After return borrowed immutable copies valid until Destroy.
 * Retained reviews do not keep their original workspace alive. */
    UmiStatus UmiDebugSetupReviewCreate(UmiDebugWorkspace *workspace, const UmiDebugSetup *proposed,
                                        UmiDebugSetupReview **out);
    void UmiDebugSetupReviewDestroy(UmiDebugSetupReview *review);
    const UmiDebugSetup *UmiDebugSetupReviewBefore(const UmiDebugSetupReview *review);
    const UmiDebugSetup *UmiDebugSetupReviewAfter(const UmiDebugSetupReview *review);
    /* Replace BOTH collections only when the reviewed owner, controller, session,
 * configuration and settings generations still match. Preparation allocates
 * complete candidate registries; publication cannot leave only one replaced.
 * Existing borrowed registry addresses stay valid. All restored records get
 * fresh local IDs/revisions, no session, and no adapter/evaluation confirmation.
 * Only idle/terminated/failed controllers are accepted. Native hosts must ALSO
 * exclude their own active/pending runtime before calling: this model cannot
 * infer another process owner's liveness. Serialize access; no callbacks/I/O.
 * A successful apply consumes its generation evidence, even for empty sets. */
    UmiStatus UmiDebugSetupReviewValidate(UmiDebugWorkspace *workspace, const UmiDebugSetupReview *review);
    UmiStatus UmiDebugSetupReviewApply(UmiDebugWorkspace *workspace, const UmiDebugSetupReview *review);
#ifdef __cplusplus
}
#endif
#endif
