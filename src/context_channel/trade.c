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
#include <string.h>
void umi_trade_context_init(UmiTradeContext *context)
{
    if (context == NULL) return;
    memset(context, 0, sizeof(*context));
    context->structure_size = (uint32_t)sizeof(*context);
    context->revision = 1U;
}
UmiStatus umi_trade_context_validate(const UmiTradeContext *context)
{
    if (context == NULL || context->structure_size != sizeof(*context)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(context->trade_id, sizeof(context->trade_id))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(context->source_system, sizeof(context->source_system))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(context->product_type, sizeof(context->product_type))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(context->book_id, sizeof(context->book_id))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(context->counterparty_id, sizeof(context->counterparty_id))) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus umi_trade_context_copy(UmiTradeContext *destination, const UmiTradeContext *source)
{
    if (destination == NULL || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_trade_context_validate(source) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    *destination = *source;
    return UMI_STATUS_OK;
}
UmiStatus umi_trade_context_set_trade_id(UmiTradeContext *context, const char *value)
{
    UmiStatus status;
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->trade_id, sizeof(context->trade_id), value);
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
UmiStatus umi_trade_context_set_source_system(UmiTradeContext *context, const char *value)
{
    UmiStatus status;
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->source_system, sizeof(context->source_system), value);
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
UmiStatus umi_trade_context_set_product_type(UmiTradeContext *context, const char *value)
{
    UmiStatus status;
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->product_type, sizeof(context->product_type), value);
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
UmiStatus umi_trade_context_set_book_id(UmiTradeContext *context, const char *value)
{
    UmiStatus status;
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->book_id, sizeof(context->book_id), value);
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
UmiStatus umi_trade_context_set_counterparty_id(UmiTradeContext *context, const char *value)
{
    UmiStatus status;
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->counterparty_id, sizeof(context->counterparty_id), value);
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
UmiStatus umi_trade_context_set_version(UmiTradeContext *context, uint64_t value)
{
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    context->version = value;
    context->revision += 1U;
    return UMI_STATUS_OK;
}
