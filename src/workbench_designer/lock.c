/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/lock.c
 *
 * PURPOSE:
 *   Implement one consistent editability decision for toolbar, commands and
 *   adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/lock.h"
#include "internal.h"


/*
 * Provide the workbench designer lock state build operation used by this module and its
 * client applications.
 */
void umi_workbench_designer_lock_state_build(
    UmiWorkbenchDesignerLockState *state,
    const UmiWorkbenchLayoutDocument *document,
    bool administrator_override)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL) return;
    (void)memset(state, 0, sizeof(*state));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL) {
        state->locked = true;
        (void)umi_workbench_designer_copy_text(state->reason, sizeof(state->reason), "No layout document is open.");
        return;
    }
    state->locked = umi_workbench_layout_document_has_flag(document, UMI_WORKBENCH_LAYOUT_DOCUMENT_LOCKED);
    state->built_in = umi_workbench_layout_document_has_flag(document, UMI_WORKBENCH_LAYOUT_DOCUMENT_BUILT_IN);
    state->read_only = umi_workbench_layout_document_has_flag(document, UMI_WORKBENCH_LAYOUT_DOCUMENT_READ_ONLY);
    state->administrator_override = administrator_override;
    /* Apply this branch only when its contract condition is satisfied. */
    if (state->read_only) {
        (void)umi_workbench_designer_copy_text(state->reason, sizeof(state->reason), "The layout is read-only.");
    } else /* Apply this branch only when its contract condition is satisfied. */ if (state->locked && !administrator_override) {
        (void)umi_workbench_designer_copy_text(state->reason, sizeof(state->reason), "Unlock the layout before editing.");
    } else /* Apply this branch only when its contract condition is satisfied. */ if (state->built_in && !administrator_override) {
        (void)umi_workbench_designer_copy_text(state->reason, sizeof(state->reason), "Clone the built-in template before editing.");
    }
    state->revision = document->version.revision;
}

/*
 * Provide the workbench designer lock allows operation used by this module and its client
 * applications.
 */
bool umi_workbench_designer_lock_allows(
    const UmiWorkbenchDesignerLockState *state,
    UmiWorkbenchDesignerCommandKind command_kind)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL) return false;
    /* Apply this branch only when its contract condition is satisfied. */
    if (command_kind == UMI_WORKBENCH_DESIGNER_COMMAND_UNLOCK_LAYOUT &&
        !state->read_only) return true;
    /* Apply this branch only when its contract condition is satisfied. */
    if (command_kind == UMI_WORKBENCH_DESIGNER_COMMAND_EXPORT ||
        command_kind == UMI_WORKBENCH_DESIGNER_COMMAND_SAVE) return true;
    /* Apply this branch only when its contract condition is satisfied. */
    if (state->administrator_override) return true;
    return !state->locked && !state->read_only && !state->built_in;
}
