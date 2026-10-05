/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/trade.c
 *
 * PURPOSE:
 *   Implement canonical trade context validation and mutation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/trade.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise trade context from caller-provided values so later operations receive a known
 * state.
 */
void umi_trade_context_init(UmiTradeContext *context)
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
/* Check that trade context satisfies its contract before another service relies on it. */
UmiStatus umi_trade_context_validate(const UmiTradeContext *context)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->trade_id, '\0', sizeof(context->trade_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->source_system, '\0', sizeof(context->source_system)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->product_type, '\0', sizeof(context->product_type)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->book_id, '\0', sizeof(context->book_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->counterparty_id, '\0', sizeof(context->counterparty_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || context->structure_size != sizeof(*context)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->trade_id, sizeof(context->trade_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->source_system, sizeof(context->source_system))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->product_type, sizeof(context->product_type))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->book_id, sizeof(context->book_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->counterparty_id, sizeof(context->counterparty_id))) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
/*
 * Copy trade context into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_trade_context_copy(UmiTradeContext *destination, const UmiTradeContext *source)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_trade_context_validate(source) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    *destination = *source;
    return UMI_STATUS_OK;
}
/*
 * Provide the trade context set trade id operation used by this module and its client
 * applications.
 */
UmiStatus umi_trade_context_set_trade_id(UmiTradeContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->trade_id, sizeof(context->trade_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the trade context set source system operation used by this module and its client
 * applications.
 */
UmiStatus umi_trade_context_set_source_system(UmiTradeContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->source_system, sizeof(context->source_system), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the trade context set product type operation used by this module and its client
 * applications.
 */
UmiStatus umi_trade_context_set_product_type(UmiTradeContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->product_type, sizeof(context->product_type), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the trade context set book id operation used by this module and its client
 * applications.
 */
UmiStatus umi_trade_context_set_book_id(UmiTradeContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->book_id, sizeof(context->book_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the trade context set counterparty id operation used by this module and its
 * client applications.
 */
UmiStatus umi_trade_context_set_counterparty_id(UmiTradeContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->counterparty_id, sizeof(context->counterparty_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the trade context set version operation used by this module and its client
 * applications.
 */
UmiStatus umi_trade_context_set_version(UmiTradeContext *context, uint64_t value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    context->version = value;
    context->revision += 1U;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTradeContextArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x7a2b762cdd6a702f);
    schema = (schema ^ (uint64_t)sizeof(((UmiTradeContext *)0)->trade_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTradeContext *)0)->source_system)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTradeContext *)0)->product_type)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTradeContext *)0)->book_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTradeContext *)0)->counterparty_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTradeContextArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTradeContext *)0)->trade_id) - 1U +
        8U + sizeof(((UmiTradeContext *)0)->source_system) - 1U +
        8U + sizeof(((UmiTradeContext *)0)->product_type) - 1U +
        8U + sizeof(((UmiTradeContext *)0)->book_id) - 1U +
        8U + sizeof(((UmiTradeContext *)0)->counterparty_id) - 1U +
        8U +
        8U;
}
static void UmiTradeContextArchiveWrite(UmiArchiveWriter *writer, const UmiTradeContext *value)
{
    UmiArchiveWriteText(writer, value->trade_id, sizeof(value->trade_id));
    UmiArchiveWriteText(writer, value->source_system, sizeof(value->source_system));
    UmiArchiveWriteText(writer, value->product_type, sizeof(value->product_type));
    UmiArchiveWriteText(writer, value->book_id, sizeof(value->book_id));
    UmiArchiveWriteText(writer, value->counterparty_id, sizeof(value->counterparty_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->version);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiTradeContextArchiveRead(UmiArchiveReader *reader, UmiTradeContext *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->trade_id, sizeof(value->trade_id));
    UmiArchiveReadText(reader, value->source_system, sizeof(value->source_system));
    UmiArchiveReadText(reader, value->product_type, sizeof(value->product_type));
    UmiArchiveReadText(reader, value->book_id, sizeof(value->book_id));
    UmiArchiveReadText(reader, value->counterparty_id, sizeof(value->counterparty_id));
    value->version = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiTradeContextArchiveValidate(const UmiTradeContext *value)
{
    return umi_trade_context_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_trade_context_archive_encode, umi_trade_context_archive_decode,
    UmiTradeContext, UmiTradeContextArchiveSchema, UmiTradeContextArchiveBound, UmiTradeContextArchiveWrite, UmiTradeContextArchiveRead, UmiTradeContextArchiveValidate)
