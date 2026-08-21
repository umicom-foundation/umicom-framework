/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/trade.h
 *
 * PURPOSE:
 *   Define the canonical trade context shared by applications and panels.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_TRADE_H
#define UMICOM_CONTEXT_CHANNEL_TRADE_H
#include "umicom/context_channel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTradeContext {
    uint32_t structure_size;
    char trade_id[UMI_CONTEXT_TEXT_CAPACITY];
    char source_system[64U];
    char product_type[96U];
    char book_id[UMI_CONTEXT_TEXT_CAPACITY];
    char counterparty_id[UMI_CONTEXT_TEXT_CAPACITY];
    uint64_t version;
    uint64_t revision;
} UmiTradeContext;
void umi_trade_context_init(UmiTradeContext *context);
UmiStatus umi_trade_context_validate(const UmiTradeContext *context);
UmiStatus umi_trade_context_copy(UmiTradeContext *destination, const UmiTradeContext *source);
UmiStatus umi_trade_context_set_trade_id(UmiTradeContext *context, const char *value);
UmiStatus umi_trade_context_set_source_system(UmiTradeContext *context, const char *value);
UmiStatus umi_trade_context_set_product_type(UmiTradeContext *context, const char *value);
UmiStatus umi_trade_context_set_book_id(UmiTradeContext *context, const char *value);
UmiStatus umi_trade_context_set_counterparty_id(UmiTradeContext *context, const char *value);
UmiStatus umi_trade_context_set_version(UmiTradeContext *context, uint64_t value);
#ifdef __cplusplus
}
#endif
#endif
