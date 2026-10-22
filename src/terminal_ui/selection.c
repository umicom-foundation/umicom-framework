/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal_ui/selection.c
 * PURPOSE: Recheck a captured terminal without retaining a session pointer.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal_ui/selection.h"
#include <string.h>

UmiStatus UmiTerminalSelectionCapture(const UmiTerminalController *controller,
    UmiTerminalSelection *out_selection)
{
    if (controller == NULL || out_selection == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiTerminalControllerSnapshot model;
    UmiStatus status = umi_terminal_controller_snapshot(controller, &model);
    if (status != UMI_STATUS_OK) return status;
    UmiTerminalSession *session = umi_terminal_controller_active_session(controller);
    if (session == NULL || model.active_session_id[0] == '\0') return UMI_STATUS_NOT_FOUND;
    UmiTerminalSelection captured = {0};
    status = umi_terminal_session_snapshot(session, &captured.session);
    if (status != UMI_STATUS_OK) return status;
    if (captured.session.state == UMI_TERMINAL_CLOSED ||
        strcmp(model.active_session_id, captured.session.session_id) != 0)
        return UMI_STATUS_INVALID_STATE;
    captured.controller_revision = model.revision;
    *out_selection = captured;
    return UMI_STATUS_OK;
}

UmiStatus UmiTerminalSelectionValidate(const UmiTerminalController *controller,
    const UmiTerminalSelection *selection)
{
    if (controller == NULL || selection == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiTerminalSelection current;
    if (UmiTerminalSelectionCapture(controller, &current) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_STATE;
    /* Compare fixed arrays as bytes: callers cannot make validation read past
     * a malformed capture. Padding and transcript length are not identity. */
    if (current.controller_revision != selection->controller_revision ||
        memcmp(current.session.session_id, selection->session.session_id,
            sizeof current.session.session_id) != 0 ||
        memcmp(current.session.working_directory, selection->session.working_directory,
            sizeof current.session.working_directory) != 0 ||
        current.session.state != selection->session.state ||
        current.session.commands_executed != selection->session.commands_executed)
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
