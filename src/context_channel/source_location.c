/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/source_location.c
 *
 * PURPOSE:
 *   Implement canonical source location context validation and mutation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/source_location.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise source location context from caller-provided values so later operations
 * receive a known state.
 */
void umi_source_location_context_init(UmiSourceLocationContext *context)
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
/*
 * Check that source location context satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_source_location_context_validate(const UmiSourceLocationContext *context)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->workspace_id, '\0', sizeof(context->workspace_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->file_path, '\0', sizeof(context->file_path)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->symbol, '\0', sizeof(context->symbol)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || context->structure_size != sizeof(*context)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->workspace_id, sizeof(context->workspace_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->file_path, sizeof(context->file_path))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->symbol, sizeof(context->symbol))) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
/*
 * Copy source location context into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_source_location_context_copy(UmiSourceLocationContext *destination, const UmiSourceLocationContext *source)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_source_location_context_validate(source) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    *destination = *source;
    return UMI_STATUS_OK;
}
/*
 * Provide the source location context set workspace id operation used by this module and
 * its client applications.
 */
UmiStatus umi_source_location_context_set_workspace_id(UmiSourceLocationContext *context, const char *value)
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
 * Provide the source location context set file path operation used by this module and its
 * client applications.
 */
UmiStatus umi_source_location_context_set_file_path(UmiSourceLocationContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->file_path, sizeof(context->file_path), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the source location context set symbol operation used by this module and its
 * client applications.
 */
UmiStatus umi_source_location_context_set_symbol(UmiSourceLocationContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->symbol, sizeof(context->symbol), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the source location context set line operation used by this module and its
 * client applications.
 */
UmiStatus umi_source_location_context_set_line(UmiSourceLocationContext *context, uint32_t value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    context->line = value;
    context->revision += 1U;
    return UMI_STATUS_OK;
}
/*
 * Provide the source location context set column operation used by this module and its
 * client applications.
 */
UmiStatus umi_source_location_context_set_column(UmiSourceLocationContext *context, uint32_t value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    context->column = value;
    context->revision += 1U;
    return UMI_STATUS_OK;
}
/*
 * Provide the source location context set selection length operation used by this module
 * and its client applications.
 */
UmiStatus umi_source_location_context_set_selection_length(UmiSourceLocationContext *context, uint32_t value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    context->selection_length = value;
    context->revision += 1U;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiSourceLocationContextArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xeb90cc0ec5b4fbf3);
    schema = (schema ^ (uint64_t)sizeof(((UmiSourceLocationContext *)0)->workspace_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiSourceLocationContext *)0)->file_path)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiSourceLocationContext *)0)->symbol)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiSourceLocationContextArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiSourceLocationContext *)0)->workspace_id) - 1U +
        8U + sizeof(((UmiSourceLocationContext *)0)->file_path) - 1U +
        8U + sizeof(((UmiSourceLocationContext *)0)->symbol) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiSourceLocationContextArchiveWrite(UmiArchiveWriter *writer, const UmiSourceLocationContext *value)
{
    UmiArchiveWriteText(writer, value->workspace_id, sizeof(value->workspace_id));
    UmiArchiveWriteText(writer, value->file_path, sizeof(value->file_path));
    UmiArchiveWriteText(writer, value->symbol, sizeof(value->symbol));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->line);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->column);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->selection_length);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiSourceLocationContextArchiveRead(UmiArchiveReader *reader, UmiSourceLocationContext *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->workspace_id, sizeof(value->workspace_id));
    UmiArchiveReadText(reader, value->file_path, sizeof(value->file_path));
    UmiArchiveReadText(reader, value->symbol, sizeof(value->symbol));
    value->line = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->column = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->selection_length = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiSourceLocationContextArchiveValidate(const UmiSourceLocationContext *value)
{
    return umi_source_location_context_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_source_location_context_archive_encode, umi_source_location_context_archive_decode,
    UmiSourceLocationContext, UmiSourceLocationContextArchiveSchema, UmiSourceLocationContextArchiveBound, UmiSourceLocationContextArchiveWrite, UmiSourceLocationContextArchiveRead, UmiSourceLocationContextArchiveValidate)
