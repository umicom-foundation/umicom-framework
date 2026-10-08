/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/order_numbers.h
 * PURPOSE: Share lossless broker numeric decoding between order observation streams.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_IBKR_ORDER_NUMBERS_H
#define UMICOM_IBKR_ORDER_NUMBERS_H
#include "umicom/broker_connectivity/order_recovery.h"
bool UmiIbkrOrderNumberRead(const char *, bool, bool, UmiIbkrOrderNumber *);
#endif
