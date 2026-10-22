/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal_ui/selection.h
 * PURPOSE: Capture a terminal selection before an asynchronous user decision.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_UI_SELECTION_H
#define UMICOM_TERMINAL_UI_SELECTION_H
#include "umicom/terminal_ui/controller.h"
#ifdef __cplusplus
extern "C" {
#endif

/** A transient value belonging to one live controller and its owner thread.
 * It owns no controller, session or file. Do not persist it or reuse it after
 * destroying the controller. Application lifetime checks remain necessary. */
typedef struct UmiTerminalSelection {
    uint64_t controller_revision;
    UmiTerminalSessionSnapshot session;
} UmiTerminalSelection;

/** Capture the active session without changing it. Output is unchanged on
 * failure; an absent or closed active session cannot be captured. */
UmiStatus UmiTerminalSelectionCapture(const UmiTerminalController *controller,
    UmiTerminalSelection *out_selection);

/** Recheck a captured selection before publishing a delayed user decision.
 * Controller revisions reject switching away and back, including recycling a
 * session name. Changes to the selected directory, state or command count also
 * invalidate the decision. Ordinary transcript appends do not invalidate it.
 * Other controller mutations may conservatively invalidate it as well.
 * Returns INVALID_STATE when stale, and INVALID_ARGUMENT for null arguments.
 * This is an identity check, not a lock, authorisation or execution permission. */
UmiStatus UmiTerminalSelectionValidate(const UmiTerminalController *controller,
    const UmiTerminalSelection *selection);
#ifdef __cplusplus
}
#endif
#endif
