/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/instrument.h
 *
 * PURPOSE:
 *   Define the canonical instrument context shared by applications and panels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_INSTRUMENT_H
#define UMICOM_CONTEXT_CHANNEL_INSTRUMENT_H
#include "umicom/context_channel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the instrument context data shared with callers of this public contract.
 */
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
/**
 * Initialise instrument context from caller-provided values so later operations receive a
 * known state.
 */
void umi_instrument_context_init(UmiInstrumentContext *context);
/**
 * Check that instrument context satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_instrument_context_validate(const UmiInstrumentContext *context);
/**
 * Copy instrument context into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_instrument_context_copy(UmiInstrumentContext *destination, const UmiInstrumentContext *source);
/**
 * Provide the instrument context set instrument id operation used by this module and its
 * client applications.
 */
UmiStatus umi_instrument_context_set_instrument_id(UmiInstrumentContext *context, const char *value);
/**
 * Provide the instrument context set symbol operation used by this module and its client
 * applications.
 */
UmiStatus umi_instrument_context_set_symbol(UmiInstrumentContext *context, const char *value);
/**
 * Provide the instrument context set venue operation used by this module and its client
 * applications.
 */
UmiStatus umi_instrument_context_set_venue(UmiInstrumentContext *context, const char *value);
/**
 * Provide the instrument context set currency operation used by this module and its client
 * applications.
 */
UmiStatus umi_instrument_context_set_currency(UmiInstrumentContext *context, const char *value);
/**
 * Provide the instrument context set asset class operation used by this module and its
 * client applications.
 */
UmiStatus umi_instrument_context_set_asset_class(UmiInstrumentContext *context, const char *value);
/**
 * Provide the instrument context set contract id operation used by this module and its
 * client applications.
 */
UmiStatus umi_instrument_context_set_contract_id(UmiInstrumentContext *context, const char *value);
#ifdef __cplusplus
}
#endif
#endif
