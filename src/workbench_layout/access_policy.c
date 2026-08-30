/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/access_policy.c
 *
 * PURPOSE:
 *   Implement ownership, workspace, role, lock and trust policy for viewing, editing, deleting, sharing and administering semantic layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/workbench_layout/access_policy.h"

#include <string.h>

#include "internal.h"

static void decision_init(
    UmiWorkbenchLayoutAccessDecision *decision,
    UmiWorkbenchLayoutAccessAction action)
{
    if (decision == NULL) {
        return;
    }
    (void)memset(decision, 0, sizeof(*decision));
    decision->structure_size = sizeof(*decision);
    decision->action = action;
}

static void decision_reason(
    UmiWorkbenchLayoutAccessDecision *decision,
    const char *reason)
{
    if (decision != NULL) {
        (void)umi_workbench_layout_copy_text(
            decision->reason,
            sizeof(decision->reason),
            reason != NULL ? reason : "",
            true);
    }
}

UmiWorkbenchLayoutAccessPolicy
umi_workbench_layout_access_policy_default(void)
{
    UmiWorkbenchLayoutAccessPolicy policy;

    (void)memset(&policy, 0, sizeof(policy));
    policy.structure_size = sizeof(policy);
    policy.viewers_may_export = true;
    policy.editors_may_share = true;
    policy.owners_may_delete = true;
    policy.administrators_override_lock = true;
    policy.require_trusted_workspace_for_edit = true;
    policy.require_same_workspace = true;
    return policy;
}

