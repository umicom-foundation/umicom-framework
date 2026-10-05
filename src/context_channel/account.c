/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/account.c
 *
 * PURPOSE:
 *   Implement canonical account context validation and mutation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/account.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise account context from caller-provided values so later operations receive a
 * known state.
 */
void umi_account_context_init(UmiAccountContext *context)
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
/* Check that account context satisfies its contract before another service relies on it. */
UmiStatus umi_account_context_validate(const UmiAccountContext *context)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->account_id, '\0', sizeof(context->account_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->organisation_id, '\0', sizeof(context->organisation_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->book_id, '\0', sizeof(context->book_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->currency, '\0', sizeof(context->currency)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->account_type, '\0', sizeof(context->account_type)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->environment, '\0', sizeof(context->environment)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || context->structure_size != sizeof(*context)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->account_id, sizeof(context->account_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->organisation_id, sizeof(context->organisation_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->book_id, sizeof(context->book_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->currency, sizeof(context->currency))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->account_type, sizeof(context->account_type))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->environment, sizeof(context->environment))) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
/*
 * Copy account context into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_account_context_copy(UmiAccountContext *destination, const UmiAccountContext *source)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_account_context_validate(source) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    *destination = *source;
    return UMI_STATUS_OK;
}
/*
 * Provide the account context set account id operation used by this module and its client
 * applications.
 */
UmiStatus umi_account_context_set_account_id(UmiAccountContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->account_id, sizeof(context->account_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the account context set organisation id operation used by this module and its
 * client applications.
 */
UmiStatus umi_account_context_set_organisation_id(UmiAccountContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->organisation_id, sizeof(context->organisation_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the account context set book id operation used by this module and its client
 * applications.
 */
UmiStatus umi_account_context_set_book_id(UmiAccountContext *context, const char *value)
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
 * Provide the account context set currency operation used by this module and its client
 * applications.
 */
UmiStatus umi_account_context_set_currency(UmiAccountContext *context, const char *value)
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
 * Provide the account context set account type operation used by this module and its
 * client applications.
 */
UmiStatus umi_account_context_set_account_type(UmiAccountContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->account_type, sizeof(context->account_type), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the account context set environment operation used by this module and its client
 * applications.
 */
UmiStatus umi_account_context_set_environment(UmiAccountContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->environment, sizeof(context->environment), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAccountContextArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x4bf948d7effa9f11);
    schema = (schema ^ (uint64_t)sizeof(((UmiAccountContext *)0)->account_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAccountContext *)0)->organisation_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAccountContext *)0)->book_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAccountContext *)0)->currency)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAccountContext *)0)->account_type)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAccountContext *)0)->environment)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAccountContextArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAccountContext *)0)->account_id) - 1U +
        8U + sizeof(((UmiAccountContext *)0)->organisation_id) - 1U +
        8U + sizeof(((UmiAccountContext *)0)->book_id) - 1U +
        8U + sizeof(((UmiAccountContext *)0)->currency) - 1U +
        8U + sizeof(((UmiAccountContext *)0)->account_type) - 1U +
        8U + sizeof(((UmiAccountContext *)0)->environment) - 1U +
        8U;
}
static void UmiAccountContextArchiveWrite(UmiArchiveWriter *writer, const UmiAccountContext *value)
{
    UmiArchiveWriteText(writer, value->account_id, sizeof(value->account_id));
    UmiArchiveWriteText(writer, value->organisation_id, sizeof(value->organisation_id));
    UmiArchiveWriteText(writer, value->book_id, sizeof(value->book_id));
    UmiArchiveWriteText(writer, value->currency, sizeof(value->currency));
    UmiArchiveWriteText(writer, value->account_type, sizeof(value->account_type));
    UmiArchiveWriteText(writer, value->environment, sizeof(value->environment));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiAccountContextArchiveRead(UmiArchiveReader *reader, UmiAccountContext *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->account_id, sizeof(value->account_id));
    UmiArchiveReadText(reader, value->organisation_id, sizeof(value->organisation_id));
    UmiArchiveReadText(reader, value->book_id, sizeof(value->book_id));
    UmiArchiveReadText(reader, value->currency, sizeof(value->currency));
    UmiArchiveReadText(reader, value->account_type, sizeof(value->account_type));
    UmiArchiveReadText(reader, value->environment, sizeof(value->environment));
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiAccountContextArchiveValidate(const UmiAccountContext *value)
{
    return umi_account_context_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_account_context_archive_encode, umi_account_context_archive_decode,
    UmiAccountContext, UmiAccountContextArchiveSchema, UmiAccountContextArchiveBound, UmiAccountContextArchiveWrite, UmiAccountContextArchiveRead, UmiAccountContextArchiveValidate)
