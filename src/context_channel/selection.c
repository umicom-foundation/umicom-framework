/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/selection.c
 *
 * PURPOSE:
 *   Implement canonical selection context validation and mutation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../base/conditional_archive_internal.h"
#include "umicom/context_channel/selection.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise selection context from caller-provided values so later operations receive a
 * known state.
 */
void umi_selection_context_init(UmiSelectionContext *context)
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
/* Check that selection context satisfies its contract before another service relies on it. */
UmiStatus umi_selection_context_validate(const UmiSelectionContext *context)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->selection_id, '\0', sizeof(context->selection_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->selection_type, '\0', sizeof(context->selection_type)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->primary_id, '\0', sizeof(context->primary_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->secondary_id, '\0', sizeof(context->secondary_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || context->structure_size != sizeof(*context)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->selection_id, sizeof(context->selection_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->selection_type, sizeof(context->selection_type))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->primary_id, sizeof(context->primary_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->secondary_id, sizeof(context->secondary_id))) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
/*
 * Copy selection context into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_selection_context_copy(UmiSelectionContext *destination, const UmiSelectionContext *source)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_selection_context_validate(source) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    *destination = *source;
    return UMI_STATUS_OK;
}
/*
 * Provide the selection context set selection id operation used by this module and its
 * client applications.
 */
UmiStatus umi_selection_context_set_selection_id(UmiSelectionContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* A revision is a change token used by editors and reviewed snapshots.
     * Refuse an exhausted token before changing either the owner or its nested value. */
    if (context->revision == UINT64_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = umi_context_copy_text(context->selection_id, sizeof(context->selection_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the selection context set selection type operation used by this module and its
 * client applications.
 */
UmiStatus umi_selection_context_set_selection_type(UmiSelectionContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* A revision is a change token used by editors and reviewed snapshots.
     * Refuse an exhausted token before changing either the owner or its nested value. */
    if (context->revision == UINT64_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = umi_context_copy_text(context->selection_type, sizeof(context->selection_type), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the selection context set primary id operation used by this module and its
 * client applications.
 */
UmiStatus umi_selection_context_set_primary_id(UmiSelectionContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* A revision is a change token used by editors and reviewed snapshots.
     * Refuse an exhausted token before changing either the owner or its nested value. */
    if (context->revision == UINT64_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = umi_context_copy_text(context->primary_id, sizeof(context->primary_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the selection context set secondary id operation used by this module and its
 * client applications.
 */
UmiStatus umi_selection_context_set_secondary_id(UmiSelectionContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* A revision is a change token used by editors and reviewed snapshots.
     * Refuse an exhausted token before changing either the owner or its nested value. */
    if (context->revision == UINT64_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = umi_context_copy_text(context->secondary_id, sizeof(context->secondary_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the selection context set index operation used by this module and its client
 * applications.
 */
UmiStatus umi_selection_context_set_index(UmiSelectionContext *context, uint64_t value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* A revision is a change token used by editors and reviewed snapshots.
     * Refuse an exhausted token before changing either the owner or its nested value. */
    if (context->revision == UINT64_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    context->index = value;
    context->revision += 1U;
    return UMI_STATUS_OK;
}
/*
 * Return the number of records represented by selection context set without changing their
 * state.
 */
UmiStatus umi_selection_context_set_count(UmiSelectionContext *context, uint64_t value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* A revision is a change token used by editors and reviewed snapshots.
     * Refuse an exhausted token before changing either the owner or its nested value. */
    if (context->revision == UINT64_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    context->count = value;
    context->revision += 1U;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiSelectionContextArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xfedc3c04e6da23a0);
    schema = (schema ^ (uint64_t)sizeof(((UmiSelectionContext *)0)->selection_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiSelectionContext *)0)->selection_type)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiSelectionContext *)0)->primary_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiSelectionContext *)0)->secondary_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiSelectionContextArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiSelectionContext *)0)->selection_id) - 1U +
        8U + sizeof(((UmiSelectionContext *)0)->selection_type) - 1U +
        8U + sizeof(((UmiSelectionContext *)0)->primary_id) - 1U +
        8U + sizeof(((UmiSelectionContext *)0)->secondary_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiSelectionContextArchiveWrite(UmiArchiveWriter *writer, const UmiSelectionContext *value)
{
    UmiArchiveWriteText(writer, value->selection_id, sizeof(value->selection_id));
    UmiArchiveWriteText(writer, value->selection_type, sizeof(value->selection_type));
    UmiArchiveWriteText(writer, value->primary_id, sizeof(value->primary_id));
    UmiArchiveWriteText(writer, value->secondary_id, sizeof(value->secondary_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->index);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiSelectionContextArchiveRead(UmiArchiveReader *reader, UmiSelectionContext *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->selection_id, sizeof(value->selection_id));
    UmiArchiveReadText(reader, value->selection_type, sizeof(value->selection_type));
    UmiArchiveReadText(reader, value->primary_id, sizeof(value->primary_id));
    UmiArchiveReadText(reader, value->secondary_id, sizeof(value->secondary_id));
    value->index = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiSelectionContextArchiveValidate(const UmiSelectionContext *value)
{
    return umi_selection_context_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_selection_context_archive_encode, umi_selection_context_archive_decode,
    UmiSelectionContext, UmiSelectionContextArchiveSchema, UmiSelectionContextArchiveBound, UmiSelectionContextArchiveWrite, UmiSelectionContextArchiveRead, UmiSelectionContextArchiveValidate)

/* SDK callers choose staging storage independently of their thread stack.
 * Reuse this owner\'s exact schema, reader and validation before publishing
 * a destination; decoded values remain data, never execution authority. */
UMI_DEFINE_VALUE_ARCHIVE_WORKSPACE(umi_selection_context_archive_decode_with_workspace, UmiSelectionContext,
    UmiSelectionContextArchiveSchema, UmiSelectionContextArchiveBound, UmiSelectionContextArchiveRead, UmiSelectionContextArchiveValidate)

/* Restore this typed value only while its encoded state still matches the
 * caller's reviewed snapshot. The shared guard checks every encoded byte;
 * domain decoding and validation stay with this owner. */
UMI_DEFINE_CONDITIONAL_ARCHIVE(umi_selection_context_archive_decode_if_unchanged, umi_selection_context_archive_encode,
    UmiSelectionContext, UmiSelectionContextArchiveSchema, UmiSelectionContextArchiveBound, UmiSelectionContextArchiveRead, UmiSelectionContextArchiveValidate)