UmiStatus umi_workbench_layout_access_decide(
    const UmiWorkbenchLayoutAccessPolicy *policy,
    const UmiWorkbenchLayoutPrincipal *principal,
    const UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchLayoutAccessAction action,
    UmiWorkbenchLayoutAccessDecision *out_decision)
{
    UmiWorkbenchLayoutAccessPolicy effective;
    bool owner;
    bool same_workspace;
    bool locked;
    bool mutating;

    if (principal == NULL || document == NULL ||
        out_decision == NULL ||
        principal->structure_size < sizeof(*principal) ||
        !umi_workbench_layout_text_present(principal->user_id) ||
        action < UMI_WORKBENCH_LAYOUT_ACCESS_VIEW ||
        action > UMI_WORKBENCH_LAYOUT_ACCESS_ADMINISTER) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    effective = policy != NULL
        ? *policy
        : umi_workbench_layout_access_policy_default();
    decision_init(out_decision, action);

    owner = document->identity.owner_user_id[0] != '\0' &&
        strcmp(
            principal->user_id,
            document->identity.owner_user_id) == 0;
    same_workspace =
        document->identity.workspace_id[0] == '\0' ||
        strcmp(
            principal->workspace_id,
            document->identity.workspace_id) == 0;
    locked = umi_workbench_layout_document_has_flag(
        document, UMI_WORKBENCH_LAYOUT_DOCUMENT_LOCKED);
    mutating =
        action == UMI_WORKBENCH_LAYOUT_ACCESS_CREATE ||
        action == UMI_WORKBENCH_LAYOUT_ACCESS_EDIT ||
        action == UMI_WORKBENCH_LAYOUT_ACCESS_DELETE ||
        action == UMI_WORKBENCH_LAYOUT_ACCESS_LOCK ||
        action == UMI_WORKBENCH_LAYOUT_ACCESS_ADMINISTER;

    out_decision->ownership_matched = owner;
    out_decision->workspace_matched = same_workspace;

    if (effective.require_same_workspace &&
        !same_workspace &&
        !principal->administrator) {
        decision_reason(
            out_decision,
            "The layout belongs to a different workspace.");
        return UMI_STATUS_OK;
    }

    if (mutating &&
        effective.require_trusted_workspace_for_edit &&
        !principal->trusted_workspace &&
        !principal->administrator) {
        out_decision->trust_required = true;
        decision_reason(
            out_decision,
            "A trusted workspace is required for this layout action.");
        return UMI_STATUS_OK;
    }

    if (locked && mutating &&
        !(principal->administrator &&
          effective.administrators_override_lock) &&
        action != UMI_WORKBENCH_LAYOUT_ACCESS_LOCK) {
        out_decision->lock_blocked = true;
        decision_reason(
            out_decision,
            "The layout is locked against modification.");
        return UMI_STATUS_OK;
    }

    if (principal->administrator) {
        out_decision->allowed = true;
        decision_reason(
            out_decision,
            "An administrator may perform this action.");
        return UMI_STATUS_OK;
    }

    switch (action) {
    case UMI_WORKBENCH_LAYOUT_ACCESS_VIEW:
        out_decision->allowed = true;
        break;

    case UMI_WORKBENCH_LAYOUT_ACCESS_CREATE:
        out_decision->allowed =
            principal->role >= UMI_WORKBENCH_LAYOUT_ROLE_EDITOR;
        break;

    case UMI_WORKBENCH_LAYOUT_ACCESS_EDIT:
        out_decision->allowed =
            owner ||
            principal->role >= UMI_WORKBENCH_LAYOUT_ROLE_EDITOR;
        break;

    case UMI_WORKBENCH_LAYOUT_ACCESS_DELETE:
        out_decision->allowed =
            owner &&
            effective.owners_may_delete &&
            principal->role >= UMI_WORKBENCH_LAYOUT_ROLE_OWNER;
        break;

    case UMI_WORKBENCH_LAYOUT_ACCESS_SHARE:
        out_decision->allowed =
            owner ||
            (effective.editors_may_share &&
             principal->role >=
                 UMI_WORKBENCH_LAYOUT_ROLE_EDITOR);
        break;

    case UMI_WORKBENCH_LAYOUT_ACCESS_LOCK:
        out_decision->allowed =
            owner &&
            principal->role >= UMI_WORKBENCH_LAYOUT_ROLE_OWNER;
        break;

    case UMI_WORKBENCH_LAYOUT_ACCESS_ADMINISTER:
        out_decision->allowed = false;
        break;

    default:
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    decision_reason(
        out_decision,
        out_decision->allowed
            ? "The principal satisfies the layout access policy."
            : "The principal does not satisfy the layout access policy.");
    return UMI_STATUS_OK;
}

bool umi_workbench_layout_access_allowed(
    const UmiWorkbenchLayoutAccessPolicy *policy,
    const UmiWorkbenchLayoutPrincipal *principal,
    const UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchLayoutAccessAction action)
{
    UmiWorkbenchLayoutAccessDecision decision;
    return umi_workbench_layout_access_decide(
               policy,
               principal,
               document,
               action,
               &decision) == UMI_STATUS_OK &&
           decision.allowed;
}

const char *umi_workbench_layout_access_action_text(
    UmiWorkbenchLayoutAccessAction action)
{
    switch (action) {
    case UMI_WORKBENCH_LAYOUT_ACCESS_VIEW: return "view";
    case UMI_WORKBENCH_LAYOUT_ACCESS_CREATE: return "create";
    case UMI_WORKBENCH_LAYOUT_ACCESS_EDIT: return "edit";
    case UMI_WORKBENCH_LAYOUT_ACCESS_DELETE: return "delete";
    case UMI_WORKBENCH_LAYOUT_ACCESS_SHARE: return "share";
    case UMI_WORKBENCH_LAYOUT_ACCESS_LOCK: return "lock";
    case UMI_WORKBENCH_LAYOUT_ACCESS_ADMINISTER:
        return "administer";
    default:
        return "invalid";
    }
}

const char *umi_workbench_layout_role_text(
    UmiWorkbenchLayoutRole role)
{
    switch (role) {
    case UMI_WORKBENCH_LAYOUT_ROLE_VIEWER: return "viewer";
    case UMI_WORKBENCH_LAYOUT_ROLE_EDITOR: return "editor";
    case UMI_WORKBENCH_LAYOUT_ROLE_OWNER: return "owner";
    case UMI_WORKBENCH_LAYOUT_ROLE_ADMINISTRATOR:
        return "administrator";
    default:
        return "invalid";
    }
}
