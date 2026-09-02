/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/shell_actions.c
 *
 * PURPOSE:
 *   Implement the shell actions behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework | Federated desktop shell action catalogue
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/shell_actions.h"

#include <stdio.h>
#include <string.h>

typedef enum ActionEnablement {
    ACTION_ALWAYS = 1,
    ACTION_DESIGNER_INACTIVE,
    ACTION_DESIGNER_ACTIVE,
    ACTION_CAN_UNDO,
    ACTION_CAN_REDO,
    ACTION_WINDOW_SELECTED
} ActionEnablement;

static const struct ActionDefinition {
    const char *action_id;
    const char *label;
    const char *tooltip;
    ActionEnablement enablement;
} ACTIONS[] = {
    {UMI_DESKTOP_ACTION_NEW_LAYOUT, "Design Layout",
     "Clone the active protected layout and open the visual designer",
     ACTION_DESIGNER_INACTIVE},
    {UMI_DESKTOP_ACTION_SAVE_LAYOUT, "Save Layout",
     "Commit the current custom layout through Framework",
     ACTION_DESIGNER_ACTIVE},
    {UMI_DESKTOP_ACTION_CANCEL_LAYOUT, "Cancel",
     "Discard this visual layout design transaction",
     ACTION_DESIGNER_ACTIVE},
    {UMI_DESKTOP_ACTION_UNDO, "Undo", "Undo the last layout operation",
     ACTION_CAN_UNDO},
    {UMI_DESKTOP_ACTION_REDO, "Redo", "Redo the last layout operation",
     ACTION_CAN_REDO},
    {UMI_DESKTOP_ACTION_DOCK_LEFT, "Dock Left",
     "Dock the selected window to the left half", ACTION_WINDOW_SELECTED},
    {UMI_DESKTOP_ACTION_DOCK_RIGHT, "Dock Right",
     "Dock the selected window to the right half", ACTION_WINDOW_SELECTED},
    {UMI_DESKTOP_ACTION_DOCK_TOP, "Dock Top",
     "Dock the selected window to the top half", ACTION_WINDOW_SELECTED},
    {UMI_DESKTOP_ACTION_DOCK_BOTTOM, "Dock Bottom",
     "Dock the selected window to the bottom half", ACTION_WINDOW_SELECTED},
    {UMI_DESKTOP_ACTION_FLOAT, "Float",
     "Float the selected window in the active monitor", ACTION_WINDOW_SELECTED},
    {UMI_DESKTOP_ACTION_MONITORS, "Monitors",
     "Inspect the Framework-owned multi-monitor topology", ACTION_ALWAYS}
};

/*
 * Return the number of records represented by desktop shell action without changing their
 * state.
 */
size_t umi_desktop_shell_action_count(void)
{
    return sizeof(ACTIONS) / sizeof(ACTIONS[0]);
}

/*
 * Find desktop shell action while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_desktop_shell_action_at(
    const UmiDesktopLayoutDesigner *designer,
    size_t index,
    UmiDesktopShellAction *out_action)
{
    UmiDesktopLayoutDesignerSnapshot snapshot;
    int first;
    int second;
    int third;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (designer == NULL || out_action == NULL ||
        index >= umi_desktop_shell_action_count())
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_desktop_layout_designer_snapshot(designer, &snapshot) !=
        UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;
    (void)memset(out_action, 0, sizeof(*out_action));
    first = snprintf(out_action->action_id, sizeof(out_action->action_id),
                     "%s", ACTIONS[index].action_id);
    second = snprintf(out_action->label, sizeof(out_action->label), "%s",
                      ACTIONS[index].label);
    third = snprintf(out_action->tooltip, sizeof(out_action->tooltip), "%s",
                     ACTIONS[index].tooltip);
    /* Apply this branch only when its contract condition is satisfied. */
    if (first < 0 || second < 0 || third < 0 ||
        (size_t)first >= sizeof(out_action->action_id) ||
        (size_t)second >= sizeof(out_action->label) ||
        (size_t)third >= sizeof(out_action->tooltip))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Select the behaviour associated with the requested command or state value. */
    switch (ACTIONS[index].enablement) {
        case ACTION_ALWAYS: out_action->enabled = true; break;
        case ACTION_DESIGNER_INACTIVE:
            out_action->enabled = !snapshot.active; break;
        case ACTION_DESIGNER_ACTIVE:
            out_action->enabled = snapshot.active && snapshot.can_commit; break;
        case ACTION_CAN_UNDO:
            out_action->enabled = snapshot.active && snapshot.undo_count > 0U;
            break;
        case ACTION_CAN_REDO:
            out_action->enabled = snapshot.active && snapshot.redo_count > 0U;
            break;
        case ACTION_WINDOW_SELECTED:
            out_action->enabled = snapshot.active &&
                snapshot.selected_window_id[0] != '\0';
            break;
        default: return UMI_STATUS_INVALID_STATE;
    }
    out_action->destructive =
        strcmp(out_action->action_id, UMI_DESKTOP_ACTION_CANCEL_LAYOUT) == 0;
    return UMI_STATUS_OK;
}
