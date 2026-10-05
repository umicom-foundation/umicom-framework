/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/instrument.c
 *
 * PURPOSE:
 *   Implement canonical instrument context validation and mutation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/instrument.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise instrument context from caller-provided values so later operations receive a
 * known state.
 */
void umi_instrument_context_init(UmiInstrumentContext *context)
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
 * Check that instrument context satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_instrument_context_validate(const UmiInstrumentContext *context)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->instrument_id, '\0', sizeof(context->instrument_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->symbol, '\0', sizeof(context->symbol)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->venue, '\0', sizeof(context->venue)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->currency, '\0', sizeof(context->currency)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->asset_class, '\0', sizeof(context->asset_class)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->contract_id, '\0', sizeof(context->contract_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || context->structure_size != sizeof(*context)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->instrument_id, sizeof(context->instrument_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->symbol, sizeof(context->symbol))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->venue, sizeof(context->venue))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->currency, sizeof(context->currency))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->asset_class, sizeof(context->asset_class))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->contract_id, sizeof(context->contract_id))) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
/*
 * Copy instrument context into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_instrument_context_copy(UmiInstrumentContext *destination, const UmiInstrumentContext *source)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_instrument_context_validate(source) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    *destination = *source;
    return UMI_STATUS_OK;
}
/*
 * Provide the instrument context set instrument id operation used by this module and its
 * client applications.
 */
UmiStatus umi_instrument_context_set_instrument_id(UmiInstrumentContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->instrument_id, sizeof(context->instrument_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the instrument context set symbol operation used by this module and its client
 * applications.
 */
UmiStatus umi_instrument_context_set_symbol(UmiInstrumentContext *context, const char *value)
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
 * Provide the instrument context set venue operation used by this module and its client
 * applications.
 */
UmiStatus umi_instrument_context_set_venue(UmiInstrumentContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->venue, sizeof(context->venue), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the instrument context set currency operation used by this module and its client
 * applications.
 */
UmiStatus umi_instrument_context_set_currency(UmiInstrumentContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->currency, sizeof(context->currency), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the instrument context set asset class operation used by this module and its
 * client applications.
 */
UmiStatus umi_instrument_context_set_asset_class(UmiInstrumentContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->asset_class, sizeof(context->asset_class), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the instrument context set contract id operation used by this module and its
 * client applications.
 */
UmiStatus umi_instrument_context_set_contract_id(UmiInstrumentContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->contract_id, sizeof(context->contract_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiInstrumentContextArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x90283808bdf1cf7c);
    schema = (schema ^ (uint64_t)sizeof(((UmiInstrumentContext *)0)->instrument_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiInstrumentContext *)0)->symbol)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiInstrumentContext *)0)->venue)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiInstrumentContext *)0)->currency)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiInstrumentContext *)0)->asset_class)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiInstrumentContext *)0)->contract_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiInstrumentContextArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiInstrumentContext *)0)->instrument_id) - 1U +
        8U + sizeof(((UmiInstrumentContext *)0)->symbol) - 1U +
        8U + sizeof(((UmiInstrumentContext *)0)->venue) - 1U +
        8U + sizeof(((UmiInstrumentContext *)0)->currency) - 1U +
        8U + sizeof(((UmiInstrumentContext *)0)->asset_class) - 1U +
        8U + sizeof(((UmiInstrumentContext *)0)->contract_id) - 1U +
        8U;
}
static void UmiInstrumentContextArchiveWrite(UmiArchiveWriter *writer, const UmiInstrumentContext *value)
{
    UmiArchiveWriteText(writer, value->instrument_id, sizeof(value->instrument_id));
    UmiArchiveWriteText(writer, value->symbol, sizeof(value->symbol));
    UmiArchiveWriteText(writer, value->venue, sizeof(value->venue));
    UmiArchiveWriteText(writer, value->currency, sizeof(value->currency));
    UmiArchiveWriteText(writer, value->asset_class, sizeof(value->asset_class));
    UmiArchiveWriteText(writer, value->contract_id, sizeof(value->contract_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiInstrumentContextArchiveRead(UmiArchiveReader *reader, UmiInstrumentContext *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->instrument_id, sizeof(value->instrument_id));
    UmiArchiveReadText(reader, value->symbol, sizeof(value->symbol));
    UmiArchiveReadText(reader, value->venue, sizeof(value->venue));
    UmiArchiveReadText(reader, value->currency, sizeof(value->currency));
    UmiArchiveReadText(reader, value->asset_class, sizeof(value->asset_class));
    UmiArchiveReadText(reader, value->contract_id, sizeof(value->contract_id));
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiInstrumentContextArchiveValidate(const UmiInstrumentContext *value)
{
    return umi_instrument_context_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_instrument_context_archive_encode, umi_instrument_context_archive_decode,
    UmiInstrumentContext, UmiInstrumentContextArchiveSchema, UmiInstrumentContextArchiveBound, UmiInstrumentContextArchiveWrite, UmiInstrumentContextArchiveRead, UmiInstrumentContextArchiveValidate)
