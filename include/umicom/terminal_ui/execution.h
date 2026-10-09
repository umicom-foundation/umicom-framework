/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal_ui/execution.h
 * PURPOSE: Route background terminal commands through the shared controller and history.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TERMINAL_UI_EXECUTION_H
#define UMICOM_TERMINAL_UI_EXECUTION_H
#include "umicom/terminal/execution.h"
#include "umicom/terminal_ui/controller.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /** Identify the originating session even when the user selects another tab. */
    typedef struct UmiTerminalControllerExecution
    {
        char session_id[UMI_TERMINAL_ID_CAPACITY];
        char working_directory[UMI_TERMINAL_PATH_CAPACITY];
        char command[UMI_TERMINAL_COMMAND_CAPACITY];
        UmiTerminalExecutionSnapshot execution;
    } UmiTerminalControllerExecution;

    /** Arm only the next controller Execute call for background submission.
 * Call on the controller's owning thread immediately before the application's
 * normal command registry dispatch, then Disarm even if policy denies dispatch.
 * This does not grant permission to execute. A controller owns one pending
 * background command at a time, regardless of the selected terminal tab. */
    UmiStatus UmiTerminalControllerArmBackground(UmiTerminalController *controller);
    /** Report the one-shot dispatch mode so a command handler can choose its
 * explicit timeout without changing command text or bypassing policy checks. */
    int UmiTerminalControllerBackgroundArmed(const UmiTerminalController *controller);
    /** Clear an unused arm without affecting an already submitted command. */
    void UmiTerminalControllerDisarmBackground(UmiTerminalController *controller);
    /** Report pending publication without waiting or consuming completion. */
    int UmiTerminalControllerJobPending(const UmiTerminalController *controller);
    /** Poll the originating session, record completed history once, and return a
 * copied result. The active tab is never used to identify the running job. */
    UmiStatus UmiTerminalControllerPollJob(UmiTerminalController *controller,
                                           UmiTerminalControllerExecution *out_snapshot);
    /** Request Stop for the controller's pending command, even from another tab.
 * This operation never starts another program and cannot cancel unrelated jobs. */
    UmiStatus UmiTerminalControllerStopJob(UmiTerminalController *controller);
#ifdef __cplusplus
}
#endif
#endif
