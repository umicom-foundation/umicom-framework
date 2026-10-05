/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/project.c
 *
 * PURPOSE:
 *   Implement canonical project context validation and mutation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/project.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise project context from caller-provided values so later operations receive a
 * known state.
 */
void umi_project_context_init(UmiProjectContext *context)
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
/* Check that project context satisfies its contract before another service relies on it. */
UmiStatus umi_project_context_validate(const UmiProjectContext *context)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->project_id, '\0', sizeof(context->project_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->repository_id, '\0', sizeof(context->repository_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->root_path, '\0', sizeof(context->root_path)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->target_id, '\0', sizeof(context->target_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->configuration_id, '\0', sizeof(context->configuration_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->language_id, '\0', sizeof(context->language_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || context->structure_size != sizeof(*context)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->project_id, sizeof(context->project_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->repository_id, sizeof(context->repository_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->root_path, sizeof(context->root_path))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->target_id, sizeof(context->target_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->configuration_id, sizeof(context->configuration_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->language_id, sizeof(context->language_id))) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
/*
 * Copy project context into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_project_context_copy(UmiProjectContext *destination, const UmiProjectContext *source)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_project_context_validate(source) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    *destination = *source;
    return UMI_STATUS_OK;
}
/*
 * Provide the project context set project id operation used by this module and its client
 * applications.
 */
UmiStatus umi_project_context_set_project_id(UmiProjectContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->project_id, sizeof(context->project_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the project context set repository id operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_context_set_repository_id(UmiProjectContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->repository_id, sizeof(context->repository_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the project context set root path operation used by this module and its client
 * applications.
 */
UmiStatus umi_project_context_set_root_path(UmiProjectContext *context, const char *value)
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
 * Provide the project context set target id operation used by this module and its client
 * applications.
 */
UmiStatus umi_project_context_set_target_id(UmiProjectContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->target_id, sizeof(context->target_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the project context set configuration id operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_context_set_configuration_id(UmiProjectContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->configuration_id, sizeof(context->configuration_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the project context set language id operation used by this module and its client
 * applications.
 */
UmiStatus umi_project_context_set_language_id(UmiProjectContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->language_id, sizeof(context->language_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiProjectContextArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x8407b5437894b29c);
    schema = (schema ^ (uint64_t)sizeof(((UmiProjectContext *)0)->project_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiProjectContext *)0)->repository_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiProjectContext *)0)->root_path)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiProjectContext *)0)->target_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiProjectContext *)0)->configuration_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiProjectContext *)0)->language_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiProjectContextArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiProjectContext *)0)->project_id) - 1U +
        8U + sizeof(((UmiProjectContext *)0)->repository_id) - 1U +
        8U + sizeof(((UmiProjectContext *)0)->root_path) - 1U +
        8U + sizeof(((UmiProjectContext *)0)->target_id) - 1U +
        8U + sizeof(((UmiProjectContext *)0)->configuration_id) - 1U +
        8U + sizeof(((UmiProjectContext *)0)->language_id) - 1U +
        8U;
}
static void UmiProjectContextArchiveWrite(UmiArchiveWriter *writer, const UmiProjectContext *value)
{
    UmiArchiveWriteText(writer, value->project_id, sizeof(value->project_id));
    UmiArchiveWriteText(writer, value->repository_id, sizeof(value->repository_id));
    UmiArchiveWriteText(writer, value->root_path, sizeof(value->root_path));
    UmiArchiveWriteText(writer, value->target_id, sizeof(value->target_id));
    UmiArchiveWriteText(writer, value->configuration_id, sizeof(value->configuration_id));
    UmiArchiveWriteText(writer, value->language_id, sizeof(value->language_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiProjectContextArchiveRead(UmiArchiveReader *reader, UmiProjectContext *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->project_id, sizeof(value->project_id));
    UmiArchiveReadText(reader, value->repository_id, sizeof(value->repository_id));
    UmiArchiveReadText(reader, value->root_path, sizeof(value->root_path));
    UmiArchiveReadText(reader, value->target_id, sizeof(value->target_id));
    UmiArchiveReadText(reader, value->configuration_id, sizeof(value->configuration_id));
    UmiArchiveReadText(reader, value->language_id, sizeof(value->language_id));
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiProjectContextArchiveValidate(const UmiProjectContext *value)
{
    return umi_project_context_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_project_context_archive_encode, umi_project_context_archive_decode,
    UmiProjectContext, UmiProjectContextArchiveSchema, UmiProjectContextArchiveBound, UmiProjectContextArchiveWrite, UmiProjectContextArchiveRead, UmiProjectContextArchiveValidate)
