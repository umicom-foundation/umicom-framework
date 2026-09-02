/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/types.c
 *
 * PURPOSE:
 *   Implement stable diagnostic text for Studio runtime composition records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/types.h"

/*
 * Provide the studio runtime surface kind text operation used by this module and its
 * client applications.
 */
const char *umi_studio_runtime_surface_kind_text(
    UmiStudioRuntimeSurfaceKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_STUDIO_SURFACE_EXPLORER: return "explorer";
        case UMI_STUDIO_SURFACE_EDITOR: return "editor";
        case UMI_STUDIO_SURFACE_OUTLINE: return "outline";
        case UMI_STUDIO_SURFACE_SEARCH: return "search";
        case UMI_STUDIO_SURFACE_PROBLEMS: return "problems";
        case UMI_STUDIO_SURFACE_OUTPUT: return "output";
        case UMI_STUDIO_SURFACE_TERMINAL: return "terminal";
        case UMI_STUDIO_SURFACE_SOURCE_CONTROL: return "source-control";
        case UMI_STUDIO_SURFACE_TEST_EXPLORER: return "test-explorer";
        case UMI_STUDIO_SURFACE_DEBUG: return "debug";
        case UMI_STUDIO_SURFACE_BUILD: return "build";
        case UMI_STUDIO_SURFACE_AI_CHAT: return "ai-chat";
        case UMI_STUDIO_SURFACE_AI_REVIEW: return "ai-review";
        case UMI_STUDIO_SURFACE_AI_APPROVALS: return "ai-approvals";
        case UMI_STUDIO_SURFACE_AI_TOOLS: return "ai-tools";
        case UMI_STUDIO_SURFACE_AI_VALIDATION: return "ai-validation";
        case UMI_STUDIO_SURFACE_AI_MODEL_COMPARISON:
            return "ai-model-comparison";
        default: return "unknown";
    }
}

/*
 * Provide the studio runtime selection kind text operation used by this module and its
 * client applications.
 */
const char *umi_studio_runtime_selection_kind_text(
    UmiStudioRuntimeSelectionKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_STUDIO_SELECTION_NONE: return "none";
        case UMI_STUDIO_SELECTION_PROBLEM: return "problem";
        case UMI_STUDIO_SELECTION_TEST: return "test";
        case UMI_STUDIO_SELECTION_SOURCE_CONTROL: return "source-control";
        case UMI_STUDIO_SELECTION_DEBUG_FRAME: return "debug-frame";
        case UMI_STUDIO_SELECTION_SYMBOL: return "symbol";
        case UMI_STUDIO_SELECTION_DIAGNOSTIC: return "diagnostic";
        case UMI_STUDIO_SELECTION_EDITOR: return "editor";
        case UMI_STUDIO_SELECTION_AI_APPROVAL: return "ai-approval";
        case UMI_STUDIO_SELECTION_AI_PATCH_FILE: return "ai-patch-file";
        default: return "unknown";
    }
}

/*
 * Provide the studio runtime status kind text operation used by this module and its client
 * applications.
 */
const char *umi_studio_runtime_status_kind_text(
    UmiStudioRuntimeStatusKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_STUDIO_STATUS_WORKSPACE: return "workspace";
        case UMI_STUDIO_STATUS_BRANCH: return "branch";
        case UMI_STUDIO_STATUS_PROBLEMS: return "problems";
        case UMI_STUDIO_STATUS_TESTS: return "tests";
        case UMI_STUDIO_STATUS_BUILD: return "build";
        case UMI_STUDIO_STATUS_DEBUG: return "debug";
        case UMI_STUDIO_STATUS_AI: return "ai";
        case UMI_STUDIO_STATUS_LANGUAGE: return "language";
        case UMI_STUDIO_STATUS_ENCODING: return "encoding";
        case UMI_STUDIO_STATUS_CURSOR: return "cursor";
        default: return "unknown";
    }
}

/*
 * Provide the studio runtime close decision text operation used by this module and its
 * client applications.
 */
const char *umi_studio_runtime_close_decision_text(
    UmiStudioRuntimeCloseDecision decision)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (decision) {
        case UMI_STUDIO_CLOSE_ALLOW: return "allow";
        case UMI_STUDIO_CLOSE_CONFIRM_DIRTY: return "confirm-dirty";
        case UMI_STUDIO_CLOSE_CONFIRM_RUNNING: return "confirm-running";
        case UMI_STUDIO_CLOSE_CONFIRM_DIRTY_AND_RUNNING:
            return "confirm-dirty-and-running";
        case UMI_STUDIO_CLOSE_BLOCKED: return "blocked";
        default: return "unknown";
    }
}
