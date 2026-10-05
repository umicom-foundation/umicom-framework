/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/workspace.c
 *
 * PURPOSE:
 *   Implement canonical workspace context validation and mutation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/workspace.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise workspace context from caller-provided values so later operations receive a
 * known state.
 */
void umi_workspace_context_init(UmiWorkspaceContext *context)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL) return;
    memset(context, 0, sizeof(*context));
    context->structure_size = (uint32_t)sizeof(*context);
    context->revision = 1U;
}
/* Check that workspace context satisfies its contract before another service relies on it. */
UmiStatus umi_workspace_context_validate(const UmiWorkspaceContext *context)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->workspace_id, '\0', sizeof(context->workspace_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->profile_id, '\0', sizeof(context->profile_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->root_path, '\0', sizeof(context->root_path)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->perspective_id, '\0', sizeof(context->perspective_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->layout_id, '\0', sizeof(context->layout_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || context->structure_size != sizeof(*context)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->workspace_id, sizeof(context->workspace_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->profile_id, sizeof(context->profile_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->root_path, sizeof(context->root_path))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->perspective_id, sizeof(context->perspective_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->layout_id, sizeof(context->layout_id))) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
/*
 * Copy workspace context into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_workspace_context_copy(UmiWorkspaceContext *destination, const UmiWorkspaceContext *source)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_workspace_context_validate(source) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    *destination = *source;
    return UMI_STATUS_OK;
}
/*
 * Provide the workspace context set workspace id operation used by this module and its
 * client applications.
 */
UmiStatus umi_workspace_context_set_workspace_id(UmiWorkspaceContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->workspace_id, sizeof(context->workspace_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the workspace context set profile id operation used by this module and its
 * client applications.
 */
UmiStatus umi_workspace_context_set_profile_id(UmiWorkspaceContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->profile_id, sizeof(context->profile_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the workspace context set root path operation used by this module and its client
 * applications.
 */
UmiStatus umi_workspace_context_set_root_path(UmiWorkspaceContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->root_path, sizeof(context->root_path), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the workspace context set perspective id operation used by this module and its
 * client applications.
 */
UmiStatus umi_workspace_context_set_perspective_id(UmiWorkspaceContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->perspective_id, sizeof(context->perspective_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the workspace context set layout id operation used by this module and its client
 * applications.
 */
UmiStatus umi_workspace_context_set_layout_id(UmiWorkspaceContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->layout_id, sizeof(context->layout_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the workspace context set trusted operation used by this module and its client
 * applications.
 */
UmiStatus umi_workspace_context_set_trusted(UmiWorkspaceContext *context, bool value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    context->trusted = value;
    context->revision += 1U;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiWorkspaceContextArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x93f6d4976e02c88c);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkspaceContext *)0)->workspace_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkspaceContext *)0)->profile_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkspaceContext *)0)->root_path)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkspaceContext *)0)->perspective_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkspaceContext *)0)->layout_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiWorkspaceContextArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiWorkspaceContext *)0)->workspace_id) - 1U +
        8U + sizeof(((UmiWorkspaceContext *)0)->profile_id) - 1U +
        8U + sizeof(((UmiWorkspaceContext *)0)->root_path) - 1U +
        8U + sizeof(((UmiWorkspaceContext *)0)->perspective_id) - 1U +
        8U + sizeof(((UmiWorkspaceContext *)0)->layout_id) - 1U +
        8U +
        8U;
}
static void UmiWorkspaceContextArchiveWrite(UmiArchiveWriter *writer, const UmiWorkspaceContext *value)
{
    UmiArchiveWriteText(writer, value->workspace_id, sizeof(value->workspace_id));
    UmiArchiveWriteText(writer, value->profile_id, sizeof(value->profile_id));
    UmiArchiveWriteText(writer, value->root_path, sizeof(value->root_path));
    UmiArchiveWriteText(writer, value->perspective_id, sizeof(value->perspective_id));
    UmiArchiveWriteText(writer, value->layout_id, sizeof(value->layout_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->trusted);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiWorkspaceContextArchiveRead(UmiArchiveReader *reader, UmiWorkspaceContext *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->workspace_id, sizeof(value->workspace_id));
    UmiArchiveReadText(reader, value->profile_id, sizeof(value->profile_id));
    UmiArchiveReadText(reader, value->root_path, sizeof(value->root_path));
    UmiArchiveReadText(reader, value->perspective_id, sizeof(value->perspective_id));
    UmiArchiveReadText(reader, value->layout_id, sizeof(value->layout_id));
    value->trusted = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiWorkspaceContextArchiveValidate(const UmiWorkspaceContext *value)
{
    return umi_workspace_context_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_workspace_context_archive_encode, umi_workspace_context_archive_decode,
    UmiWorkspaceContext, UmiWorkspaceContextArchiveSchema, UmiWorkspaceContextArchiveBound, UmiWorkspaceContextArchiveWrite, UmiWorkspaceContextArchiveRead, UmiWorkspaceContextArchiveValidate)
