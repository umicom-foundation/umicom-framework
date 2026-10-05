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
#include "../../base/value_archive_internal.h"
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
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->name, '\0', sizeof(value->name)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->detail, '\0', sizeof(value->detail)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

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

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiProjectWorkspaceNamedStateArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x42101df8fcf3470f);
    schema = (schema ^ (uint64_t)sizeof(((UmiProjectWorkspaceNamedState *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiProjectWorkspaceNamedState *)0)->name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiProjectWorkspaceNamedState *)0)->detail)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiProjectWorkspaceNamedStateArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U + sizeof(((UmiProjectWorkspaceNamedState *)0)->id) - 1U +
        8U + sizeof(((UmiProjectWorkspaceNamedState *)0)->name) - 1U +
        8U + sizeof(((UmiProjectWorkspaceNamedState *)0)->detail) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiProjectWorkspaceNamedStateArchiveWrite(UmiArchiveWriter *writer, const UmiProjectWorkspaceNamedState *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->api_version);
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteText(writer, value->detail, sizeof(value->detail));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->flags);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->priority);
    UmiArchiveWriteSigned(writer, (int64_t)value->state);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
}
static void UmiProjectWorkspaceNamedStateArchiveRead(UmiArchiveReader *reader, UmiProjectWorkspaceNamedState *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    value->api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    UmiArchiveReadText(reader, value->detail, sizeof(value->detail));
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->flags = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->priority = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->state = (UmiProjectWorkspaceState)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiProjectWorkspaceNamedStateArchiveValidate(const UmiProjectWorkspaceNamedState *value)
{
    return umi_project_workspace_named_state_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_project_workspace_named_state_archive_encode, umi_project_workspace_named_state_archive_decode,
    UmiProjectWorkspaceNamedState, UmiProjectWorkspaceNamedStateArchiveSchema, UmiProjectWorkspaceNamedStateArchiveBound, UmiProjectWorkspaceNamedStateArchiveWrite, UmiProjectWorkspaceNamedStateArchiveRead, UmiProjectWorkspaceNamedStateArchiveValidate)
