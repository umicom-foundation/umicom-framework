/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/types.c
 * PURPOSE: Implement shared project/workspace bounded state helpers.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/types.h"
#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *dst, size_t cap, const char *src)
{
    size_t len;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (dst == NULL || cap == 0U || src == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    len = strlen(src);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (len >= cap) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Apply this branch only when its contract condition is satisfied. */
    if (len > 0U) (void)memcpy(dst, src, len);
    dst[len] = '\0';
    return UMI_STATUS_OK;
}

/*
 * Provide the project workspace state text operation used by this module and its client
 * applications.
 */
const char *umi_project_workspace_state_text(UmiProjectWorkspaceState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
        case UMI_PROJECT_WORKSPACE_READY: return "ready";
        case UMI_PROJECT_WORKSPACE_DEGRADED: return "degraded";
        case UMI_PROJECT_WORKSPACE_BLOCKED: return "blocked";
        case UMI_PROJECT_WORKSPACE_INVALID: return "invalid";
        default: return "unknown";
    }
}

/*
 * Initialise project workspace named state from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_project_workspace_named_state_init(UmiProjectWorkspaceNamedState *value,
const char *id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || id == NULL || id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(value, 0, sizeof(*value));
    value->structure_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_PROJECT_WORKSPACE_API_VERSION;
    value->revision = 1U;
    value->enabled = true;
    value->state = UMI_PROJECT_WORKSPACE_READY;
    status = copy_text(value->id, sizeof(value->id), id);
    return status;
}

/*
 * Check that project workspace named state satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_project_workspace_named_state_validate(const UmiProjectWorkspaceNamedState *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->structure_size < sizeof(*value) ||
    value->api_version != UMI_PROJECT_WORKSPACE_API_VERSION || value->id[0] == '\0')
    return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this operation only while the related capability or state is available. */
    if (value->state < UMI_PROJECT_WORKSPACE_UNKNOWN || value->state > UMI_PROJECT_WORKSPACE_INVALID)
    return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/*
 * Provide the project workspace named state set name operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_workspace_named_state_set_name(UmiProjectWorkspaceNamedState *value,
const char *name)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = copy_text(value->name, sizeof(value->name), name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

/*
 * Provide the project workspace named state set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_project_workspace_named_state_set_detail(UmiProjectWorkspaceNamedState *value,
const char *detail)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = copy_text(value->detail, sizeof(value->detail), detail);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

/*
 * Provide the project workspace named state set state operation used by this module and
 * its client applications.
 */
UmiStatus umi_project_workspace_named_state_set_state(UmiProjectWorkspaceNamedState *value,
UmiProjectWorkspaceState state)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || state < UMI_PROJECT_WORKSPACE_UNKNOWN || state > UMI_PROJECT_WORKSPACE_INVALID)
    return UMI_STATUS_INVALID_ARGUMENT;
    value->state = state;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the project workspace named state same identity operation used by this module
 * and its client applications.
 */
bool umi_project_workspace_named_state_same_identity(const UmiProjectWorkspaceNamedState *left,
const UmiProjectWorkspaceNamedState *right)
{
    return left != NULL && right != NULL && left->id[0] != '\0' && strcmp(left->id, right->id) == 0;
}
