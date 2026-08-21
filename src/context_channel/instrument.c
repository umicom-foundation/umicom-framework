/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/instrument.c
 *
 * PURPOSE:
 *   Implement canonical instrument context validation and mutation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/instrument.h"
#include <string.h>
void umi_instrument_context_init(UmiInstrumentContext *context)
{
    if (context == NULL) return;
    memset(context, 0, sizeof(*context));
    context->structure_size = (uint32_t)sizeof(*context);
    context->revision = 1U;
}
UmiStatus umi_instrument_context_validate(const UmiInstrumentContext *context)
{
    if (context == NULL || context->structure_size != sizeof(*context)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(context->instrument_id, sizeof(context->instrument_id))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(context->symbol, sizeof(context->symbol))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(context->venue, sizeof(context->venue))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(context->currency, sizeof(context->currency))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(context->asset_class, sizeof(context->asset_class))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(context->contract_id, sizeof(context->contract_id))) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus umi_instrument_context_copy(UmiInstrumentContext *destination, const UmiInstrumentContext *source)
{
    if (destination == NULL || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_instrument_context_validate(source) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    *destination = *source;
    return UMI_STATUS_OK;
}
UmiStatus umi_instrument_context_set_instrument_id(UmiInstrumentContext *context, const char *value)
{
    UmiStatus status;
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->instrument_id, sizeof(context->instrument_id), value);
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
UmiStatus umi_instrument_context_set_symbol(UmiInstrumentContext *context, const char *value)
{
    UmiStatus status;
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->symbol, sizeof(context->symbol), value);
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
UmiStatus umi_instrument_context_set_venue(UmiInstrumentContext *context, const char *value)
{
    UmiStatus status;
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->venue, sizeof(context->venue), value);
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
UmiStatus umi_instrument_context_set_currency(UmiInstrumentContext *context, const char *value)
{
    UmiStatus status;
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->currency, sizeof(context->currency), value);
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
UmiStatus umi_instrument_context_set_asset_class(UmiInstrumentContext *context, const char *value)
{
    UmiStatus status;
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->asset_class, sizeof(context->asset_class), value);
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
UmiStatus umi_instrument_context_set_contract_id(UmiInstrumentContext *context, const char *value)
{
    UmiStatus status;
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->contract_id, sizeof(context->contract_id), value);
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
