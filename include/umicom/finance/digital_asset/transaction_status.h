/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/digital_asset/transaction_status.h
 *
 * PURPOSE:
 *   Enforce deterministic lifecycle transitions for provider-neutral digital-asset transactions.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_TRANSACTION_STATUS_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_TRANSACTION_STATUS_H

#include <stdbool.h>
#include "umicom/finance/digital_asset/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Return true when a digital transaction may move between lifecycle states. */
bool umi_digital_asset_transaction_status_can_transition(UmiDigitalTransactionState from, UmiDigitalTransactionState to);

/* Return true for immutable final, failed or cancelled transaction states. */
bool umi_digital_asset_transaction_status_terminal(UmiDigitalTransactionState state);

#ifdef __cplusplus
}
#endif

#endif
