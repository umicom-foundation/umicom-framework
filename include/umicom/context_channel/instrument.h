/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/instrument.h
 *
 * PURPOSE:
 *   Define the canonical instrument context shared by applications and panels.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_INSTRUMENT_H
#define UMICOM_CONTEXT_CHANNEL_INSTRUMENT_H
#include "umicom/context_channel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiInstrumentContext {
    uint32_t structure_size;
    char instrument_id[UMI_CONTEXT_TEXT_CAPACITY];
    char symbol[UMI_CONTEXT_TEXT_CAPACITY];
    char venue[UMI_CONTEXT_TEXT_CAPACITY];
    char currency[32U];
    char asset_class[64U];
    char contract_id[UMI_CONTEXT_TEXT_CAPACITY];
    uint64_t revision;
} UmiInstrumentContext;
void umi_instrument_context_init(UmiInstrumentContext *context);
UmiStatus umi_instrument_context_validate(const UmiInstrumentContext *context);
UmiStatus umi_instrument_context_copy(UmiInstrumentContext *destination, const UmiInstrumentContext *source);
UmiStatus umi_instrument_context_set_instrument_id(UmiInstrumentContext *context, const char *value);
UmiStatus umi_instrument_context_set_symbol(UmiInstrumentContext *context, const char *value);
UmiStatus umi_instrument_context_set_venue(UmiInstrumentContext *context, const char *value);
UmiStatus umi_instrument_context_set_currency(UmiInstrumentContext *context, const char *value);
UmiStatus umi_instrument_context_set_asset_class(UmiInstrumentContext *context, const char *value);
UmiStatus umi_instrument_context_set_contract_id(UmiInstrumentContext *context, const char *value);
#ifdef __cplusplus
}
#endif
#endif
