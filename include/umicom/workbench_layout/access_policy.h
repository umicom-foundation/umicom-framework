/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/access_policy.h
 *
 * PURPOSE:
 *   Authorise view, edit, delete, share and administration actions using layout ownership, role, lock state and workspace policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_ACCESS_POLICY_H
#define UMICOM_WORKBENCH_LAYOUT_ACCESS_POLICY_H

#include "umicom/workbench_layout/document.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench layout principal data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutPrincipal {
    uint32_t structure_size;
    char user_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char workspace_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    UmiWorkbenchLayoutRole role;
    bool trusted_workspace;
    bool administrator;
} UmiWorkbenchLayoutPrincipal;

/**
 * Represent the workbench layout access decision data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutAccessDecision {
    uint32_t structure_size;
    UmiWorkbenchLayoutAccessAction action;
    bool allowed;
    bool ownership_matched;
    bool workspace_matched;
    bool lock_blocked;
    bool trust_required;
    char reason[UMI_WORKBENCH_LAYOUT_ERROR_CAPACITY];
} UmiWorkbenchLayoutAccessDecision;

/**
 * Represent the workbench layout access policy data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutAccessPolicy {
    uint32_t structure_size;
    bool viewers_may_export;
    bool editors_may_share;
    bool owners_may_delete;
    bool administrators_override_lock;
    bool require_trusted_workspace_for_edit;
    bool require_same_workspace;
} UmiWorkbenchLayoutAccessPolicy;

/**
 * Provide the workbench layout access policy default operation used by this module and its
 * client applications.
 */
UmiWorkbenchLayoutAccessPolicy
umi_workbench_layout_access_policy_default(void);

/**
 * Provide the workbench layout access decide operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_access_decide(
    const UmiWorkbenchLayoutAccessPolicy *policy,
    const UmiWorkbenchLayoutPrincipal *principal,
    const UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchLayoutAccessAction action,
    UmiWorkbenchLayoutAccessDecision *out_decision);

/**
 * Provide the workbench layout access allowed operation used by this module and its client
 * applications.
 */
bool umi_workbench_layout_access_allowed(
    const UmiWorkbenchLayoutAccessPolicy *policy,
    const UmiWorkbenchLayoutPrincipal *principal,
    const UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchLayoutAccessAction action);

/**
 * Provide the workbench layout access action text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_layout_access_action_text(
    UmiWorkbenchLayoutAccessAction action);

/**
 * Provide the workbench layout role text operation used by this module and its client
 * applications.
 */
const char *umi_workbench_layout_role_text(
    UmiWorkbenchLayoutRole role);

#ifdef __cplusplus
}
#endif

#endif
