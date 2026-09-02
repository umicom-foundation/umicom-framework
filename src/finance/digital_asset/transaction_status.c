/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/transaction_status.c
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

#include "umicom/finance/digital_asset/transaction_status.h"

/* Centralise lifecycle policy so custody adapters cannot invent incompatible states. */
bool umi_digital_asset_transaction_status_can_transition(UmiDigitalTransactionState from, UmiDigitalTransactionState to)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (from == to) return true;
    /* Select the behaviour associated with the requested command or state value. */
    switch (from) {
        case UMI_DIGITAL_TX_CREATED:
            return to == UMI_DIGITAL_TX_SIGNING || to == UMI_DIGITAL_TX_CANCELLED;
        case UMI_DIGITAL_TX_SIGNING:
            return to == UMI_DIGITAL_TX_SUBMITTED || to == UMI_DIGITAL_TX_FAILED || to == UMI_DIGITAL_TX_CANCELLED;
        case UMI_DIGITAL_TX_SUBMITTED:
            return to == UMI_DIGITAL_TX_CONFIRMED || to == UMI_DIGITAL_TX_FAILED;
        case UMI_DIGITAL_TX_CONFIRMED:
            return to == UMI_DIGITAL_TX_FINAL || to == UMI_DIGITAL_TX_FAILED;
        default:
            return false;
    }
}

/* Terminal transactions cannot be silently re-opened by a network adapter. */
bool umi_digital_asset_transaction_status_terminal(UmiDigitalTransactionState state)
{
    return state == UMI_DIGITAL_TX_FINAL || state == UMI_DIGITAL_TX_FAILED || state == UMI_DIGITAL_TX_CANCELLED;
}
